
# include "keyboard_movement_controller.hpp"
# include <limits>
# include <iostream>
namespace m4 {

    void KeyboardMovementController::moveInPlaneXZ(GLFWwindow* window, float dt, M4GameObject& gameObject) {
        glm::vec3 rotate{0.0f};

        if (glfwGetKey(window, keys.lookRight) == GLFW_PRESS) {rotate.y += 1.f;}
        if (glfwGetKey(window, keys.lookLeft) == GLFW_PRESS) {rotate.y -= 1.f;}
        if (glfwGetKey(window, keys.lookUp) == GLFW_PRESS) { rotate.x += 1.f;}
        if (glfwGetKey(window, keys.lookDown) == GLFW_PRESS) {rotate.x -= 1.f;}
        if (glfwGetKey(window, keys.sprint) == GLFW_PRESS) {moveSpeed=baseSpeed*sprintMultiplier;} 
        if (glfwGetKey(window, keys.sprint) == GLFW_RELEASE) {moveSpeed=baseSpeed;} 
        if (glfwGetKey(window, keys.jump) == GLFW_PRESS) {pitchEnabled = true;}
        if (glfwGetKey(window, keys.jump) == GLFW_RELEASE) {pitchEnabled = false;} 

        /*
            
            glfwSetCursorPosCallback:  
                continuous tracking (like drawing or camera movement) 
                your expected to provide a global function and pass that to glfwSetCursorPosCallback.
                https://stackoverflow.com/questions/29356783/glfwsetcursorposcallback-to-function-in-another-class
                
            glfwGetCursorPos: 
                discrete checks within a frame (e.g., checking position before a click). 
        */ 
       

        glfwGetCursorPos(window, &cursorXNow, &cursorYNow);
        if(cursorXPrev==0.0){cursorXPrev=cursorXNow;}
        if(cursorYPrev==0.0){cursorYPrev=cursorYNow;}

        cursorXOffset=(float)(cursorXNow - cursorXPrev)*mouseSensitivity;
        cursorYOffset=(float)(cursorYNow - cursorYPrev)*mouseSensitivity;

        cursorXPrev=cursorXNow;
        cursorYPrev=cursorYNow;
        rotate.y += cursorXOffset;
        rotate.x -= cursorYOffset;

        // rotation 0 will break the math, only rotate if > epsilon
        if (glm::dot(rotate,rotate) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.rotation += lookSpeed * dt *glm::normalize(rotate);
        }

        // limit pitch
        gameObject.transform.rotation.x = glm::clamp(
            gameObject.transform.rotation.x,
            -glm::radians(89.0f),
            glm::radians(89.0f));
        gameObject.transform.rotation.y = glm::mod(gameObject.transform.rotation.y, glm::two_pi<float>());
        
        float yaw = gameObject.transform.rotation.y;
        float pitch = gameObject.transform.rotation.x;

        float yRot=0.0f;
        if(pitchEnabled){
            yRot=-sin(pitch);
        }

        const glm::vec3 forwardDir{sin(yaw), yRot, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, yRot, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) {moveDir += forwardDir;}
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) {moveDir -= forwardDir;}
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) {moveDir += rightDir;}
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) {moveDir -= rightDir;}
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) {moveDir += upDir;}
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) {moveDir -= upDir;}
        //moveDir must be non zero to move
        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform.translation += moveSpeed * dt * glm::normalize(moveDir);
        }
    }

}  // namespace m4