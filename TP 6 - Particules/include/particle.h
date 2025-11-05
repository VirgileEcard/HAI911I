#ifndef PARTICLE_H
#define PARTICLE_H
#include <QVector3D>
#include <cmath>
struct Particle {
    QVector3D pos;      // 12 bytes (offset 0)
    float age;          // 4 bytes  (offset 12)
    QVector3D speed;    // 12 bytes (offset 16)
    float ageMax;       // 4 bytes  (offset 28)
    int type;           // 4 bytes  (offset 32)
    float padding[3];   // 12 bytes pour alignement à 48 bytes

    void init() {
        pos = QVector3D(0.0, 0.0, 0.0);

        int random = rand() % 100;
        if (random < 40) {
            type = 0;  // Feu
        } else if (random < 90) {
            type = 1;  // Fumée
        } else {
            type = 2;  // Etincelles
        }

        float angle = 2.0 * M_PI * rand() / RAND_MAX;
        float norm = 0.04 * rand() / RAND_MAX;
        
        switch(type) {
            case 0: 
                speed = QVector3D(norm * cos(angle), 
                                 norm * sin(angle),
                                 1.5 + 0.5 * rand() / RAND_MAX);
                ageMax = 3.0f + (3.0f * rand() / float(RAND_MAX));
                break;
            case 1:
                speed = QVector3D(norm * 2.0 * cos(angle), 
                                 norm * 2.0 * sin(angle),
                                 0.3 + 0.3 * rand() / RAND_MAX);
                ageMax = 8.0f + (5.0f * rand() / float(RAND_MAX));
                break;
            case 2:
                speed = QVector3D(norm * 5.0 * cos(angle), 
                                 norm * 5.0 * sin(angle),
                                 2.0 + 1.0 * rand() / RAND_MAX);
                ageMax = 2.0f + (2.0f * rand() / float(RAND_MAX));
                break;
        }

        age = 0.0f;

    }

    void animate() {
        speed[2] -= 0.05f;
        pos += 0.1f * speed;

        if (pos[2] < 0.0) {
            speed[2] = -0.8 * speed[2];
            pos[2] = 0.0;
        }
        age += 1.0f;
        if(age >= ageMax) init();
    }

    void animate(float deltaTime) {
        const float gravity = 5.0f;  // Had hoc
        
        // Appliquer la gravité
        speed[2] -= gravity * deltaTime;
        
        // Mettre à jour la position
        pos += speed * deltaTime;
        
        // Rebond au sol
        if (pos[2] < 0.0) {
            speed[2] = -0.8 * speed[2];
            pos[2] = 0.0;
        }
        
        // Incrémenter l'âge en secondes
        age += deltaTime;
        
        // Réinitialiser si trop vieille
        if(age >= ageMax) {
            init();
        }
    }
};

#endif // PARTICLE_H
