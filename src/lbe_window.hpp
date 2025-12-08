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

      VkExtent2D getExtent() const {return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};} 
      bool wasWindowResized() const { return framebufferResized; }
      void resetWindowResizedFlag() {framebufferResized = false; }
      GLFWwindow* getGLFWwindow() const { return window; }
      void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
      
    private:
      static void framebufferResizeCallback(GLFWwindow* window, int width, int height);
      void initWindow();
      int width;
      int height;
      bool framebufferResized = false;

      std::string title;
      GLFWwindow* window;
};

}// namespace lbe