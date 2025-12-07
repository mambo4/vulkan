#include "simple_render_system.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace lbe {

    struct SimplePushConstantData {

        glm::mat4 transform{1.0f};
        alignas(16) glm::vec3 color;
    };  

    SimpleRenderSystem::SimpleRenderSystem(LbeDevice& device, VkRenderPass renderPass) : lbeDevice{device} {
        createPipelineLayout();
        createPipeline(renderPass);
    }

    SimpleRenderSystem::~SimpleRenderSystem() {
        vkDestroyPipelineLayout(lbeDevice.device(), pipelineLayout, nullptr);
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
            lbeDevice.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
    }
    void SimpleRenderSystem::createPipeline(VkRenderPass renderPass){
        assert(pipelineLayout != nullptr && "SimpleRenderSystem::createPipeline() Cannot create pipeline before pipeline layout");

        //auto pipelineConfig = LbePipeline::defaultPipelineConfigInfo(lbeSwapChain.width(), lbeSwapChain.height());
        PipelineConfigInfo pipelineConfig{};
        LbePipeline::defaultPipelineConfigInfo(
            pipelineConfig
        );

        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        lbePipeline = std::make_unique<LbePipeline>(
            lbeDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void SimpleRenderSystem::renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<LbeGameObject>& gameObjects,
                const LbeCamera& camera){
        lbePipeline->bind(commandBuffer);

        for (auto& obj : gameObjects) {
            obj.transform.rotation.y =glm::mod(obj.transform.rotation.y + 0.01f, glm::two_pi<float>());
            obj.transform.rotation.x =glm::mod(obj.transform.rotation.x + 0.005f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.color = obj.color;
            push.transform = camera.getProjection() * obj.transform.mat4(); //temp : mat* is ultimately for GPU

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