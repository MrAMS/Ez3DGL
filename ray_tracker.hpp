#pragma once
#include "glm/fwd.hpp"
#include <glm/glm.hpp>

/**
 * @brief 光线追踪,待实现
 * 
 * @tparam T 
 */
template <class T>
class ray{
    using vec3 = glm::vec<3, T>;

    public:
        ray(vec3 ori, vec3 dir):ori(ori),dir(glm::normalize(dir)){}

        vec3 ori;        
        vec3 dir;
        vec3 at(T t) const{
            return ori + t * dir;
        }
};



