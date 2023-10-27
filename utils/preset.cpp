#include "preset.hpp"
#include <glm/gtx/normal.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Ez3DGL {namespace preset{
    
    const float cube_vertices_with_normals_texture [] = {
        // positions          // normals           // texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
    };

    // from http://www.ogre3d.org/tikiwiki/tiki-index.php?page=-Point+Light+Attenuation
    const float attenuation_param[] = {
    //  constant    linear      quadratic   distance
        1.0f,       0.0014,     0.000007,   // 3250
        1.0f,       0.007,      0.0002      // 600
    };
    
    std::vector<float> revolu_surf_vgenerator::generate(int num){
        std::vector<float> vertices;
        
        float delta_degree = 360.f/num;
        for(int i=0;i<outlines.size()-1;++i){
            for(int slice_i=0;slice_i<num;++slice_i){
                float degree = slice_i*delta_degree;
                auto p1_2d = outlines[i];
                auto p3_2d = outlines[i+1];
                push_surface(&vertices, p1_2d, p3_2d, degree, delta_degree);
            }
        }

        return vertices;
    }

    glm::vec3 revolu_surf_vgenerator::get_3d_point(glm::vec2 point, float degree){
        auto v = glm::vec4(point.x, point.y, 0, 1);
        return glm::rotate(glm::mat4(1.0), (float)glm::radians(degree), rotate_axis) * v;
    }

    void revolu_surf_vgenerator::push_point(std::vector<float> *target, glm::vec3 point, glm::vec3 normal, float texture_x, float texture_y){
        target->push_back(point.x);
        target->push_back(point.y);
        target->push_back(point.z);
        target->push_back(normal.x);
        target->push_back(normal.y);
        target->push_back(normal.z);
        target->push_back(texture_x);
        target->push_back(texture_y);
    }

    void revolu_surf_vgenerator::push_surface(std::vector<float> *target, glm::vec2 p1, glm::vec2 p3, float degree, float degree_delta){
        /*
        draw a plane P1P2P4P3
        P1--P2
        |  /|
        | / |
        |/  |
        P3--P4
        */

        auto p1_3d = get_3d_point(p1, degree);
        auto p2_3d = get_3d_point(p1, degree+degree_delta);
        auto p3_3d = get_3d_point(p3, degree);
        auto p4_3d = get_3d_point(p3, degree+degree_delta);

        auto texture_x1 = degree/360;
        auto texture_x2 = (degree+degree_delta)/360;
        auto texture_y1 = p1.y+0.5; // note [-0.5, 0.5]
        auto texture_y2 = p3.y+0.5;

        glm::vec3 normal1 = glm::triangleNormal(p1_3d, p2_3d, p3_3d);
        glm::vec3 normal2 = glm::triangleNormal(p2_3d, p4_3d, p3_3d);

        push_point(target, p1_3d, normal1, texture_x1, texture_y1);
        push_point(target, p2_3d, normal1, texture_x2, texture_y1);
        push_point(target, p3_3d, normal1, texture_x1, texture_y2);

        push_point(target, p2_3d, normal2, texture_x2, texture_y1);
        push_point(target, p3_3d, normal2, texture_x1, texture_y2);
        push_point(target, p4_3d, normal2, texture_x2, texture_y2);

    }


}}