#pragma once

#define GLFW_INCLUDE_VULKAN // tells '#include <GLFW/glfw3.h>' to include the Vulkan headers
#include <GLFW/glfw3.h>
#include <string>

namespace m4 {

class M4Window {

    public:

      M4Window(int w, int h, std::string title);
      ~M4Window();
    
      M4Window(const M4Window&) = delete;
      M4Window& operator=(const M4Window&) = delete;
    

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

}// namespace m4