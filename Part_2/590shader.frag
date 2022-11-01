


#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform SceneUniforms {
    vec4 uLight;
    vec4 cLight;
    vec4 pLight;
    vec4 cLight1;
    vec4 cAmb;
    mat4 camera;

} scene;
layout(binding = 1) uniform BodyUniforms {
    mat4 modeling;
    uvec4 texIndices;
} body;
layout(binding = 2) uniform sampler2D samplers[3];

layout(location = 0) in vec2 st;
layout(location = 1) in vec4 dNorm;
layout(location = 2) in vec4 dLight;



layout(location = 0) out vec4 outColor;

void main() {
    vec4 cDiff = texture(samplers[body.texIndices[0]], st);
    vec4 uNorm = normalize(dNorm);
    vec4 uLight1 = normalize(dLight);
    float iDiff = max(0.0, dot(uNorm, scene.uLight));
    float iDiff1 = max(0.0, dot(uNorm, uLight1));



    outColor = iDiff * scene.cLight * cDiff + iDiff1 * scene.cLight1 * cDiff + scene.cAmb * cDiff;

}


