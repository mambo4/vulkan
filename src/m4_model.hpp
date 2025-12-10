#pragma once

#include "m4_device.hpp"
#include "m4_buffer.hpp"


//LIB
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>

//STD
#include <vector>
#include <memory>

namespace m4 {
    class M4Model {

    public:

        struct Vertex {
            glm::vec3 position{};
            glm::vec3 color{};
            glm::vec3 normal{};
            glm::vec2 uv{};
            static std::vector<VkVertexInputBindingDescription> getBindingDescriptions();
            static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

            bool operator==(const Vertex& other) const {
                return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
            }
        };

        //Builder: all tutorial refs to "Builder" use "M4Mesh" instead
        struct M4Mesh {
            std::vector<Vertex> vertices{};
            std::vector<uint32_t> indices{};
            void loadModel(const std::string &filepath);
        };  

        M4Model( M4Device &device, const M4Model::M4Mesh &builder);
        ~M4Model();

        M4Model(const M4Model&) = delete;
        M4Model& operator=(const M4Model&) = delete;

        std::unique_ptr<M4Model> static createModelFromFile(M4Device &device, const std::string &filepath);  

        void bind(VkCommandBuffer commandBuffer);
        void draw(VkCommandBuffer commandBuffer);

    private:

        void createVertexBuffers(const std::vector<Vertex>& vertices);
        void createIndexBuffers(const std::vector<uint32_t>& indices);

        M4Device& m4Device;

        std::unique_ptr<M4Buffer> vertexBuffer;
        uint32_t vertexCount;

        bool hasIndexBuffer = false;
        std::unique_ptr<M4Buffer> indexBuffer;
        uint32_t indexCount;


    };
}  // namespace m4
