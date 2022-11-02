
/*
Thomas Zeng & Kyosuke
*/

#version 450
#extension GL_ARB_separate_shader_objects : enable

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

layout(location = 0) in vec3 attrXYZ;
layout(location = 1) in vec2 attrST;
layout(location = 2) in vec3 attrNOP;

layout(location = 0) out vec2 st;
layout(location = 1) out vec4 uNorm;
layout(location = 2) out vec4 uLight1;
layout(location = 3) out vec4 uCam;


void main() {
    uNorm = body.modeling * vec4(attrNOP, 0.0);
    vec4 xyzwWorld = body.modeling * vec4(attrXYZ, 1.0);

    uLight1 = normalize(scene.pLight - xyzwWorld);
    uCam = normalize(scene.pCam - xyzwWorld);
    gl_Position = scene.camera * xyzwWorld;
    st = attrST;
}


