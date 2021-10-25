#ifndef __RAY_H__
#define __RAY_H__

#include "algebra.hpp"

struct ray
{
    point3d o; // 光源
    vec3d dir; // 方向
    ray() = default;
    ray(const point3d& o_, const vec3d& dir_) noexcept : o(o_), dir(dir_) {}

    point3d at(const double t) {
        return o + dir * t;
    }
};


#endif