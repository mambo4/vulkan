#pragma once
#include "m4_model.hpp"
//libs
#include <gtc/matrix_transform.hpp>
//std
#include <memory>


namespace m4
{

    struct TransformComponent {
        glm::vec3 translation{};
        glm::vec3 scale{1.0f, 1.0f, 1.0f};
        glm::vec3 rotation{};

        // Matrix corrsponds to Translate * Ry * Rx * Rz * Scale
        // Rotations correspond to Tait-bryan angles of Y(1), X(2), Z(3)
        // https://en.wikipedia.org/wiki/Euler_angles#Rotation_matrix
        glm::mat4 mat4();
        glm::mat3 normalMatrix();
    };

    class M4GameObject{
    public:
        using id_t =unsigned int;

        static M4GameObject createGameObject(){
            static id_t currentId = 0;
            return M4GameObject{currentId++};
        }

        M4GameObject(const M4GameObject&) = delete;
        M4GameObject& operator=(const M4GameObject&) = delete;
        M4GameObject(M4GameObject&&) = default;
        M4GameObject& operator=(M4GameObject&&) = default;

        id_t getId() { return id; }

        std::shared_ptr<M4Model> model;
        glm::vec3 color{};
        TransformComponent transform{};

    private:
        M4GameObject(id_t objId) : id{objId} {}
        id_t id;

    };
} // namespace m4