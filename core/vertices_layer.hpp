/**
 * @file vertices_layer.hpp
 * @author Santiego (2421653893@qq.com)
 * @brief 顶点层面封装
 * @version 0.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <cstdlib>
#include <glad/glad.h>
#include <stdio.h>
#include <math.h>
#include <glm/glm.hpp>
#include <initializer_list>
#include <memory>
#include <vector>

namespace Ez3DGL {

class texture_t;
class camera_t;

/**
 * @brief 着色器对象,支持着色器编译检查,纹理自动绑定管理
 * 
 */
class shader_t{
    public:
        unsigned int program_id, vertex_id, fragment_id;

        shader_t(const char* vertex_shader_path, const char* fragment_shader_path,
                   const char* view_key, const char* proj_key, const char* model_key);
        void use() const;
        void blind_texture(const char* texture_key, class texture_t* texture);
        void update_camera(struct camera_t *camera);
        void update_model(class model_t *model);

        void set_uniform(const char* key, bool val) const;
        void set_uniform(const char* key, int val) const;
        void set_uniform(const char* key, unsigned int val) const;
        void set_uniform(const char* key, float val) const;
        void set_uniform(const char* key, const glm::mat4 &mat) const;
        void set_uniform(const char* key, const glm::vec3 &val) const;
        void set_uniform(const char* key, float x, float y, float z) const;
        void set_uniform(const char* key, const glm::vec4 &val) const;
        void set_uniform(const char* key, float x, float y, float z, float w) const;
        
        ~shader_t();
    private:
        const char* vertex_shader_path;
        const char* fragment_shader_path;
        const char* view_key;
        const char* proj_key;
        const char* model_key;
        // utility function for checking shader compilation/linking errors.
        void check_compile_errors(unsigned int shader, const char* type);
        unsigned int texture_cnt=0;
        std::vector<texture_t*> texture_blinded;
        int get_uniform_loc(const char* key) const;
};

/**
 * @brief 纹理对象,读取图片生成纹理
 * @note 一般来说,读取PNG图片时,color_format=RGBA,JPG等图片时,color_format=RGB
 *
 */
class texture_t{
    public:
        unsigned int texture_id;
        bool valid = false;

        texture_t(const char* file_name, GLenum color_format);
    private:
        const char* file_name;
        
};

/**
 * @brief 顶点对象,封装VAO,VBO,EBO于一体
 * 
 */
class vertices_t{
public:
    // Vertex Array ID
    unsigned int VAO_id;
    // Vertex Buffer ID
    unsigned int VBO_id;
    // Element Buffer
    unsigned int EBO_id;
    // Vertex number
    unsigned int v_cnt;
    // Element number
    unsigned int e_cnt;

    vertices_t(unsigned int vertex_num, std::initializer_list<unsigned int> vertex_div, const float* vertex_data,
                        unsigned int element_num, const unsigned int* element_data,
                        GLenum buffer_usage=GL_STATIC_DRAW);
    ~vertices_t();
    void draw_array(GLenum draw_mode, int beg, int num) const;
    void draw_array(GLenum draw_mode) const;
    void draw_element(GLenum draw_mode) const;
};

/**
 * @brief 摄像机对象,三维观察显示,封装了view,projection矩阵
 * 
 */
class camera_t{
    public:
        glm::vec3 position, up, front;
        float pitch, yaw;

        // field of view
        float fov = 45.0f;
        float max_fov = 0;

        float screen_w_div_h;

        glm::mat4 view;
        glm::mat4 projection;

        // input key sensitivity
        float sensitivity;

        enum dir{LEFT, RIGHT, UP, DOWN};

        explicit camera_t(float screen_w_div_h_, glm::vec3 position_ = glm::vec3(0.0f, 0.0f, 3.0f),
                    glm::vec3 up_ = glm::vec3(0.0f, 1.0f, 0.0f), float yaw_ = -90.0f, float pitch_ = 0,
                    float sensitivity_=0.05, float fov_=45.0f, float max_fov_=75.0f);

        void calc_view();
        void calc_projection();
        
        void change_pos(enum dir move_dir, float step);
        void change_pitch_yaw(float x_offset, float y_offset);
        // change camera's field of view
        void change_fov(float target_fov);

        // input position by keyboard
        void input_pos(enum dir move_dir, float delta_time);
        // input camera's pitch & yaw by mouse
        void input_pitch_yaw(double cur_x, double cur_y);
        // input camera's pitch & yaw by scroll
        void input_fov(double scroll);
        
};

/**
 * @brief model对象,封装了model矩阵,方便缩放平移旋转物体,支持父子绑定
 * 
 */
class model_t{
public:
    glm::vec3 pos;
    glm::vec3 scale=glm::vec3(1.);
    glm::vec3 rotate_axis=glm::vec3(0, 0, 1);
    float rotate_degree=0;

    model_t()=default;
    model_t(glm::vec3 pos, glm::vec3 scale=glm::vec3(1.), class model_t* p=nullptr);

    glm::mat4 get_model();
    glm::mat4 move_to(glm::vec3 pos);
    glm::mat4 move_to(float x, float y, float z);
    glm::mat4 scale_to(float x);
    glm::mat4 scale_to(glm::vec3 x);
    glm::mat4 scale_to(float x, float y, float z);
    glm::mat4 rotate_to(float degree, glm::vec3 axis);
    void set_parent_model(class model_t* p);

    glm::mat4 get_trans_mat(glm::vec3 pos, glm::vec3 scale, float rotate_degree, glm::vec3 rotate_axis);
private:
    class model_t* parent = nullptr;
};

/**
 * @brief 立方碰撞体,通过model检测碰撞
 * 
 */
class collision_box_t{
public:
    class model_t* model=nullptr;
    glm::vec3 vel_dir;

    explicit collision_box_t(class model_t* model, glm::vec3 vel_dir=glm::vec3(0, -1, 0));

    bool check_collision(class collision_box_t* other);
};

class light_base_t{
public:
    glm::vec3 ambient=glm::vec3(0.2f, 0.2f, 0.2f);
    glm::vec3 diffuse=glm::vec3(0.5f, 0.5f, 0.5f);
    glm::vec3 specular=glm::vec3(1.0f, 1.0f, 1.0f);

    void apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular);
};

class light_dir_t : public light_base_t{
public:
    glm::vec3 direction;

    void apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular,
        const char* key_direction);
};

class light_point_t : public light_base_t{
public:
    glm::vec3 position;

    float constant = 1.f;
    float linear = 0.0014f;
    float quadratic = 0.000007f;

    void apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular,
        const char* key_position, const char* key_constant, const char* key_linear, const char* key_quadratic);
};

class light_spot_t : public light_base_t{
public:
    glm::vec3 direction;
    glm::vec3 position;

    float cutoff = glm::cos(glm::radians(12.5f));
    float cutoff_outer = glm::cos(glm::radians(17.5f));

    float constant = 1.f;
    float linear = 0.0014f;
    float quadratic = 0.000007f;

    void set_cutoff_angle(float inner_degree, float outer_degree);

    void apply2shader(shader_t* shader,
        const char* key_ambient, const char* key_diffuse, const char* key_specular,
        const char* key_position, const char* key_direction,
        const char* key_cutoff, const char* key_cutoff_outer,
        const char* key_constant, const char* key_linear, const char* key_quadratic);
};

}