#include "lbe_window.hpp"

namespace lbe {
    
    LbeWindow::LbeWindow(int w, int h, std::string title) : width{w}, height{h}, title{title} {

        initWindow();

    };

    LbeWindow::~LbeWindow() {

        glfwDestroyWindow(window);
        glfwTerminate();

    }

    
    void LbeWindow::initWindow() {

        glfwInit();

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, title.c_str(), nullptr, nullptr);

    }
} 