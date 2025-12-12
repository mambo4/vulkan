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
    
     class PointLightSystem{
        
        public: 
        
            PointLightSystem(M4Device& device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~PointLightSystem();

            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator=(const PointLightSystem&) = delete;

            void update(FrameInfo &frameInfo, GlobalUbo &ubo);
            void render(FrameInfo &frameInfo);

        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            M4Device &m4Device;

            std::unique_ptr<M4Pipeline> m4Pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
     }; 

} 