#include "core/mesh_layer.hpp"

namespace Ez3DGL{

class MotionObj{
public:
    float mass=1;
    void apply_force(glm::vec3 force){
        acceleration = force / mass;
    }
    glm::vec3 update(float delta_time_seconds){
        position += velocity * delta_time_seconds + acceleration * delta_time_seconds * delta_time_seconds * 0.5f;
        return position;
    }
    glm::vec3 pos() const{
        return position;
    }
    glm::vec3 vel() const{
        return velocity;
    }
private:
    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration;
};

class RenderObj{
public:
    void render(){
        // render
    }
private:
    Model* model;
    Shader* shader;

};

}