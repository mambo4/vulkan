#include "m4_model.hpp"

#include <cassert>
#include <cstring>

namespace m4 {

    M4Model::M4Model( M4Device &device, const M4Model::M4Mesh &m4Mesh) : m4Device{device} {
        createVertexBuffers(m4Mesh.vertices);
        createIndexBuffers(m4Mesh.indices);
    }


    M4Model::~M4Model() {
        vkDestroyBuffer(m4Device.device(), vertexBuffer, nullptr);
        vkFreeMemory(m4Device.device(), vertexBufferMemory, nullptr);
        if(hasIndexBuffer) {
            vkDestroyBuffer(m4Device.device(), indexBuffer, nullptr);
            vkFreeMemory(m4Device.device(), indexBufferMemory, nullptr);
        }   
    }

    void M4Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory);

        void* data;
        vkMapMemory(m4Device.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m4Device.device(), vertexBufferMemory);
    }

    void M4Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0 ;

        if(!hasIndexBuffer) {
            return;
        }   

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            indexBuffer,
            indexBufferMemory);

        void* data;
        vkMapMemory(m4Device.device(), indexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m4Device.device(), indexBufferMemory);
    }

    void M4Model::draw(VkCommandBuffer commandBuffer) {
        if(hasIndexBuffer) {
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
            return;
        }  else{
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
    }

    void M4Model::bind(VkCommandBuffer commandBuffer) {
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
        if(hasIndexBuffer) {
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }


    std::vector<VkVertexInputBindingDescription> M4Model::Vertex::getBindingDescriptions() {
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    } 
    
    std::vector<VkVertexInputAttributeDescription> M4Model::Vertex::getAttributeDescriptions() {
        std::vector<VkVertexInputAttributeDescription> attributeDescriptions(2);
        //vertex position attribute
        attributeDescriptions[0].binding = 0;
        attributeDescriptions[0].location = 0;
        attributeDescriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[0].offset = offsetof(Vertex, position);
        //color attribute
        attributeDescriptions[1].binding = 0;
        attributeDescriptions[1].location = 1;
        attributeDescriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributeDescriptions[1].offset = offsetof(Vertex, color);

        return attributeDescriptions;
    } 
}