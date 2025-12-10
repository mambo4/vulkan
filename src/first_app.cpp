#include "first_app.hpp"
#include "simple_render_system.hpp"
#include "m4_camera.hpp"
#include "keyboard_movement_controller.hpp"

//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm.hpp>
#include <gtc/constants.hpp>

//std
#include <chrono>
#include <stdexcept>
#include <array>

namespace m4 {

    struct GlobalUbo {
        glm::mat4 projectionView{1.0f};
        glm::vec3 lightDirection{1.0f, -3.0f, -1.0f};
    };

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    //game loop
    void FirstApp::run() {

        M4Buffer globalUboBuffer{
            m4Device,
            sizeof(GlobalUbo),
            M4SwapChain::MAX_FRAMES_IN_FLIGHT,
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            m4Device.properties.limits.minUniformBufferOffsetAlignment
        };

        globalUboBuffer.map();

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
                int frameIndex = m4Renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera
                };
                //update
                GlobalUbo ubo{};
                ubo.projectionView = camera.getProjection() * camera.getView();
                globalUboBuffer.writeToIndex(&ubo, m4Renderer.getFrameIndex());
                globalUboBuffer.flushIndex(m4Renderer.getFrameIndex());    

                //render
                m4Renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo, gameObjects);
                m4Renderer.endSwapChainRenderPass(commandBuffer);
                m4Renderer.endFrame();
            }
        }
        vkDeviceWaitIdle(m4Device.device());
    } 

    void FirstApp::loadGameObjects(){
        std::shared_ptr<M4Model> M4Model = M4Model::createModelFromFile(m4Device,"../models/flat_vase.obj");
        auto object_1 = M4GameObject::createGameObject();
        object_1.model = M4Model;
        object_1.transform.translation = {-0.5f, 0.5f, 2.5f}; // translate cube back(+) from 0.0z to be in viewing volume
        object_1.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.push_back(std::move(object_1));

        M4Model = M4Model::createModelFromFile(m4Device,"../models/smooth_vase.obj");
        auto object_2 = M4GameObject::createGameObject();
        object_2.model = M4Model;
        object_2.transform.translation = {0.5f, 0.5f, 2.5f}; // translate cube back(+) from 0.0z to be in viewing volume
        object_2.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.push_back(std::move(object_2));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    }

}