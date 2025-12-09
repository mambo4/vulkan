#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "m4_camera.hpp"
#include "keyboard_movement_controller.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <chrono>
#include <stdexcept>
#include <array>

namespace m4 {

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    //game loop
    void FirstApp::run() {

        SimpleRenderSystem simpleRenderSystem{m4Device, m4Renderer.getSwapChainRenderPass()};
        M4Camera camera{};
        camera.setViewTarget(glm::vec3{-1.0f,-1.0f, -2.5f}, glm::vec3{0.0f, 0.0f,  2.5f});

        auto viewerObject = M4GameObject::createGameObject();
        KeyboardMovementController cameraController{};
        cameraController.oldx=0.0;
        cameraController.oldy=0.0;
        auto currentTime = std::chrono::high_resolution_clock::now();

        while(!m4Window.shouldClose()) {
            glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = std::min(frameTime,MAX_FRAME_TIME ); //cap max frame time to 16ms

            cameraController.moveInPlaneXZ(m4Window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = m4Renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect,0.1f,10.0f);
            if(auto commandBuffer = m4Renderer.beginFrame()) {
                m4Renderer.beginSwapChainRenderPass(commandBuffer);

                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects,camera);

                m4Renderer.endSwapChainRenderPass(commandBuffer);
                m4Renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(m4Device.device());
    } 

    // temporary helper function, creates a 1x1x1 cube centered at offset
    std::unique_ptr<M4Model> createCubeModel(M4Device& device, glm::vec3 offset) {
        M4Model::M4Mesh modelBuilder;
        modelBuilder.vertices={
        
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
        
            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
        
            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
        
            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
        
            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
        
            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
        };

    for (auto& v : modelBuilder.vertices) {
        v.position += offset;
    }
    return std::make_unique<M4Model>(device, modelBuilder);
    }

    void FirstApp::loadGameObjects(){
        std::shared_ptr<M4Model> M4Model = createCubeModel(m4Device, {0.0f, 0.0f, 0.0f});
        auto cube = M4GameObject::createGameObject();
        cube.model = M4Model;
        cube.transform.translation = {0.0f, 0.0f, 2.5f}; // translate cube back(+) from 0.0z to be in viewing volume
        cube.transform.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(cube));
    }

}