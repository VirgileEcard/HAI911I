#version 430
layout (location=0) in vec3 position;
layout (location=1) in float age;
layout (location=2) in float ageMax;
layout (location=3) in int type;

uniform mat4 mvp;

// Si utilisation du temps
uniform float time = 0;

out vec3 initialVertPos;
out float particleAge;
out float particleAgeMax;
out flat int particleType;

void main(void)
{
    // Variables envoyées au geometry shader
    initialVertPos = position;// + vec3(0, 0, -0.05) * time;
    particleAge = age;
    particleAgeMax = ageMax;
    particleType = type;
    
    // Position dans le viewport
    gl_Position = mvp * vec4(initialVertPos, 1.0);
}