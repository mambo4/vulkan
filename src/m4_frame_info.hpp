#pragma once

#include "m4_camera.hpp"
#include "m4_game_object.hpp"

//lib
#include <vulkan/vulkan.h>

namespace m4{
    struct FrameInfo{
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        M4Camera &camera;
        VkDescriptorSet globalDescriptorSet;
        M4GameObject::Map &gameObjects;
    };
}