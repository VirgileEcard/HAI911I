#ifndef SYSTEMWINDOW_H
#define SYSTEMWINDOW_H

#include "openglwindow.h"

#include <QOpenGLShaderProgram>
#include <QOpenGLBuffer>
#include <QOpenGLVertexArrayObject>
#include <QOpenGLTexture>
#include <QElapsedTimer>


#include "particle.h"

class ParticleSystemWindow : public OpenGLWindow {
public:
    using OpenGLWindow::OpenGLWindow;

    void initialize() override;
    void render() override;
    ~ParticleSystemWindow()
    {
        delete program;
        delete particleTexture;
    }


private:
    std::vector<Particle> particles;
    std::vector<GLfloat> particlePositions;

    QOpenGLBuffer ssbo;
    QOpenGLVertexArrayObject vao;

    QOpenGLShaderProgram* program = nullptr;
    QOpenGLShaderProgram* computeProgram = nullptr;
    QOpenGLFunctions_4_3_Core* gl43 = nullptr;

    GLint matrixUniform = -1;
    GLint deltaTimeUniform = -1;

    const int numParticles = 2000;

    QOpenGLTexture* particleTexture = nullptr;

    QElapsedTimer timer;
    float lastFrameTime = 0.0f;

};


#endif // SYSTEMWINDOW_H
