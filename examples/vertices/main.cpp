#include "core/vertices_layer.hpp" // import this first!
#include "utils/preset.hpp"
#include "window/window.hpp"

using namespace Ez3DGL;

#define WIN_W 1200
#define WIN_H 800

vertices_t* vaos[3];

const auto vertices_ball = Ez3DGL::preset::vgen_ball::generate(20);
const auto vertices_cone_45 = Ez3DGL::preset::vgen_cone::generate(20, 45);

shader_t* shader_simple;

camera_t camera(WIN_W*1.0f/WIN_H, glm::vec3(0., 0., 10));

void user_setup(){
    vaos[0] = new vertices_t(36*(3+3+2), {3, 3, 2}, preset::cube_vertices_with_normals_texture, 0, NULL, GL_STATIC_DRAW);
    vaos[1] = new vertices_t(vertices_ball.size(), {3, 3, 2}, &vertices_ball[0], 0, NULL, GL_STATIC_DRAW);
    vaos[2] = new vertices_t(vertices_cone_45.size(), {3, 3, 2}, &vertices_cone_45[0], 0, NULL, GL_STATIC_DRAW);

    shader_simple = new shader_t("./shader/cube.vs", "./shader/cube.fs", "view", "projection", "model");
}

model_t model_matrixs [] = {model_t(glm::vec3(-2, 0, 0)), model_t(glm::vec3(0, 0, 0)), model_t(glm::vec3(2, 0, 0))};

float rotate_param = 10.f;

void user_loop(long int frame_delta_ms){
    static uint32_t loop_cnt = 0;
    loop_cnt += 1;
    const auto color = glm::vec3(loop_cnt%1000/1000.f);
    for(auto& model: model_matrixs){
        model.rotate(frame_delta_ms/rotate_param, glm::vec3(0, 1, 0));
        model.rotate(frame_delta_ms/rotate_param, glm::vec3(1, 0, 0));
    }

    shader_simple->use();
    shader_simple->update_camera(&camera);
    shader_simple->set_uniform("color", color);

    for(int i=0;i<3;++i){
        shader_simple->update_model(model_matrixs[i]);
        vaos[i]->draw_array(GL_TRIANGLES);
    }
}

void user_imgui(){
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();

    ImGui::SliderFloat("rotate_param", &rotate_param, 1, 100);
}


void user_exit(){
    for(int i=0;i<3;++i)
        delete vaos[i];
    delete shader_simple;
}

int main(int argc, const char** argv){
    return window_launch("Ez3DGL - vertices", WIN_W, WIN_H);
}