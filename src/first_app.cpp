#include "first_app.hpp"
#include "systems/simple_render_system.hpp"//?
#include "systems/point_light_system.hpp"//?
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

//constants 
#define CLIP_Z 100.0f


namespace m4 {

    struct GlobalUbo {
        glm::mat4 projection{1.0f};
        glm::mat4 view{1.0f};
        glm::vec4 ambientLightColor{0.2f, 0.0f, 0.0f, 0.02f}; //RGBI
        glm::vec3 lightPosition{-1.0f};
        alignas(16)glm::vec4 lightColor{1.0f};//RGBI
    };

    FirstApp::FirstApp() {
        globalPool = M4DescriptorPool::Builder(m4Device)
            .setMaxSets(M4SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, M4SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();
        
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    //game loop
    void FirstApp::run() {

        //create UBO buffer foreach MAX_FRAMES_IN_FLIGHT
        std::vector<std::unique_ptr<M4Buffer>> uboBuffers(M4SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(size_t i = 0; i < uboBuffers.size(); i++) {
            uboBuffers[i] = std::make_unique<M4Buffer>(
                m4Device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        auto globalSetLayout = M4DescriptorSetLayout::Builder(m4Device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();   

        std::vector<VkDescriptorSet> globalDescriptorSets(M4SwapChain::MAX_FRAMES_IN_FLIGHT);
        for(size_t i = 0; i < globalDescriptorSets.size(); i++) {
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            M4DescriptorWriter(*globalSetLayout, *globalPool)
                .writeBuffer(0, &bufferInfo)
                .build(globalDescriptorSets[i]);
        }

        SimpleRenderSystem simpleRenderSystem{m4Device, m4Renderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout()};
        PointLightSystem pointLightSystem{m4Device, m4Renderer.getSwapChainRenderPass(),globalSetLayout->getDescriptorSetLayout()};
        M4Camera camera{};
        camera.setViewTarget(glm::vec3{-1.0f,-1.0f, -2.5f}, glm::vec3{0.0f, 0.0f,  2.5f});

        

        auto viewerObject = M4GameObject::createGameObject();
        viewerObject.transform.translation = {0.0f, 0.0f, -2.5f};
        KeyboardMovementController cameraController{};
        cameraController.oldx=0.0;
        cameraController.oldy=0.0;
        auto currentTime = std::chrono::high_resolution_clock::now();
        
        glfwSetInputMode(m4Window.getGLFWwindow(), GLFW_CURSOR, GLFW_CURSOR_DISABLED);

        while(!m4Window.shouldClose()) {
            glfwPollEvents();
            
            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            frameTime = std::min(frameTime,MAX_FRAME_TIME ); //cap max frame time to 16ms

            cameraController.moveInPlaneXZ(m4Window.getGLFWwindow(), frameTime, viewerObject);
            camera.setViewYXZ(viewerObject.transform.translation, viewerObject.transform.rotation);

            float aspect = m4Renderer.getAspectRatio();
            camera.setPerspectiveProjection(glm::radians(50.0f), aspect,0.1f,CLIP_Z);

            if(auto commandBuffer = m4Renderer.beginFrame()) {
                int frameIndex = m4Renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex,
                    frameTime,
                    commandBuffer,
                    camera,
                    globalDescriptorSets[frameIndex],
                    gameObjects
                };
                //update
                GlobalUbo ubo{};
                ubo.projection = camera.getProjection();
                ubo.view = camera.getView();
                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();    

                //render
                m4Renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(frameInfo);
                pointLightSystem.render(frameInfo);
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
        object_1.transform.translation = {-0.5f, 0.5f, 0.0f}; // translate cube back(+) from 0.0z to be in viewing volume
        object_1.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(object_1.getId(),std::move(object_1));

        M4Model = M4Model::createModelFromFile(m4Device,"../models/smooth_vase.obj");
        auto object_2 = M4GameObject::createGameObject();
        object_2.model = M4Model;
        object_2.transform.translation = {0.5f, 0.5f, 0.0f}; // translate cube back(+) from 0.0z to be in viewing volume
        object_2.transform.scale = {3.0f, 1.5f, 3.0f};
        gameObjects.emplace(object_2.getId(),std::move(object_2));

        M4Model = M4Model::createModelFromFile(m4Device,"../models/quad.obj");
        auto object_3 = M4GameObject::createGameObject();
        object_3.model = M4Model;
        object_3.transform.translation = {0.0f, 0.5f, 0.0f}; // translate cube back(+) from 0.0z to be in viewing volume
        object_3.transform.scale = {3.0f, 1.0f, 3.0f};
        gameObjects.emplace(object_3.getId(),std::move(object_3));
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                    
    }

}