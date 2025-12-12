#pragma once

#include "m4_camera.hpp"
#include "m4_game_object.hpp"

//lib
#include <vulkan/vulkan.h>

namespace m4{

    #define MAX_LIGHTS 10

    struct PointLight{
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo {
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 inverseView{1.0f};
        glm::vec4 ambientLightColor{0.2f, 0.0f, 0.0f, 0.02f}; //RGBI
        PointLight pointLights[MAX_LIGHTS];
        int numLights = 0;
    };
 
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        M4Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        M4GameObject::Map &gameObjects;
    };
}