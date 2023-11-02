#include "core/vertices_layer.hpp"
#include <cassert>
#include <cstddef>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <cmath>
#include "utils/debug.hpp"

using namespace Ez3DGL;


static double get_distance(glm::vec3 a, glm::vec3 b){
    return sqrt(pow(a.x-b.x, 2) + pow(a.y-b.y, 2) + pow(a.z-b.z, 2));
}

shader_t::shader_t(const char* vertex_shader_path, const char* fragment_shader_path,
                       const char* view_key, const char* proj_key, const char* model_key):
                       vertex_shader_path(vertex_shader_path), fragment_shader_path(fragment_shader_path),
                       view_key(view_key), proj_key(proj_key), model_key(model_key), texture_cnt(0){
    std::string vertexCode;
    std::string fragmentCode;
    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    // 保证ifstream对象可以抛出异常：
    vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
    try 
    {
        // 打开文件
        vShaderFile.open(vertex_shader_path);
        fShaderFile.open(fragment_shader_path);
        std::stringstream vShaderStream, fShaderStream;
        // 读取文件的缓冲内容到数据流中
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();       
        // 关闭文件处理器
        vShaderFile.close();
        fShaderFile.close();
        // 转换数据流到string
        vertexCode   = vShaderStream.str();
        fragmentCode = fShaderStream.str();     
    }
    catch(std::ifstream::failure e)
    {
        std::cout << "[File ERROR] Fail to open shader file" << ",vertex:" << vertex_shader_path << ",fragment:"  << fragment_shader_path << std::endl;
        // assert_with_info(0, FMT, ...)
    }
    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();
    // 2. compile shaders
    // vertex shader
    vertex_id = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_id, 1, &vShaderCode, NULL);
    glCompileShader(vertex_id);
    check_compile_errors(vertex_id, "VERTEX");
    // fragment Shader
    fragment_id = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_id, 1, &fShaderCode, NULL);
    glCompileShader(fragment_id);
    check_compile_errors(fragment_id, "FRAGMENT");
    // shader Program
    program_id = glCreateProgram();
    glAttachShader(program_id, vertex_id);
    glAttachShader(program_id, fragment_id);
    glLinkProgram(program_id);
    check_compile_errors(program_id, "PROGRAM");
    // delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}

void shader_t::use() const{
    glUseProgram(program_id);
}

int shader_t::get_uniform_loc(const char* key) const{
    int ret = glGetUniformLocation(program_id, key);
    assert_with_info(ret!=-1, "Invaild key: %s", key);
    return ret;
}
void shader_t::set_uniform(const char* key, bool val) const{
    set_uniform(key, (int)val);
}

void shader_t::set_uniform(const char* key, int val) const{
    glUniform1i(get_uniform_loc(key), val);
}

void shader_t::set_uniform(const char* key, unsigned int val) const{
    glUniform1i(get_uniform_loc(key), val);
}

void shader_t::set_uniform(const char* key, float val) const{
    glUniform1f(get_uniform_loc(key), val); 
}

void shader_t::set_uniform(const char* key, const glm::mat4 &mat) const{
    glUniformMatrix4fv(get_uniform_loc(key), 1, GL_FALSE, glm::value_ptr(mat));
}

void shader_t::set_uniform(const char* key, const glm::vec3 &val) const{
    glUniform3fv(get_uniform_loc(key), 1, &val[0]); 
}

void shader_t::set_uniform(const char* key, const float x, const float y, const float z) const{
    glUniform3f(get_uniform_loc(key), x, y, z);
}

void shader_t::set_uniform(const char* key, const glm::vec4 &val) const{
    glUniform4fv(get_uniform_loc(key), 1, &val[0]); 
}

void shader_t::set_uniform(const char* key, const float x, const float y, const float z, const float w) const{
    glUniform4f(get_uniform_loc(key), x, y, z, w);
}

void shader_t::blind_texture(const char *texture_key, struct texture_t* texture) {
    assert_with_info(texture->valid, "blind texture %s fail", texture_key);
    use();
    bool hav=false;
    unsigned int unit_id = 0;
    for(int i=0;i<texture_blinded.size();++i){
        auto tt = texture_blinded[i];
        if(tt->texture_id == texture->texture_id){
            unit_id = i;
            hav=true;
            break;
        }
    }
    if(!hav){
        unit_id = texture_cnt++;
        texture_blinded.push_back(texture);
    }
    assert_with_info(unit_id<16, "too much texture to blind");
    glActiveTexture(GL_TEXTURE0+unit_id);
    glBindTexture(GL_TEXTURE_2D, texture->texture_id);
    set_uniform(texture_key, unit_id);
}

