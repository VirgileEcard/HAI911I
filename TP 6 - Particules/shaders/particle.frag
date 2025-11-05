#version 430

out vec4 fragColor;

// Input du geometry shader
in vec2 UV;
in float ageRatio;
in flat int particleType_out;

//uniform vec4 color = vec4(1.0, 0.0, 0.0, 1.0);
uniform sampler2D particleTexture;

void main(void)
{
    vec4 texColor = texture(particleTexture, UV);
    
    if (texColor.a < 0.1) {
        discard;
    }
    
    vec3 color;
    
    if (particleType_out == 0) { // Feu
        vec3 youngColor = vec3(1.0, 1.0, 0.3);
        vec3 middleColor = vec3(1.0, 0.5, 0.0);
        vec3 oldColor = vec3(1.0, 0.0, 0.0);
        
        if (ageRatio < 0.5) {
            color = mix(youngColor, middleColor, ageRatio * 2.0);
        } else {
            color = mix(middleColor, oldColor, (ageRatio - 0.5) * 2.0);
        }
        color = vec3(0.8, 0.0, 0.0);
    } else if (particleType_out == 1) { // Fumée
        color = mix(vec3(0.9, 0.9, 0.9), vec3(0.2, 0.2, 0.2), ageRatio);
    	color = vec3(0.5, 0.5, 0.5);
    } else { // Etincelles
        color = mix(vec3(1.0, 1.0, 1.0), vec3(1.0, 0.8, 0.0), ageRatio);
    	color = vec3(0.8, 0.5, 0.0);
    }
    
    float alpha = 1.0 - ageRatio * 0.7;
    
    fragColor = vec4(texColor.rgb * color, texColor.a * alpha);
}