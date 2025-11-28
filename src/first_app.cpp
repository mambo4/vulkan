#include "first_app.hpp"

namespace lbe {

    void FirstApp::run() {
        while(!lbeWindow.shouldClose()) {
            glfwPollEvents();
        }
    }

}