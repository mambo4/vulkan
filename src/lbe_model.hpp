#pragma once

#include "lbe_device.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace lbe {
    class LbeModel {

    public:

        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        LbeModel( LbeDevice &device, const std::vector<Vertex> &vertices);
        ~LbeModel();

        LbeModel(const LbeModel&) = delete;
        LbeModel& operator=(const LbeModel&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:

        void createVertexBuffers(const std::vector<Vertex>& vertices);

        LbeDevice& lbeDevice;
        VkBuffer vertexBuffer;
        VkDeviceMemory vertexBufferMemory;
        uint32_t vertexCount;

    };
}  // namespace lbe
