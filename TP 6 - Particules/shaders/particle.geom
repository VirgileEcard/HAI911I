#version 430

layout ( points ) in;
layout ( triangle_strip, max_vertices = 4 ) out;

// Les matrices peuvent être réutilisées
uniform mat4 mvp;

// Variables d'entrées
in vec3 initialVertPos[];
in float particleAge[];
in float particleAgeMax[];
in flat int particleType[];

// Variables de sorties
out vec2 UV;
out float ageRatio;
out flat int particleType_out;

void main(void) {
    // Taille d'une particule
    float scale = 0.05;
    if(particleType[0] == 2) scale = 0.01;
    
    // Position centrale de la particule
    vec4 center = gl_in[0].gl_Position;
    particleType_out = particleType[0];

    // Coin bas-gauche
    gl_Position = center + vec4(-scale, -scale, 0.0, 0.0);
    UV = vec2(0.0, 0.0);
    EmitVertex();
    
    // Coin bas-droit
    gl_Position = center + vec4(scale, -scale, 0.0, 0.0);
    UV = vec2(1.0, 0.0);
    EmitVertex();
    
    // Coin haut-gauche
    gl_Position = center + vec4(-scale, scale, 0.0, 0.0);
    UV = vec2(0.0, 1.0);
    EmitVertex();
    
    // Coin haut-droit
    gl_Position = center + vec4(scale, scale, 0.0, 0.0);
    UV = vec2(1.0, 1.0);
    EmitVertex();
    
    EndPrimitive();
}