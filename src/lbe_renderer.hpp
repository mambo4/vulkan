#pragma once

#include "lbe_window.hpp"
#include "lbe_device.hpp"
#include "lbe_swap_chain.hpp"
#include "lbe_model.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>  

namespace lbe {
    
     class LbeRenderer{
        
        public: 
        
            LbeRenderer(LbeWindow& window, LbeDevice& device);
            ~LbeRenderer();

            LbeRenderer(const LbeRenderer&) = delete;
            LbeRenderer& operator=(const LbeRenderer&) = delete;

            VkRenderPass getSwapChainRenderPass() const {
                return lbeSwapChain->getRenderPass();
            }
            bool isFrameInProgress() const { return isFrameStarted; }
            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot getCurrentCommandBuffer() when frame is not in progress");
                return commandBuffers[currentFrameIndex];
            }
            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot getFrameIndex() when frame is not in progress");
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);
            


        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            LbeWindow& lbeWindow;
            LbeDevice& lbeDevice;
            std::unique_ptr<LbeSwapChain> lbeSwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted{false};
     }; 
}  