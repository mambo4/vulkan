#pragma once

#include "lbe_window.hpp"

namespace lbe {
    
     class FirstApp{
        
        public: 
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;
 
        void run();

        private:
            LbeWindow lbeWindow{WIDTH, HEIGHT, "lbeEngine -vulkan Window"};

     };

}