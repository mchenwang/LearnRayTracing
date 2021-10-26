#ifndef __SPHERE_H__
#define __SPHERE_H__

#include "algebra.hpp"
#include "ray.hpp"

struct Sphere {
    point3d o;
    double r;
    Sphere() = default;
    Sphere(const point3d& o_, const double r_) noexcept : o(o_), r(r_) {}
    
    // ray.o + ray.dir * t == hit_point
    // (hit_point - o).length() == r
    // (ray.dir * t + (ray.o - o))^2 == r^2
    // ray.dir^2 * t^2 + 2*ray.dir*(ray.o-o) * t + ((ray.o-o)^2 - r^2) == 0
    // 当 b^2 - 4ac >=0 时，t 有解
    double hit(const Ray& ray) {
        vec3d or = ray.o - o;
        double b = 2 * dot(ray.dir, or);
        double a = ray.dir.length2();
        double c = or.length2() - r * r;
        double delta = b * b - 4 * a * c;
        return delta < 0. ? -1 : (-b - sqrt(delta)) / (2. * a);
    }
};

#endif