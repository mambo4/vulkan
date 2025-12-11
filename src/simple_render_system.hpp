#pragma once

#include "m4_pipeline.hpp"
#include "m4_device.hpp"
#include "m4_model.hpp"
#include "m4_game_object.hpp"
#include "m4_camera.hpp"
#include "m4_frame_info.hpp"

//std
#include <memory>
#include <vector>


namespace m4 {
    
     class SimpleRenderSystem{
        
        public: 
        
            SimpleRenderSystem(M4Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem&) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

            void renderGameObjects(FrameInfo &frameInfo, std::vector<M4GameObject> &gameObjects);

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            M4Device &m4Device;

            std::unique_ptr<M4Pipeline> m4Pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
     }; 

} 