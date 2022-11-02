


#version 450
#extension GL_ARB_separate_shader_objects : enable
#define PI 3.14159

layout(binding = 0) uniform SceneUniforms {
    vec4 uLight;
    vec4 cLight;
    vec4 pLight;
    vec4 cLight1;
    vec4 cAmb;
    vec4 pCam;
    vec4 uSpot;
    mat4 camera;

} scene;
layout(binding = 1) uniform BodyUniforms {
    mat4 modeling;
    uvec4 texIndices;
    vec4 cSpec;
} body;
layout(binding = 2) uniform sampler2D samplers[3];

layout(location = 0) in vec2 st;
layout(location = 1) in vec4 dNorm;
layout(location = 2) in vec4 dLight;
layout(location = 3) in vec4 dCam;




layout(location = 0) out vec4 outColor;

void main() {
    vec4 cDiff = texture(samplers[body.texIndices[0]], st);
    vec4 uNorm = normalize(dNorm);
    vec4 uCam = normalize(dCam);
    vec4 uLight1 = normalize(dLight);

    vec4 uRefl = 2 * (uNorm * scene.uLight) * uNorm - scene.uLight;
    vec4 uRefl1 = 2 * (uNorm * uLight1) * uNorm - uLight1;


    float iDiff = max(0.0, dot(uNorm, scene.uLight));
    float iDiff1 = max(0.0, dot(uNorm, uLight1));

    // make sure its not zero and also iDiff is larger than zero
    float iSpec = max(0.0, dot(uRefl, uCam)) * float(iDiff > 0.0);
    float iSpec1 = max(0.0, dot(uRefl1, uCam)) * float(iDiff1 > 0.0);

    float dls = dot(scene.uSpot, -uLight1); //dot product of light and spot
    float hA = PI / 12.0; //half angle

    vec4 cLight1 = float(dls > cos(hA)) * scene.cLight1;
    
    outColor = iDiff * scene.cLight * cDiff 
             + iDiff1 * cLight1 * cDiff 
             + scene.cAmb * cDiff
             + pow(iSpec, 64.0) * scene.cLight * body.cSpec
             + pow(iSpec1, 64.0) * cLight1 * body.cSpec;

}


