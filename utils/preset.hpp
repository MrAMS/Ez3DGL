/**
 * @file preset.hpp
 * @author Santiego (2421653893@qq.com)
 * @brief 实用预设
 * @version 0.1
 * @date 2023-10-26
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once

#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

namespace Ez3DGL {
    namespace preset {
        /**
         * @brief 带有三维顶点的立方体顶点数据
         * @note 请使用 draw_array 进行绘制.
         * 
         */
        extern const float cube_vertices_with_texture[];
        /**
         * @brief 带有三维顶点,法向量,纹理坐标的立方体顶点数据
         * @note 请使用 draw_array 进行绘制.
         * 
         */
        extern const float cube_vertices_with_normals_texture[];

        /**
        * @brief 旋转面顶点生成器,通过指定二维轮廓点集,生成对应三维旋转面的顶点({三维顶点,法向量,纹理坐标})
        * @note 注意输入的二维轮廓坐标应在范围[-0.5, 0.5],否则纹理坐标可能会出现问题. 请使用 draw_array 进行绘制.
        * 
        */
        class vgen_revolu_surf{
        public:

            /**
            * @brief 生成顶点数组
            
            * @param plane_num 面数
            * @param rotate_axis 旋转轴
            * @param outlines x-y平面上, 二维轮廓点集,范围[-0.5, 0.5]
            * @return std::vector<float> 顶点数组
            */
            static std::vector<float> generate(int plane_num, const std::vector<glm::vec2>& outlines, const glm::vec3& rotate_axis=glm::vec3(0.0, 0.0, 1.0)){
                std::vector<float> vertices;
                float delta_degree = 360.f/plane_num;
                for(int i=0;i<outlines.size()-1;++i){
                    for(int slice_i=0;slice_i<plane_num;++slice_i){
                        float degree = slice_i*delta_degree;
                        auto p1_2d = outlines[i];
                        auto p3_2d = outlines[i+1];
                        push_surface(&vertices, p1_2d, p3_2d, degree, delta_degree, rotate_axis);
                    }
                }
                return vertices;
            }
            
        private:
            static glm::vec3 get_3d_point(glm::vec2 point, float degree, const glm::vec3& rotate_axis){
                const auto v = glm::vec4(point.x, point.y, 0, 1);
                const auto res = glm::rotate(glm::mat4(1.0), (float)glm::radians(degree), rotate_axis) * v;
                return res;
            }
            static void push_point(std::vector<float> *target, glm::vec3 point, glm::vec3 normal, float texture_x, float texture_y){
                target->push_back(point.x);
                target->push_back(point.y);
                target->push_back(point.z);
                target->push_back(normal.x);
                target->push_back(normal.y);
                target->push_back(normal.z);
                target->push_back(texture_x);
                target->push_back(texture_y);
            }
            static glm::vec3 convert_nan(glm::vec3 x, float num=0){
                if(std::isnan(x.x)) x.x = num;
                if(std::isnan(x.y)) x.y = num;
                if(std::isnan(x.z)) x.z = num;
                return x;
            }
            static void push_surface(std::vector<float> *target, glm::vec2 p1, glm::vec2 p3, float degree, float degree_delta, const glm::vec3& rotate_axis){
                /*
                draw a plane P1P2P4P3
                P1--P2
                |  /|
                | / |
                |/  |
                P3--P4
                */

                auto p1_3d = get_3d_point(p1, degree, rotate_axis);
                auto p2_3d = get_3d_point(p1, degree+degree_delta, rotate_axis);
                auto p3_3d = get_3d_point(p3, degree, rotate_axis);
                auto p4_3d = get_3d_point(p3, degree+degree_delta, rotate_axis);

                auto texture_x1 = degree/360;
                auto texture_x2 = (degree+degree_delta)/360;
                auto texture_y1 = p1.y+0.5; // note [-0.5, 0.5]
                auto texture_y2 = p3.y+0.5;

                glm::vec3 normal1 = convert_nan(glm::triangleNormal(p1_3d, p2_3d, p3_3d));
                glm::vec3 normal2 = convert_nan(glm::triangleNormal(p2_3d, p4_3d, p3_3d));

                push_point(target, p1_3d, normal1, texture_x1, texture_y1);
                push_point(target, p2_3d, normal1, texture_x2, texture_y1);
                push_point(target, p3_3d, normal1, texture_x1, texture_y2);

                push_point(target, p2_3d, normal2, texture_x2, texture_y1);
                push_point(target, p3_3d, normal2, texture_x1, texture_y2);
                push_point(target, p4_3d, normal2, texture_x2, texture_y2);
            }
        };

        class vgen_ball {
        public:
            static std::vector<float> generate(int plane_num){
                std::vector<glm::vec2> semicircle_points;
                for(int i=0; i<plane_num; i++){
                    float theta = M_PI*i/plane_num;
                    semicircle_points.push_back(glm::vec2(cos(theta)/2, sin(theta)/2));
                }
                return vgen_revolu_surf::generate(plane_num, semicircle_points, glm::vec3(1, 0, 0));
            }
        };

        class vgen_cone {
        public:
            static std::vector<float> generate(int plane_num, float theta){
                std::vector<glm::vec2> outlines;
                outlines.push_back(glm::vec2(0, 0.5));
                outlines.push_back(glm::vec2(-0.5/std::tan(M_PI_2+theta/(M_PI*2)), -0.5));
                return vgen_revolu_surf::generate(plane_num, outlines, glm::vec3(0.0, 1.0, 0.0));
            }
        };

        class shader{
        public:
            static std::string vs_fragpos_normal_texcoord(){
                return std::string(R"(
#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;


out vec3 FragPos;
out vec3 Normal;
out vec2 TexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    FragPos = vec3(model * vec4(aPos, 1.0));
    Normal = mat3(transpose(inverse(model))) * aNormal;  
    TexCoord = aTexCoord;
    
    gl_Position = projection * view * vec4(FragPos, 1.0);

}
                )");
            }
            static std::string fs_multiple_lights_shader(uint32_t max_light_num){
                return std::string(R"(
#version 330 core

out vec4 frag_col;

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TexCoord;

uniform vec3 viewPos;

// 材质

#define MAX_LIGHT_NUM )")+
std::to_string(max_light_num)+
std::string(R"(
#define MAX_MATERIAL_NUM 4

uniform int diffuse_num;
uniform int specular_num;
struct Material {
    sampler2D diffuse[MAX_MATERIAL_NUM];
    sampler2D specular[MAX_MATERIAL_NUM];
    //TODO Emission
    float shininess;
}; 
uniform Material material;

// 定向光

struct DirLight {
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight lights_dir[MAX_LIGHT_NUM];
uniform int dir_light_num;

vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse[0], TexCoord));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse[0], TexCoord));
    vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoord));
    return (ambient + diffuse + specular);
}

