#include "lbe_renderer.hpp"

#include <stdexcept>
#include <array>


namespace lbe {

    LbeRenderer::LbeRenderer(LbeWindow& window, LbeDevice& device) : lbeWindow{window}, lbeDevice{device} {
        recreateSwapChain();
        createCommandBuffers();
    }

    LbeRenderer::~LbeRenderer() {
        freeCommandBuffers();
    }

    //game loop

    void LbeRenderer::recreateSwapChain() {
        auto extent = lbeWindow.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = lbeWindow.getExtent();
            glfwWaitEvents();
        }

        vkDeviceWaitIdle(lbeDevice.device());

        if (lbeSwapChain==nullptr) {
            lbeSwapChain = std::make_unique<LbeSwapChain>(lbeDevice, extent);
        } else {
            std::shared_ptr<LbeSwapChain> oldSwapChain = std::move(lbeSwapChain);
            lbeSwapChain = std::make_unique<LbeSwapChain>(lbeDevice, extent, oldSwapChain);

            if(!oldSwapChain->compareSwapFormats(*lbeSwapChain.get())) {
                throw std::runtime_error("Swap chain image or depth format has changed!");
            }

        }

    }

    void LbeRenderer::createCommandBuffers(){

        commandBuffers.resize(LbeSwapChain::MAX_FRAMES_IN_FLIGHT);
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = lbeDevice.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(lbeDevice.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("(vkAllocateCommandBuffers) failed to allocate command buffers!");
        }
    }
    
    void LbeRenderer::freeCommandBuffers(){
        vkFreeCommandBuffers(
            lbeDevice.device(),
            lbeDevice.getCommandPool(),
            static_cast<uint32_t>(commandBuffers.size()),
            commandBuffers.data());
        commandBuffers.clear();
    }    

    VkCommandBuffer LbeRenderer::beginFrame(){
        assert(!isFrameStarted && "Cannot call beginFrame() when frame is already in progress");

        auto result = lbeSwapChain->acquireNextImage(&currentImageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR) {
            recreateSwapChain();
            return nullptr;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("(lbeSwapChain->acquireNextImagefailed to acquire next swap chain image!");
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

    void LbeRenderer::endFrame() {
        assert(isFrameStarted && "Cannot call endFrame() when frame is not in progress");
        auto commandBuffer = getCurrentCommandBuffer();
        if (vkEndCommandBuffer(commandBuffer) != VK_SUCCESS) {
            throw std::runtime_error("(vkEndCommandBuffer)failed to record command buffer!");
        }
        auto result = lbeSwapChain->submitCommandBuffers(&commandBuffer, &currentImageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || lbeWindow.wasWindowResized()) {
            lbeWindow.resetWindowResizedFlag();
            recreateSwapChain();
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("(lbeSwapChain.submitCommandBuffers) failed to present swap chain image!");
        }
        isFrameStarted = false;
        currentFrameIndex = (currentFrameIndex + 1)% LbeSwapChain::MAX_FRAMES_IN_FLIGHT;
    }

    void LbeRenderer::beginSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Cannot call beginSwapChainRenderPass() when frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot begin render pass on command buffer from a different frame");

        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = lbeSwapChain->getRenderPass();
        renderPassInfo.framebuffer = lbeSwapChain->getFrameBuffer(currentImageIndex);
        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = lbeSwapChain->getSwapChainExtent();

        std::array<VkClearValue, 2> clearValues{};
        clearValues[0].color = {{0.001f, 0.001f, 0.001f, 1.0f}};  
        clearValues[1].depthStencil = {1.0f, 0};

        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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
        vkCmdSetViewport(commandBuffer, 0, 1, &viewport);
        vkCmdSetScissor(commandBuffer, 0, 1, &scissor);

    }

    void LbeRenderer::endSwapChainRenderPass(VkCommandBuffer commandBuffer){
        assert(isFrameStarted && "Cannot call endSwapChainRenderPass() when frame is not in progress");
        assert(commandBuffer == getCurrentCommandBuffer() && "Cannot end render pass on command buffer from a different frame");

        vkCmdEndRenderPass(commandBuffer);

    }


}