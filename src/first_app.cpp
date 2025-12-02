#include "first_app.hpp"
#include <stdexcept>

namespace lbe {

    FirstApp::FirstApp() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lbeDevice.device(), pipelineLayout, nullptr);
    }

    void FirstApp::run() {
        while(!lbeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }

    void FirstApp::createPipelineLayout(){
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        pipelineLayoutInfo.pushConstantRangeCount = 0;
        pipelineLayoutInfo.pPushConstantRanges = nullptr;

        if (vkCreatePipelineLayout(
            lbeDevice.device(),
            &pipelineLayoutInfo,
            nullptr,
            &pipelineLayout) != VK_SUCCESS) {
                throw std::runtime_error("failed to create pipeline layout!");
            }
    }
    void FirstApp::createPipeline(){
        //auto pipelineConfig = LbePipeline::defaultPipelineConfigInfo(lbeSwapChain.width(), lbeSwapChain.height());
        PipelineConfigInfo pipelineConfig{};
        LbePipeline::defaultPipelineConfigInfo(
            pipelineConfig,
            lbeSwapChain.width(),
            lbeSwapChain.height());

        pipelineConfig.renderPass = lbeSwapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lbePipeline = std::make_unique<LbePipeline>(
            lbeDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::createCommandBuffers(){}
    void FirstApp::drawFrame(){}

}