#pragma once

#include "m4_game_object.hpp"
#include "m4_window.hpp"

namespace m4 {

    class KeyboardMovementController {
    public:
        struct KeyMappings {
            int moveForward{GLFW_KEY_W};
            int moveBackward{GLFW_KEY_S};
            int moveLeft{GLFW_KEY_A};
            int moveRight{GLFW_KEY_D};
            int moveUp{GLFW_KEY_E};
            int moveDown{GLFW_KEY_Q};
            int lookLeft{GLFW_KEY_LEFT};
            int lookRight{GLFW_KEY_RIGHT};  
            int lookUp{GLFW_KEY_UP};
            int lookDown{GLFW_KEY_DOWN}; 
            int sprint{GLFW_KEY_LEFT_SHIFT};
            int jump{GLFW_KEY_SPACE};
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, M4GameObject& gameObject);
        void moveInXYZ(GLFWwindow* window, float dt, M4GameObject& gameObject);
        float mouseSensitivity{0.02f},cursorXOffset{0.0f},cursorYOffset{0.0f};
        double cursorXNow,cursorYNow,cursorXPrev,cursorYPrev;

        KeyMappings keys{};
        bool pitchEnabled{false};
        float moveSpeed{3.f};
        float baseSpeed{3.f};
        float sprintMultiplier{4.f};
        float lookSpeed{10.0f};
    };

}  // namespace m4