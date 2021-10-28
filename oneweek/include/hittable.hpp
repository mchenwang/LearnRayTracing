﻿#ifndef __HITTABLE_H__
#define __HITTABLE_H__

#include "algebra.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include <memory>

using std::shared_ptr;

class Hittable;

struct hit_info{
    double t;
    point3d point;
    vec3d normal;
    vec3d cast_ray_dir;
    shared_ptr<Hittable> obj;
};

class Hittable {
protected:
    std::shared_ptr<Material> material;
public:
    Hittable() = default;
    Hittable(std::shared_ptr<Material> m) noexcept : material(m) {}
    virtual bool hit(const Ray& ray, double t_min, double t_max, hit_info& ret) const = 0;
    virtual bool scatter(Ray& ray_out, const hit_info& hit) const = 0;
    Color get_material_attenuation_coef() const { return material->get_color_attenuation_coef(); }
};

class Sphere : public Hittable {
    point3d o;
    double r;
public:
    Sphere() = default;
    Sphere(const point3d& o_, const double r_, std::shared_ptr<Material> m) noexcept 
    : Hittable(m), o(o_), r(r_) {}
    point3d get_origin() const { return o; }
    double get_radius() const { return r; }
    
    // ray.o + ray.dir * t == hit_point
    // (hit_point - o).length() == r
    // (ray.dir * t + (ray.o - o))^2 == r^2
    // ray.dir^2 * t^2 + 2*ray.dir*(ray.o-o) * t + ((ray.o-o)^2 - r^2) == 0
    // 当 b^2 - 4ac >=0 时，t 有解
    bool hit(const Ray& ray, double t_min, double t_max, hit_info& ret) const override {
        vec3d or = ray.o - o;
        double b = 2 * dot(ray.dir, or);
        double a = ray.dir.length2();
        double c = or.length2() - r * r;
        double delta = b * b - 4 * a * c;
        if (delta < 0.) return false;
        double t = (-b - sqrt(delta)) / (2. * a);
        if (t < t_min || t > t_max) {
            t = (-b + sqrt(delta)) / (2. * a);
            if (t < t_min || t > t_max) 
                return false;
        }
        ret.t = t;
        ret.point = ray.at(t);
        ret.normal = (ret.point - o).normalize();
        if (dot(ret.normal, ray.dir) > 0.) {
            ret.normal = vec3d() - ret.normal;
        }
        return true;
    }

    bool scatter(Ray& ray_out, const hit_info& hit) const override {
        material->scatter(hit.point, hit.normal, hit.cast_ray_dir, ray_out);
        return true;
    }
};

#endif