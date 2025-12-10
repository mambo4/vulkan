#include "m4_model.hpp"
#include "m4_utils.hpp"

//lib
#define TINYOBJLOADER_IMPLEMENTATION
#include <tinyobjloader.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/hash.hpp>

//std
#include <cassert>
#include <cstring>
#include <unordered_map>
#include <iostream>

namespace std {

    /*
    // this buggy hash is from  https://www.youtube.com/watch?v=jdiPVfIHmEA&list=PL8327DO66nu9qYVKLDmdLW_84-yE4auCR&index=20
    // it uses m4::hashCombine which is a fold expression available in c++17
    // but the code below does not expand the parameter pack 'rest'
    // and causes compile error :"error c3520: 'rest': parameter pack must be expanded in this context"
    // I am too ignorant to fix it right now so I am using the other hash function below
    template <>
    struct hash<m4::M4Model::Vertex> {
        size_t operator()(m4::M4Model::Vertex const &vertex) const {
            size_t seed = 0;
            m4::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };
    */

    // this hash is from  https://vulkan-tutorial.com/Loading_models modified to check vertex normals
    template<> struct hash<m4::M4Model::Vertex> {
        size_t operator()(m4::M4Model::Vertex const& vertex) const {
            return ( 
                (((hash<glm::vec3>()(vertex.position) ^ (hash<glm::vec3>()(vertex.color) << 1))
                  >> 1) ^ (hash<glm::vec2>()(vertex.uv) << 1))>> 1) ^ (hash<glm::vec3>()(vertex.normal) << 1
            );
        }
    };

}  // namespace std


namespace m4 {
    //was: Builder
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

    std::unique_ptr<M4Model> M4Model::createModelFromFile(M4Device &device, const std::string &filepath) { 
        M4Mesh m4Mesh{};
        m4Mesh.loadModel(filepath);
        std::cout << "Vertex count: " << m4Mesh.vertices.size() << "\n";
        return std::make_unique<M4Model>(device, m4Mesh);

    }

    void M4Model::createVertexBuffers(const std::vector<Vertex>& vertices) {
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");
        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m4Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m4Device.device(), stagingBufferMemory);
        
        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory);

        m4Device.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);
        vkDestroyBuffer(m4Device.device(), stagingBuffer, nullptr);
        vkFreeMemory(m4Device.device(), stagingBufferMemory, nullptr);
    }

    void M4Model::createIndexBuffers(const std::vector<uint32_t>& indices) {
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0 ;

        if(!hasIndexBuffer) {
            return;
        }   

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory);

        void* data;
        vkMapMemory(m4Device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(m4Device.device(), stagingBufferMemory);
        
        m4Device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory);

        m4Device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);
        vkDestroyBuffer(m4Device.device(), stagingBuffer, nullptr);
        vkFreeMemory(m4Device.device(), stagingBufferMemory, nullptr);
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

    //M4Mesh was: Builder !!! all tutorial refs to "Builder" use "M4Mesh" instead
    void M4Model::M4Mesh::loadModel(const std::string &filepath) {

        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filepath.c_str())) {
            throw std::runtime_error(warn + err);
        }

        vertices.clear();
        indices.clear();

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};

        int stride=0;
        for (const auto& shape : shapes) {
            for (const auto& index : shape.mesh.indices) {
                Vertex vertex{};

                if (index.vertex_index >= 0) {
          
                    stride=3;
                    vertex.position = {
                        
                        attrib.vertices[stride * index.vertex_index + 0],
                        attrib.vertices[stride * index.vertex_index + 1],
                        attrib.vertices[stride * index.vertex_index + 2]
                    };
                    
                    //optional OBJ vert color extension
                    auto colorIndex = 3 * index.vertex_index +2; //looks suspicious
                    if(colorIndex < attrib.colors.size()){
                        vertex.color = {
                            attrib.colors[colorIndex - 2],
                            attrib.colors[colorIndex - 1],
                            attrib.colors[colorIndex - 0]
                        };
                    }else{
                        vertex.color = {1.0f, 1.0f, 1.0f};
                    }
                }

                if (index.normal_index >= 0) {
                    stride=3;
                    vertex.normal = {
                        attrib.normals[stride * index.normal_index + 0],
                        attrib.normals[stride * index.normal_index + 1],
                        attrib.normals[stride * index.normal_index + 2]
                    };
                }
                
                if (index.texcoord_index >= 0) {
                    stride=2;
                    vertex.uv = {
                        attrib.texcoords[stride * index.texcoord_index + 0],
                        1.0f - attrib.texcoords[stride * index.texcoord_index + 1] //vulkan inverts y
                    };
                }

                //vertices.push_back(vertex); /*Vertex count: 30888*/
                if (uniqueVertices.count(vertex) == 0) {
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]); /*Vertex count: 5545*/
            }
        }
    }
}