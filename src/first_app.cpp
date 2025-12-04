#include "first_app.hpp"
#include <stdexcept>
#include <array>

namespace lbe {

    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    FirstApp::~FirstApp() {
        vkDestroyPipelineLayout(lbeDevice.device(), pipelineLayout, nullptr);
    }

    //game loop
    void FirstApp::run() {
        while(!lbeWindow.shouldClose()) {
            glfwPollEvents();
            drawFrame();  
        }
        vkDeviceWaitIdle(lbeDevice.device());
    } 

    void FirstApp::loadModels(){
/*
        std::vector<LbeModel::Vertex> vertices{
            {{0.0f, 0.5f},{1.0,0.0,0.0}},
            {{-0.5f, 0.0f},{0.0,1.0,0.0}},
            {{-0.25f,- 0.5f},{0.0,0.0,1.0}},

            {{0.0f, 0.5f},{1.0,0.0,0.0}},
            {{-0.25f,- 0.5f},{0.0,0.0,1.0}},
            {{0.0f, -0.3f},{1.0,0.0,0.0}},
            
            {{0.0f, 0.5f},{1.0,0.0,0.0}},
            {{0.0f, -0.3f},{1.0,0.0,0.0}},
            {{0.25f,- 0.5f},{0.0,0.0,1.0}},

            {{0.0f, 0.5f},{1.0,0.0,0.0}},
            {{0.25f,- 0.5f},{0.0,0.0,1.0}},
            {{0.5f,0.0f},{0.0,1.0,0.0}},
        };
*/
        std::vector<LbeModel::Vertex> vertices{
            {{0.0f, -0.5f},{1.0,0.0,0.0}},
            {{0.5f, 0.5f},{0.0,1.0,0.0}},
            {{-0.5f, 0.5f},{0.0,0.0,1.0}},
        }; 
        lbeModel = std::make_unique<LbeModel>(lbeDevice, vertices);
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

    void FirstApp::createCommandBuffers(){

        commandBuffers.resize(lbeSwapChain.imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lbeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(lbeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("(vkAllocateCommandBuffers) failed to allocate command buffers!");
        }

        for (size_t i = 0; i < commandBuffers.size(); i++) {
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("(vkBeginCommandBufferfailed to begin recording command buffer!");
            }

            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = lbeSwapChain.getRenderPass();
            renderPassInfo.framebuffer = lbeSwapChain.getFrameBuffer(i);
            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = lbeSwapChain.getSwapChainExtent();

            std::array<VkClearValue, 2> clearValues{};
            clearValues[0].color = {{0.01f, 0.01f, 0.01f, 1.0f}};
            clearValues[1].depthStencil = {1.0f, 0};

            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

            lbePipeline->bind(commandBuffers[i]);
            lbeModel->bind(commandBuffers[i]);
            lbeModel->draw(commandBuffers[i]);

            vkCmdEndRenderPass(commandBuffers[i]);

            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("(vkEndCommandBuffer)failed to record command buffer!");
            }
        }

    }
    
    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = lbeSwapChain.acquireNextImage(&imageIndex);

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("(lbeSwapChain.acquireNextImagefailed to acquire next swap chain image!");
        }

        result = lbeSwapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("(lbeSwapChain.submitCommandBuffers) failed to present swap chain image!");
        }
    }
}