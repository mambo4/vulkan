#include "point_light_system.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

namespace m4 {

    PointLightSystem::PointLightSystem(M4Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout) : m4Device{device} {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(m4Device.device(), pipelineLayout, nullptr);
    }

    //game loop

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout){

        // VkPushConstantRange pushConstantRange{};
        // pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        // pushConstantRange.offset = 0;
        // pushConstantRange.size = sizeof(SimplePushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size());
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data();
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

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

    void PointLightSystem::render(FrameInfo &frameInfo){

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

        vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);

    }

}