void shader_t::update_camera(struct camera_t *camera){
    set_uniform(proj_key, camera->projection);
    set_uniform(view_key, camera->view);
}

void shader_t::update_model(class model_t *model) {
    set_uniform(model_key, model->get_model());
}

void shader_t::check_compile_errors(unsigned int shader, const char* type)
{
    int success;
    char infoLog[1024];
    if (strcmp(type, "PROGRAM")!=0)
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            if (strcmp(type, "VERTEX")==0){
                printf("[%s Shader ERROR] %s%s\n", type, vertex_shader_path, infoLog+1);
            }else{
                printf("[%s Shader ERROR] %s%s\n", type, fragment_shader_path, infoLog+1);
            }
            exit(1);
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            printf("[%s Program ERROR] %s\n", type, infoLog);
        }
    }
}    

shader_t::~shader_t(){
    //glDeleteProgram(program_id);
}

texture_t::texture_t(const char* file_name, GLenum color_format):file_name(file_name){
    if(file_name==NULL) return;
    int width, height, nrCh;
    unsigned char* data = stbi_load(file_name, &width, &height, &nrCh, 0);
    stbi_set_flip_vertically_on_load(true);
    glGenTextures(1, &texture_id);
    glBindTexture(GL_TEXTURE_2D, texture_id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if(data){
        glTexImage2D(GL_TEXTURE_2D, 0, color_format, width, height, 0, color_format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        printf("[OK] Texture %s %d*%d %dchs readed.\n", file_name, height, width, nrCh);
    }else{
        printf("[File ERROR] Fail to load texture.\n");
        return;
    }
    stbi_image_free(data);
    valid = true;
}

vertices_t::vertices_t(unsigned int vertex_num, std::initializer_list<unsigned int> vertex_div, const float* vertex_data,
                            unsigned int element_num, const unsigned int* element_data, 
                            GLenum buffer_usage){

    v_cnt = vertex_num;
    e_cnt = element_num;
    unsigned int vertex_per_size = 0;
    for(const unsigned int &item : vertex_div)
        vertex_per_size += item;
    // Vertex Array
    glGenVertexArrays(1, &VAO_id);
    
    glBindVertexArray(VAO_id);

    // Vertex Buffer 
    glGenBuffers(1, &VBO_id);

    // Element Buffer
    if(element_num != 0)
        glGenBuffers(1, &EBO_id);
        

    glBindBuffer(GL_ARRAY_BUFFER, VBO_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertex_per_size*vertex_num, vertex_data, buffer_usage);

    if(element_num != 0){
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO_id);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int)*element_num, element_data, buffer_usage);
    }

    int i=0, j=0;
    for(const unsigned int &item : vertex_div){
        // glVertexAttribPointer(i, item, GL_FLOAT, GL_FALSE, 0, (void*)(j*sizeof(float)));
        glVertexAttribPointer(i, item, GL_FLOAT, GL_FALSE, vertex_per_size*sizeof(float), (void*)(j*sizeof(float)));
        glEnableVertexAttribArray(i);
        i+=1;
        j+=item;
    }
    
    glBindVertexArray(0);

}

vertices_t::~vertices_t(){
    /*
    glDeleteVertexArrays(1, &VAO_id);
    glDeleteBuffers(1, &VBO_id);
    if(e_cnt != 0)
        glDeleteBuffers(1, &EBO_id);
    */
}

void vertices_t::draw_array(GLenum draw_mode, int beg, int num) const{
    glBindVertexArray(VAO_id);
    glDrawArrays(draw_mode, beg, num);
}

void vertices_t::draw_array(GLenum draw_mode) const {
    draw_array(draw_mode, 0, v_cnt);
}
void vertices_t::draw_element(GLenum draw_mode) const{
    assert_with_info(e_cnt!=0, "Fail to draw elements due to e_cnt=0");
    glBindVertexArray(VAO_id);
    glDrawElements(draw_mode, e_cnt, GL_UNSIGNED_INT, 0);
}


camera_t::camera_t(float screen_w_div_h_, glm::vec3 position_,
                    glm::vec3 up_, float yaw_, float pitch_,
                    float sensitivity_, float fov_, float max_fov_)
                    :screen_w_div_h(screen_w_div_h_), position(position_), up(up_), yaw(yaw_), pitch(pitch_), sensitivity(sensitivity_), fov(fov_), max_fov(max_fov_){
    
    front = glm::vec3(0.0f, 0.0f, -1.0f);

    calc_projection();
    calc_view();
}

void camera_t::calc_view(){
    view = glm::lookAt(position, position + front, up);
}

void camera_t::calc_projection(){
    projection = glm::perspective(glm::radians(fov), screen_w_div_h, 0.1f, 500.0f);
}

