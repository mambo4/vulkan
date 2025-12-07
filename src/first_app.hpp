#pragma once

#include "lbe_window.hpp"
#include "lbe_device.hpp"
#include "lbe_model.hpp"
#include "lbe_game_object.hpp"
#include "lbe_renderer.hpp"
//std
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
            void loadGameObjects();

            LbeWindow lbeWindow{WIDTH, HEIGHT, "lbeEngine -vulkan Window"};
            LbeDevice lbeDevice{lbeWindow};
            LbeRenderer lbeRenderer{lbeWindow, lbeDevice};

            std::vector<VkCommandBuffer> commandBuffers;
            std::vector<LbeGameObject> gameObjects;
     }; 

} 