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

#include <vector>
#include <glm/glm.hpp>

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
        class revolu_surf_vgenerator{
        public:
            std::vector<glm::vec2> outlines;
            glm::vec3 rotate_axis;

            /**
             * @brief Construct a new revolu surf vgenerator object
             * 
             * @param rotate_axis 旋转轴
             * @param outlines 二维轮廓点集,范围[-0.5, 0.5]
             */
            revolu_surf_vgenerator(std::vector<glm::vec2> outlines, glm::vec3 rotate_axis=glm::vec3(0.0, 0.0, 1.0)):outlines(outlines), rotate_axis(rotate_axis){}
            
            /**
            * @brief 生成顶点数组
            * 
            * @param num 微分次数
            * @return std::vector<float> 顶点数组
            */
            std::vector<float> generate(int num);
            
        private:
            glm::vec3 get_3d_point(glm::vec2 point, float degree);
            void push_point(std::vector<float> *target, glm::vec3 point, glm::vec3 normal, float texture_x, float texture_y);
            void push_surface(std::vector<float> *target, glm::vec2 p1, glm::vec2 p3, float degree, float degree_delta);
        }; 
    }
}