// 点光源

struct PointLight {
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform PointLight lights_point[MAX_LIGHT_NUM];
uniform int point_light_num;

vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 衰减
    float distance    = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + 
                 light.quadratic * (distance * distance));    
    // 合并结果
    vec3 ambient  = light.ambient  * texture(material.diffuse[0], TexCoord).rgb;
    vec3 diffuse  = light.diffuse  * diff * texture(material.diffuse[0], TexCoord).rgb;
    vec3 specular = light.specular * spec * texture(material.specular[0], TexCoord).rgb;
    ambient  *= attenuation;
    diffuse  *= attenuation;
    specular *= attenuation;
    return (ambient + diffuse + specular);
}

// 聚光

struct SpotLight {
    vec3 position;
    vec3 direction;
    float cutoff;
    float cutoff_outer;
  
    float constant;
    float linear;
    float quadratic;
  
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;       
};
uniform SpotLight lights_spot[MAX_LIGHT_NUM];
uniform int spot_light_num;

vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 lightDir = normalize(light.position - fragPos);
    // diffuse shading
    float diff = max(dot(normal, lightDir), 0.0);
    // specular shading
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // attenuation
    float distance = length(light.position - fragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    // spotlight intensity
    float theta = dot(lightDir, normalize(-light.direction)); 
    float epsilon = light.cutoff - light.cutoff_outer;
    float intensity = clamp((theta - light.cutoff_outer) / epsilon, 0.0, 1.0);
    // combine results
    vec3 ambient = light.ambient * vec3(mix(texture(material.diffuse[0], TexCoord), texture(material.diffuse[1], TexCoord), 0.2));
    vec3 diffuse = light.diffuse * diff * vec3(mix(texture(material.diffuse[0], TexCoord), texture(material.diffuse[1], TexCoord), 0.2));
    vec3 specular = light.specular * spec * vec3(texture(material.specular[0], TexCoord));
    ambient *= attenuation * intensity;
    diffuse *= attenuation * intensity;
    specular *= attenuation * intensity;
    return (ambient + diffuse + specular);
}



void main()
{
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    // 第一阶段：定向光照
    vec3 dir_result = vec3(0, 0, 0);
    for(int i = 0; i < min(MAX_LIGHT_NUM, dir_light_num); i++)
        dir_result += CalcDirLight(lights_dir[i], norm, viewDir);
    // 第二阶段：点光源
    vec3 point_result = vec3(0, 0, 0);
    for(int i = 0; i < min(MAX_LIGHT_NUM, point_light_num); i++)
        point_result += CalcPointLight(lights_point[i], norm, FragPos, viewDir);
    // 第三阶段：聚光
    vec3 spot_result = vec3(0, 0, 0);
    for(int i = 0; i < min(MAX_LIGHT_NUM, spot_light_num); i++)
        spot_result += CalcSpotLight(lights_spot[i], norm, FragPos, viewDir);
    
    int light_num_sum = dir_light_num + point_light_num + spot_light_num;
    vec3 result = dir_result + point_result + spot_result;
    frag_col = vec4(result, 1.0);
    
} 
                )");
            }
        };
    }
}