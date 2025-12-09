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
        };

        void moveInPlaneXZ(GLFWwindow* window, float dt, M4GameObject& gameObject);
        float mouseSensitivity{0.02f},xOffset{0.0f},yOffset{0.0f};
        double xpos,ypos,oldx,oldy;

        KeyMappings keys{};
        float moveSpeed{3.f};
        float lookSpeed{1.5f};
    };

}  // namespace m4