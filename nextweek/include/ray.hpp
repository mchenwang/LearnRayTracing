#ifndef __RAY_H__
#define __RAY_H__

#include "algebra.hpp"

struct Ray
{
    point3d o; // 光源
    vec3d dir; // 方向
    double time;  // 时间
    Ray() = default;
    Ray(const point3d& o_, const vec3d& dir_, const double t_ = 0.) noexcept : o(o_), dir(dir_), time(t_) {}

    point3d at(const double t) const {
        return o + dir * t;
    }
};


#endif