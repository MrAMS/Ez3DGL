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
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>
#include <glm/gtc/type_ptr.hpp>

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
    }
}