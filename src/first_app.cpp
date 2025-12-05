#include "first_app.hpp"
#include <stdexcept>
#include <array>

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>

namespace lbe {

    struct SimplePushConstantData {
        glm::vec2 offset;
        alignas(16) glm::vec3 color;
    };  

    FirstApp::FirstApp() {
        loadModels();
        createPipelineLayout();
        recreateSwapChain();
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
    void FirstApp::createPipeline(){
        assert(lbeSwapChain != nullptr && "FirstApp::createPipeline() Cannot create pipeline before swap chain");
        assert(pipelineLayout != nullptr && "FirstApp::createPipeline() Cannot create pipeline before pipeline layout");

        //auto pipelineConfig = LbePipeline::defaultPipelineConfigInfo(lbeSwapChain.width(), lbeSwapChain.height());
        PipelineConfigInfo pipelineConfig{};
        LbePipeline::defaultPipelineConfigInfo(
            pipelineConfig
        );

        pipelineConfig.renderPass = lbeSwapChain->getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        lbePipeline = std::make_unique<LbePipeline>(
            lbeDevice,
            "../shaders/simple_shader.vert.spv",
            "../shaders/simple_shader.frag.spv",
            pipelineConfig);
    }

    void FirstApp::recreateSwapChain() {
        auto extent = lbeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lbeWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lbeDevice.device());

        if (lbeSwapChain==nullptr) {
            lbeSwapChain = std::make_unique<LbeSwapChain>(lbeDevice, extent);
        } else {
            lbeSwapChain = std::make_unique<LbeSwapChain>(lbeDevice, extent, std::move(lbeSwapChain));
            if(!lbeSwapChain->imageCount() != commandBuffers.size())    {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        // if render pass is compatible, don't need to re-create pipeline
        createPipeline();
        // createCommandBuffers();
    }

    void FirstApp::createCommandBuffers(){

        commandBuffers.resize(lbeSwapChain->imageCount());
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lbeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(lbeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("(vkAllocateCommandBuffers) failed to allocate command buffers!");
        }
    }
    
    void FirstApp::freeCommandBuffers(){
        vkFreeCommandBuffers(
            lbeDevice.device(),
            lbeDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }    

    void FirstApp::recordCommandBuffer(int imageIndex){

        static int frame=0;
        frame =(frame+1)%1000;
        
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("(vkBeginCommandBufferfailed to begin recording command buffer!");
        }

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lbeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lbeSwapChain->getFrameBuffer(imageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lbeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.001f, 0.001f, 0.001f, 1.0f}};  
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;  
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(lbeSwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(lbeSwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;

        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = lbeSwapChain->getSwapChainExtent();

        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        lbePipeline->bind(commandBuffers[imageIndex]);
        lbeModel->bind(commandBuffers[imageIndex]);
        
        // animate model via push constants
        for (int j = 0; j < 4; j++)
        {
            SimplePushConstantData push{};
            push.offset = {-0.5f + frame * 0.001f, -0.4f + j * 0.25f};
            push.color = { 0.0f, 0.0f, 0.2f+ 0.2f * j };

            vkCmdPushConstants(
                commandBuffers[imageIndex],
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push); 
                
            lbeModel->draw(commandBuffers[imageIndex]);
        }

        vkCmdEndRenderPass(commandBuffers[imageIndex]);

        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("(vkEndCommandBuffer)failed to record command buffer!");
        }
    }

    void FirstApp::drawFrame(){
        uint32_t imageIndex;
        auto result = lbeSwapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("(lbeSwapChain->acquireNextImagefailed to acquire next swap chain image!");
        }

        recordCommandBuffer(imageIndex);

        result = lbeSwapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lbeWindow.wasWindowResized()) {
            lbeWindow.resetWindowResizedFlag();
            recreateSwapChain();
            return;
        }   

        if (result != VK_SUCCESS) {
            throw std::runtime_error("(lbeSwapChain.submitCommandBuffers) failed to present swap chain image!");
        }
    }
}