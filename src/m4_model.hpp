#pragma once

#include "m4_device.hpp"


#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

#include <vector>

namespace m4 {
    class M4Model {

    public:

        struct Vertex {
            glm::vec3 position;
            glm::vec3 color;
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
        };

        struct M4Mesh {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
        };  

        M4Model( M4Device &device, const M4Model::M4Mesh &builder);
        ~M4Model();

        M4Model(const M4Model&) = delete;
        M4Model& operator=(const M4Model&) = delete;

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:

        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        M4Device& m4Device;
        VkBuffer vertexBuffer;
        VkBuffer indexBuffer;
        VkDeviceMemory vertexBufferMemory;
        VkDeviceMemory indexBufferMemory;
        uint32_t vertexCount;
        uint32_t indexCount;
        bool hasIndexBuffer = false;

    };
}  // namespace m4
