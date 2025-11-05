#include "systemwindow.h"
#include <QMatrix4x4>
#include <QOpenGLShader>
#include <QScreen>
#include <QtMath>
#include <QDebug>
#include <QMouseEvent>
#include <QOpenGLFunctions>
#include <QOpenGLFunctions_4_3_Core>
#include <QDateTime>

void ParticleSystemWindow::initialize() {
    m_context = new QOpenGLContext(this);
    m_context->create();

    //Switch to OpenGL context
    m_context->makeCurrent(this);
    gl43 = QOpenGLContext::currentContext()->versionFunctions<QOpenGLFunctions_4_3_Core>();
    if (!gl43) {
        qFatal("Impossible to initialize OpenGLFunction 4.3 Core.");
    }
    gl43->initializeOpenGLFunctions();
    srand(static_cast<unsigned int>(QDateTime::currentMSecsSinceEpoch() & 0xFFFFFFFF));

    qDebug() << "OpenGL version:" << (const char*)glGetString(GL_VERSION);

    particles.resize(numParticles);
    for (auto& p : particles) p.init();

    qDebug() << "Particles initialized:" << particles.size();
    
    vao.create();
    vao.bind();

    ssbo = QOpenGLBuffer(QOpenGLBuffer::VertexBuffer);
    ssbo.create();
    ssbo.bind();
    ssbo.setUsagePattern(QOpenGLBuffer::DynamicDraw);
    ssbo.allocate(particles.data(), numParticles * sizeof(Particle));

    // Lier le SSBO au binding point 0 pour le compute shader
    gl43->glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssbo.bufferId());

    qDebug() << "SSBO created with" << numParticles << "particles";

    m_camera.setAspectRatio(width() / float(height()));

    /*program = new QOpenGLShaderProgram(this);
    program->addShaderFromSourceFile(QOpenGLShader::Vertex, "../shaders/particle.vert");
    program->addShaderFromSourceFile(QOpenGLShader::Geometry, "../shaders/particle.geom");
    program->addShaderFromSourceFile(QOpenGLShader::Fragment, "../shaders/particle.frag");
    program->link();*/

    // Charger la texture de particule
    particleTexture = new QOpenGLTexture(QImage("../data/smoke.png").mirrored());
    particleTexture->setMinificationFilter(QOpenGLTexture::LinearMipMapLinear);
    particleTexture->setMagnificationFilter(QOpenGLTexture::Linear);
    particleTexture->setWrapMode(QOpenGLTexture::ClampToEdge);

    program = new QOpenGLShaderProgram(this);
    
    if (!program->addShaderFromSourceFile(QOpenGLShader::Vertex,
        "../shaders/particle.vert")) {
        qDebug() << "Vertex shader error:" << program->log();
    } else {
        qDebug() << "Vertex shader OK";
    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Geometry,
        "../shaders/particle.geom")) {
        qDebug() << "Geometry shader error:" << program->log();
    } else {
        qDebug() << "Geometry shader OK";
    }

    if (!program->addShaderFromSourceFile(QOpenGLShader::Fragment,
        "../shaders/particle.frag")) {
        qDebug() << "Fragment shader error:" << program->log();
    } else {
        qDebug() << "Fragment shader OK";
    }

    if (!program->link()) {
        qDebug() << "Shader link error:" << program->log();
    } else {
        qDebug() << "Shaders linked successfully";
    }


    program->bind();

    matrixUniform = program->uniformLocation("mvp");
    qDebug() << "Matrix uniform location:" << matrixUniform;

    // ============ Programme de compute ============
    computeProgram = new QOpenGLShaderProgram(this);
    
    if (!computeProgram->addShaderFromSourceFile(QOpenGLShader::Compute,
        "../shaders/particle.comp")) {
        qDebug() << "Compute shader error:" << computeProgram->log();
    }
    
    if (!computeProgram->link()) {
        qDebug() << "Compute program link error:" << computeProgram->log();
    } else {
        qDebug() << "Compute program linked successfully";
    }
    
    computeProgram->bind();
    deltaTimeUniform = computeProgram->uniformLocation("deltaTime");
    qDebug() << "DeltaTime uniform location:" << deltaTimeUniform;

    // ============ Fin du compute ============

    glEnable(GL_PROGRAM_POINT_SIZE);
    glPointSize(5.0f);

    timer.start();
}

void ParticleSystemWindow::render() {
    const qreal retinaScale = devicePixelRatio();
    glViewport(0, 0, width() * retinaScale, height() * retinaScale);
    glEnable(GL_DEPTH_TEST);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(1.f, 1.0f, 1.0f, 1.0f);

    // Calculer le temps écoulé depuis le dernier frame (en secondes)
    float currentTime = timer.elapsed() / 1000.0f;  // Convertir ms en secondes
    float deltaTime = currentTime - lastFrameTime;
    lastFrameTime = currentTime;

    // ============ Phase de compute ============
    computeProgram->bind();
    computeProgram->setUniformValue(deltaTimeUniform, deltaTime);
    
    // Lancer le compute shader
    // numParticles particules / 256 particules par workgroup
    int numWorkGroups = (numParticles + 255) / 256;
    gl43->glDispatchCompute(numWorkGroups, 1, 1);
    
    // Attendre que les calculs soient terminés
    gl43->glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    
    // ============ Phase de rendu ============
    vao.bind();
    ssbo.bind();
    
    // Configuration des attributs de vertex
    // Offset 0 = position (vec3)
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)0);
    glEnableVertexAttribArray(0);

    // Offset 12 = age (float) = 4 bytes
    glVertexAttribPointer(1, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Offset 28 = ageMax (float) = 4 bytes
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);

    gl43->glVertexAttribIPointer(3, 1, GL_INT, sizeof(Particle), (void*)(offsetof(Particle, type)));
    glEnableVertexAttribArray(3);

    program->bind();

    glActiveTexture(GL_TEXTURE0);
    particleTexture->bind();
    
    QMatrix4x4 mvp = m_camera.projectionMatrix() * m_camera.viewMatrix();
    program->setUniformValue(matrixUniform, mvp);

    glDrawArrays(GL_POINTS, 0, numParticles);

    program->release();
}


