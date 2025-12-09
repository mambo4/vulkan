#include "m4_window.hpp"
#include <stdexcept>

namespace m4 {
    
    M4Window::M4Window(int w, int h, std::string title) : width{w}, height{h}, title{title} {

        initWindow();

    };

    M4Window::~M4Window() {

        glfwDestroyWindow(window);
        glfwTerminate();

    }

    void M4Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {

        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("failed to create window surface!");
        }

    }
    
    void M4Window::initWindow() {

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);
    }

    void M4Window::framebufferResizeCallback(GLFWwindow* window, int width, int height) {
        auto m4Window = reinterpret_cast<M4Window*>(glfwGetWindowUserPointer(window));
        m4Window->framebufferResized = true;
        m4Window->width = width;
        m4Window->height = height; 
        m4Window->framebufferResized = true;
    }
} 