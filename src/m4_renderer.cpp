#include "m4_renderer.hpp"

#include <stdexcept>
#include <array>


namespace m4 {

    M4Renderer::M4Renderer(M4Window& window, M4Device& device) : m4Window{window}, m4Device{device} {
        recreateSwapChain();
        createCommandBuffers();
    }

    M4Renderer::~M4Renderer() {
        freeCommandBuffers();
    }

    //game loop

    void M4Renderer::recreateSwapChain() {
        auto extent = m4Window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = m4Window.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(m4Device.device());

        if (m4SwapChain==nullptr) {
            m4SwapChain = std::make_unique<M4SwapChain>(m4Device, extent);
        } else {
            std::shared_ptr<M4SwapChain> oldSwapChain = std::move(m4SwapChain);
            m4SwapChain = std::make_unique<M4SwapChain>(m4Device, extent, oldSwapChain);

            if(!oldSwapChain->compareSwapFormats(*m4SwapChain.get())) {
                throw std::runtime_error("Swap chain image or depth format has changed!");
            }

        }

    }

    void M4Renderer::createCommandBuffers(){

        commandBuffers.resize(M4SwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m4Device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(m4Device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("(vkAllocateCommandBuffers) failed to allocate command buffers!");
        }
    }
    
    void M4Renderer::freeCommandBuffers(){
        vkFreeCommandBuffers(
            m4Device.device(),
            m4Device.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }    

    VkCommandBuffer M4Renderer::beginFrame(){
        assert(!isFrameStarted && "Cannot call beginFrame() when frame is already in progress");

        auto result = m4SwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("(m4SwapChain->acquireNextImagefailed to acquire next swap chain image!");
        }

        isFrameStarted = true;

        auto commandBuffer = getCurrentCommandBuffer();

        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffer, &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("(vkBeginCommandBufferfailed to begin recording command buffer!");
        }

        return commandBuffer;
    }

    void M4Renderer::endFrame() {
        assert(isFrameStarted && "Cannot call endFrame() when frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("(vkEndCommandBuffer)failed to record command buffer!");
        }
        auto result = m4SwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || m4Window.wasWindowResized()) {
            m4Window.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("(m4SwapChain.submitCommandBuffers) failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1)% M4SwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void M4Renderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Cannot call beginSwapChainRenderPass() when frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = m4SwapChain->getRenderPass();
        renderPassInfo.framebuffer = m4SwapChain->getFrameBuffer(currentImageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = m4SwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.001f, 0.001f, 0.001f, 1.0f}};  
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        VkViewport viewport{};
        viewport.x = 0.0f;  
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(m4SwapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(m4SwapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{};
        scissor.offset = {0, 0};
        scissor.extent = m4SwapChain->getSwapChainExtent();
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    }

    void M4Renderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Cannot call endSwapChainRenderPass() when frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);

    }


}