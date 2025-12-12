#include "point_light_system.hpp"
#include <stdexcept>
#include <array>
#include <map>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

namespace m4 {

    struct PointLightPushConstants {
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem(M4Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m4Device{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(m4Device.device(), pipelineLayout, nullptr);
    }

    //game loop

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout){

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstants);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 1;
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

        if (vkCreatePipelineLayout(
            m4Device.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass){
        assert(pipelineLayout != nullptr && "PointLightSystem::createPipeline() Cannot create pipeline before pipeline layout");

        //auto pipelineConfig = M4Pipeline::defaultPipelineConfigInfo(m4SwapChain.width(), m4SwapChain.height());
        PipelineConfigInfo pipelineConfig{};
        M4Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        M4Pipeline::enableAlphaBlending(pipelineConfig);
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout; 
        m4Pipeline = std::make_unique<M4Pipeline>(
            m4Device,
            "../shaders/point_light.vert.spv",
            "../shaders/point_light.frag.spv",
            pipelineConfig);
    }

    void PointLightSystem::update(FrameInfo &frameInfo, GlobalUbo &ubo){

        //anim rotation of lights
        auto rotateLight= glm::rotate( glm::mat4(1.0f), frameInfo.frameTime,{0.0f, -1.0f, 0.0f});



        int lightIndex = 0;
        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            assert(lightIndex < MAX_LIGHTS && "PointLightSystem::update() MAX_LIGHTS exceeded, add more lights to the shader");

            // rotate light around the scene 
            // todo: try varying intensity etc
            glm::vec3 rotatedPosXYZ = rotateLight * glm::vec4(obj.transform.translation, 1.0f);
            obj.transform.translation = rotatedPosXYZ;

            //copy light data to ubo
            ubo.pointLights[lightIndex].position = glm::vec4{obj.transform.translation, 1.0f};
            ubo.pointLights[lightIndex].color = glm::vec4{obj.color, obj.pointLight->lightIntensity};

            lightIndex += 1;
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo &frameInfo){

        //z sorting of lights for proper blending could be done here
        std::map<float, M4GameObject::id_t> sortedLights;
        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            // ditance form camera
            float distance = glm::length(frameInfo.camera.getPosition() - obj.transform.translation);
            float distSqr =  glm::dot(distance,distance);
            sortedLights[distSqr] = obj.getId();
        }

        m4Pipeline->bind(frameInfo.commandBuffer);

        auto projectionView=frameInfo.camera.getProjection() * frameInfo.camera.getView();

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        //draw in reverse sorted order (furthest first)
        for(auto it = sortedLights.rbegin(); it != sortedLights.rend(); ++it) {
            // use game obj id to find pointlight obj
            auto& obj = frameInfo.gameObjects.at(it->second);

            PointLightPushConstants push{};
            push.position = glm::vec4{obj.transform.translation, 1.0f};
            push.color = glm::vec4{obj.color, obj.pointLight->lightIntensity};
            push.radius = obj.transform.scale.x;
            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstants),
                &push);

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }
}