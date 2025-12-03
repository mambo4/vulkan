#pragma once

#include "lbe_window.hpp"
#include "lbe_pipeline.hpp"
#include "lbe_device.hpp"
#include "lbe_swap_chain.hpp"
#include "lbe_model.hpp"

#include <memory>
#include <vector>


namespace lbe {
    
     class FirstApp{
        
        public: 
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;
        
            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp&) = delete;
            FirstApp& operator=(const FirstApp&) = delete;

        
        void run();

        private:
            void loadModels();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            LbeWindow lbeWindow{WIDTH, HEIGHT, "lbeEngine -vulkan Window"};
            LbeDevice lbeDevice{lbeWindow};
            LbeSwapChain lbeSwapChain{lbeDevice, lbeWindow.getExtent()};
            std::unique_ptr<LbePipeline> lbePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<LbeModel> lbeModel;
     }; 

} 