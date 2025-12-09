#pragma once

#include "m4_pipeline.hpp"
#include "m4_device.hpp"
#include "m4_model.hpp"
#include "m4_game_object.hpp"
#include "m4_camera.hpp"

//std
#include <memory>
#include <vector>


namespace m4 {
    
     class SimpleRenderSystem{
        
        public: 
        
            SimpleRenderSystem(M4Device& device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem&) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

            void renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<M4GameObject>& gameObjects,const M4Camera& camera);

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            M4Device &m4Device;

            std::unique_ptr<M4Pipeline> m4Pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
     }; 

} 