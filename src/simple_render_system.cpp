#include "simple_render_system.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

namespace m4 {

    struct SimplePushConstantData {

        glm::mat4 transform{1.0f};
        glm::mat4 normalMatrix{1.0f};
    };  

    SimpleRenderSystem::SimpleRenderSystem(M4Device& device, VkRenderPass renderPass) : m4Device{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(m4Device.device(), pipelineLayout, nullptr);
    }

    //game loop

    void SimpleRenderSystem::createPipelineLayout(){

        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
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
    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass){
        assert(pipelineLayout != nullptr && "SimpleRenderSystem::createPipeline() Cannot create pipeline before pipeline layout");

        //auto pipelineConfig = M4Pipeline::defaultPipelineConfigInfo(m4SwapChain.width(), m4SwapChain.height());
        PipelineConfigInfo pipelineConfig{};
        M4Pipeline::defaultPipelineConfigInfo(
            pipelineConfig
        );

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        m4Pipeline = std::make_unique<M4Pipeline>(
            m4Device,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<M4GameObject>& gameObjects,
                const M4Camera& camera){
        m4Pipeline->bind(commandBuffer);

        auto projectionView=camera.getProjection() * camera.getView();

        for (auto& obj : gameObjects) {

            SimplePushConstantData push{};
            auto modelMatrix = obj.transform.mat4();
            push.transform = projectionView * modelMatrix;
            push.normalMatrix = obj.transform.normalMatrix();

            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push);

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
        }
    }

}