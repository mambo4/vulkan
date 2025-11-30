#pragma once

#include "lbe_window.hpp"
#include "lbe_pipeline.hpp"

namespace lbe {
    
     class FirstApp{
        
        public: 
            static constexpr int WIDTH = 1280;
            static constexpr int HEIGHT = 720;
 
        void run();

        private:
            LbeWindow lbeWindow{WIDTH, HEIGHT, "lbeEngine -vulkan Window"};
            LbePipeline lbePipeline{"../shaders/simple_shader.vert.spv", "../shaders/simple_shader.frag.spv"};
     };

}