void camera_t::change_pos(enum dir move_dir, float step){
    if (move_dir == UP)
        position += step * front;
    if (move_dir == DOWN)
        position -= step * front;
    if (move_dir == LEFT)
        position -= glm::normalize(glm::cross(front, up)) * step;
    if (move_dir == RIGHT)
        position += glm::normalize(glm::cross(front, up)) * step;
}

void camera_t::change_pitch_yaw(float x_offset, float y_offset){
    yaw   += x_offset;
    pitch += y_offset;

    if(pitch > 89.0f)
        pitch = 89.0f;
    if(pitch < -89.0f)
        pitch = -89.0f;

    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    front = glm::normalize(front);
}

void camera_t::change_fov(float target_fov){
    fov = target_fov;
}

void camera_t::input_pos(enum dir move_dir, float delta_time){
    auto speed = static_cast<float>(5 * delta_time);
    change_pos(move_dir, speed);
}

void camera_t::input_pitch_yaw(double cur_x, double cur_y){
    static bool is_first = true;
    static float last_x, last_y;
    if(is_first)
    {
        last_x = cur_x;
        last_y = cur_y;
        is_first = false;
    }

    float xoffset = cur_x - last_x;
    float yoffset = last_y - cur_y; 
    last_x = cur_x;
    last_y = cur_y;

    xoffset *= sensitivity;
    yoffset *= sensitivity;

    change_pitch_yaw(xoffset, yoffset);
}

void camera_t::input_fov(double scroll){
    if(fov >= 1.0f && fov <= max_fov)
        fov -= scroll*2;
    if(fov <= 1.0f)
        fov = 1.0f;
    if(fov >= max_fov)
        fov = max_fov;
}

glm::mat4 model_t::get_model(){
    return get_trans_mat(pos, scale, rotate_degree, rotate_axis);
}

glm::mat4 model_t::move_to(glm::vec3 x){
    pos = x;
    return get_model();
}

glm::mat4 model_t::move_to(float x, float y, float z){
    pos = glm::vec3(x, y, z);
    return get_model();
}

glm::mat4 model_t::scale_to(float x){
    scale = glm::vec3(x, x, x);
    return get_model();
}

glm::mat4 model_t::scale_to(float x, float y, float z){
    scale = glm::vec3(x, y, z);
    return get_model();
}

glm::mat4 model_t::scale_to(glm::vec3 x){
    scale = x;
    return get_model();
}

glm::mat4 model_t::rotate_to(float x, glm::vec3 axis){
    rotate_degree = x;
    rotate_axis = axis;
    return get_model();
}

void model_t::set_parent_model(struct model_t *p) {
    parent = p;
}

glm::mat4 model_t::get_trans_mat(glm::vec3 pos, glm::vec3 scale, float rotate_degree, glm::vec3 rotate_axis){
    auto trans = glm::mat4(1.);
    if(parent)
        trans = parent->get_model();
    trans = glm::translate(trans, pos);
    trans = glm::rotate(trans, glm::radians(rotate_degree), rotate_axis);
    trans = glm::scale(trans, scale);
    return trans;
}

model_t::model_t(glm::vec3 pos, glm::vec3 scale, class model_t *p):pos(pos), scale(scale), parent(p)  {

}

bool collision_box_t::check_collision(struct collision_box_t *other) {
    auto my_pos = model->pos;
    auto other_pos = other->model->pos;
    auto my_scale = model->scale;
    auto other_scale = other->model->scale;
    bool on_x = fabs(my_pos.x - other_pos.x) <= (my_scale.x + other_scale.x);
    bool on_y = fabs(my_pos.y - other_pos.y) <= (my_scale.y + other_scale.y);
    bool on_z = fabs(my_pos.z - other_pos.z) <= (my_scale.z + other_scale.z);
    return on_x && on_y && on_z;
}

collision_box_t::collision_box_t(struct model_t *model, glm::vec3 vel_dir):model(model) {

}

void light_base_t::apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular){
    shader->set_uniform(key_ambient, ambient);
    shader->set_uniform(key_diffuse, diffuse);
    shader->set_uniform(key_specular, specular);
}

void light_dir_t::apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular,
        const char* key_direction){
    shader->set_uniform(key_direction, direction);
    light_base_t::apply2shader(shader, key_ambient, key_diffuse, key_specular);
}

void light_point_t::apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular,
        const char* key_position, const char* key_constant, const char* key_linear, const char* key_quadratic){
    light_base_t::apply2shader(shader, key_ambient, key_diffuse, key_specular);
    shader->set_uniform(key_position, position);
    shader->set_uniform(key_constant, constant);
    shader->set_uniform(key_linear, linear);
    shader->set_uniform(key_quadratic, quadratic);

}

