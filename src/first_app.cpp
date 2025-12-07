#include "first_app.hpp"
#include "simple_render_system.hpp"
//libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

//std
#include <stdexcept>
#include <array>

namespace lbe {

    FirstApp::FirstApp() {
        loadGameObjects();
    }

    FirstApp::~FirstApp() {}

    //game loop
    void FirstApp::run() {

        SimpleRenderSystem simpleRenderSystem{lbeDevice, lbeRenderer.getSwapChainRenderPass()};
 

        while(!lbeWindow.shouldClose()) {
            glfwPollEvents();
            if(auto commandBuffer = lbeRenderer.beginFrame()) {
                lbeRenderer.beginSwapChainRenderPass(commandBuffer);

                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);

                lbeRenderer.endSwapChainRenderPass(commandBuffer);
                lbeRenderer.endFrame();
            }
        }
        vkDeviceWaitIdle(lbeDevice.device());
    } 

    void FirstApp::loadGameObjects(){

        std::vector<LbeModel::Vertex> vertices{
            {{0.0f, -0.5f},{1.0,0.0,0.0}},
            {{0.5f, 0.5f},{0.0,1.0,0.0}},
            {{-0.5f, 0.5f},{0.0,0.0,1.0}},
        }; 
        
        auto lbeModel = std::make_shared<LbeModel>(lbeDevice, vertices);

        auto triangle = LbeGameObject::createGameObject();
        triangle.model = lbeModel;
        triangle.color = {0.1f, 0.8f, 0.1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {2.0f, 0.5f};
        triangle.transform2d.rotation = .25* glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

}