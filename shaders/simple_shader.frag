#version 450

layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

layout (location = 0) out vec4 outColor;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix; 
    mat4 normalMatrix;
} push;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projection;
    mat4 view;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor; 
} ubo;


void main() {
  
  vec3 directionToLight = ubo.lightPosition - fragPosWorld;
  float attenuation = 1.0 / dot(directionToLight, directionToLight);

  vec3 lightColor=ubo.lightColor.rgb*ubo.lightColor.a *attenuation;
  vec3 ambientLightColor=ubo.ambientLightColor.rgb*ubo.ambientLightColor.a;
  vec3 diffuseLightColor=lightColor*max(dot(normalize(fragNormalWorld), normalize(directionToLight)), 0.0);

  outColor = vec4((diffuseLightColor+ambientLightColor)*fragColor, 1.0);
}