# version 450

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 normal;
layout(location = 3) in vec2 uv;

layout(location = 0) out vec3 fragColor;

layout(set = 0, binding = 0) uniform GlobalUbo {
    mat4 projectionViewMatrix;
    vec4 ambientLightColor;
    vec3 lightPosition;
    vec4 lightColor;
    
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 modelMatrix; 
    mat4 normalMatrix;
} push;


const vec3 AMBIENT_COLOR = vec3(0.01, 0.0, 0.0);

void main(){
    vec4 positionWorldSpace = push.modelMatrix * vec4(position, 1.0);
    gl_Position = ubo.projectionViewMatrix * push.modelMatrix * vec4(position, 1.0);

    vec3 normalWorldSpace = normalize(mat3(push.normalMatrix) * normal);
    vec3 directionToLight = ubo.lightPosition - positionWorldSpace.xyz;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);

    vec3 lightColor=ubo.lightColor.rgb*ubo.lightColor.a *attenuation;
    vec3 ambientLightColor=ubo.ambientLightColor.rgb*ubo.ambientLightColor.a;
    vec3 diffuseLightColor=lightColor*max(dot(normalWorldSpace, normalize(directionToLight)), 0.0);

    fragColor = (diffuseLightColor+ambientLightColor)*color;
}