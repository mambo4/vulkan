#pragma once

#define GLFW_INCLUDE_VULKAN // tells '#include <GLFW/glfw3.h>' to include the Vulkan headers
#include <GLFW/glfw3.h>
#include <string>

namespace lbe {

class LbeWindow {

    public:

      LbeWindow(int w, int h, std::string title);
      ~LbeWindow();
    
      LbeWindow(const LbeWindow&) = delete;
      LbeWindow& operator=(const LbeWindow&) = delete;
    

      bool shouldClose() const {
          return glfwWindowShouldClose(window);
      }

      VkExtent2D getExtent() const {
          return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
      } 

      void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
      
    private:
      void initWindow();
      int width;
      int height;

      std::string title;
      GLFWwindow* window;
};

}// namespace lbe