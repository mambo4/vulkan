#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix; 
    mat4 normalMatrix;
} push;

struct PointLight {
    vec4 position;
    vec4 color;
};

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    PointLight pointLights[10]; //must update with m4_frame_info.hpp MAX_LIGHTS
    int numLights;
} ubo;

void main() {
  
  vec3 diffuseLight=ubo.ambientLightColor.rgb*ubo.ambientLightColor.a;
  vec3 surfaceNormal=normalize(fragNormalWorld);

  for (int i=0;i<ubo.numLights;i++){
    PointLight light=ubo.pointLights[i];
    vec3 directionToLight=light.position.xyz-fragPosWorld;
    float attenuation=1.0/dot(directionToLight,directionToLight);//distance squared
    float cosAngleincidence=max(dot(surfaceNormal,normalize(directionToLight)),0.0);
    vec3 intensity=light.color.rgb*light.color.a*attenuation;
    diffuseLight+=intensity*cosAngleincidence;
  }
  outColor = vec4(diffuseLight*fragColor, 1.0);
}