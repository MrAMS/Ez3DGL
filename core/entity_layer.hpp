#include "core/mesh_layer.hpp"
#include "core/vertices_layer.hpp"
#include "utils/debug.hpp"

namespace Ez3DGL{

class DynamicObj{
public:
    DynamicObj(glm::vec3 pos, float mass=1): position(pos), velocity(0), acceleration(0), mass(mass){}
    glm::vec3 update_dynamic(const glm::vec3 force, float delta_time_seconds){
        acceleration = force / mass;
        position += velocity * delta_time_seconds + acceleration * delta_time_seconds * delta_time_seconds * 0.5f;
        velocity += acceleration * delta_time_seconds;
        return position;
    }
    glm::vec3 pos() const{
        return position;
    }
    glm::vec3 vel() const{
        return velocity;
    }
    void set_vel(glm::vec3 vel){
        velocity = vel;
    }
private:
    float mass=1;
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

// class RenderObj{
// public:
//     RenderObj(Shader* shader, Model* model, model_t* mat_model):
//         shader(shader), model(model), mat_model(mat_model){}
//     void render(const Lights* lights, const camera_t* camera){
//         lights->apply_shader(shader);
//         model->draw(shader, camera, mat_model);
//     }
// private:
//     model_t* mat_model;
//     Model* model;
//     Shader* shader;

// };

}