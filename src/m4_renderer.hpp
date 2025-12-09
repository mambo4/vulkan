#pragma once

#include "m4_window.hpp"
#include "m4_device.hpp"
#include "m4_swap_chain.hpp"
#include "m4_model.hpp"

//std
#include <memory>
#include <vector>
#include <cassert>  

namespace m4 {
    
     class M4Renderer{
        
        public: 
        
            M4Renderer(M4Window& window, M4Device& device);
            ~M4Renderer();

            M4Renderer(const M4Renderer&) = delete;
            M4Renderer& operator=(const M4Renderer&) = delete;

            VkRenderPass getSwapChainRenderPass() const {
                return m4SwapChain->getRenderPass();
            }
            float getAspectRatio() const {return m4SwapChain->extentAspectRatio();}   

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

            M4Window& m4Window;
            M4Device& m4Device;
            std::unique_ptr<M4SwapChain> m4SwapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            uint32_t currentImageIndex;
            int currentFrameIndex{0};
            bool isFrameStarted{false};
     }; 
}  