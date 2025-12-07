#pragma once

#include "lbe_pipeline.hpp"
#include "lbe_device.hpp"
#include "lbe_model.hpp"
#include "lbe_game_object.hpp"
#include "lbe_camera.hpp"

//std
#include <memory>
#include <vector>


namespace lbe {
    
     class SimpleRenderSystem{
        
        public: 
        
            SimpleRenderSystem(LbeDevice& device, VkRenderPass renderPass);
            ~SimpleRenderSystem();

            SimpleRenderSystem(const SimpleRenderSystem&) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

            void renderGameObjects(
                VkCommandBuffer commandBuffer,
                std::vector<LbeGameObject>& gameObjects,const LbeCamera& camera);

        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            LbeDevice &lbeDevice;

            std::unique_ptr<LbePipeline> lbePipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
     }; 

} 