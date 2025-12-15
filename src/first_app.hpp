#pragma once

#include "m4_window.hpp"
#include "m4_device.hpp"
#include "m4_model.hpp"
#include "m4_game_object.hpp"
#include "m4_renderer.hpp"
#include "m4_descriptors.hpp"

//std
#include <memory>
#include <vector>


namespace m4 {
    
    class FirstApp{
        
        public: 
            static constexpr int WIDTH = 1280;
            static constexpr int HEIGHT = 720;
        
            FirstApp();
            ~FirstApp();

            FirstApp(const FirstApp&) = delete;
            FirstApp& operator=(const FirstApp&) = delete;

            static constexpr float MAX_FRAME_TIME = 0.016f;
        void run();

        private:
            void loadGameObjects();

            M4Window m4Window{WIDTH, HEIGHT, "m4Engine -vulkan Window"};
            M4Device m4Device{m4Window};
            M4Renderer m4Renderer{m4Window, m4Device};

            // std::vector<VkCommandBuffer> commandBuffers;
            // order of declaration matters
            std::unique_ptr<M4DescriptorPool> globalPool{};
            M4GameObject::Map gameObjects;
    }; 
} 