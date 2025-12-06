#pragma once
#include "lbe_model.hpp"
//std
#include <memory>


namespace lbe
{

    struct Transform2dComponent {
        glm::vec2 translation{};
        glm::vec2 scale{1.0f, 1.0f};
        float rotation{};

        glm::mat2 mat2(){   
            const float cosine = cos(rotation);
            const float sine = sin(rotation);
            glm::mat2 rotationMat{{cosine, sine}, {-sine, cosine}};

            glm::mat2 scaleMat{{scale.x, 0.0f}, {0.0f, scale.y}};
            return rotationMat * scaleMat;    
        }
    };

    class LbeGameObject{
    public:
        using id_t =unsigned int;

        static LbeGameObject createGameObject(){
            static id_t currentId = 0;
            return LbeGameObject{currentId++};
        }

        LbeGameObject(const LbeGameObject&) = delete;
        LbeGameObject& operator=(const LbeGameObject&) = delete;
        LbeGameObject(LbeGameObject&&) = default;
        LbeGameObject& operator=(LbeGameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<LbeModel> model;
        glm::vec3 color{};
        Transform2dComponent transform2d;

    private:
        LbeGameObject(id_t objId) : id{objId} {}
        id_t id;

    };
} // namespace lbe