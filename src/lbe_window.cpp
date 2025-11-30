#include "lbe_window.hpp"
#include <stdexcept>

namespace lbe {
    
    LbeWindow::LbeWindow(int w, int h, std::string title) : width{w}, height{h}, title{title} {

        initWindow();

    };

    LbeWindow::~LbeWindow() {

        glfwDestroyWindow(window);
        glfwTerminate();

    }

    void LbeWindow::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {

        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

    }
    
    void LbeWindow::initWindow() {

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    }
} 