#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "ray.hpp"
#include "Color.hpp"

class Material {
protected:
    Color attenuation_coef;
public:
    Material() = default;
    Material(Color a_c) noexcept : attenuation_coef(a_c) {}
    virtual bool scatter(const point3d& scatter_point, const vec3d& scatter_point_nm, const vec3d& cast_ray_dir, Ray& scatter_ray) const = 0;
    Color get_color_attenuation_coef() const { return attenuation_coef; }
};

class Lambertian : public Material {
public:
    Lambertian(Color a_c) noexcept : Material(a_c) {}
    bool scatter(const point3d& scatter_point, const vec3d& scatter_point_nm, const vec3d& cast_ray_dir, Ray& scatter_ray) const override {
        // one week 中 An Alternative Diffuse Formulation 说明正确的漫反射反射光线的方向是通过随机生成半球的方向得到，
        // 这里使用法线与随机单位球方向的向量和的方向近似，可能生成反正光线方向的概率不相同，不过个人感觉影响不大
        // vec3d diffuse_ray_dir = get_random_vec3d_in_hemisphere();
        vec3d diffuse_ray_dir = scatter_point_nm + get_random_vec3d(-1., 1.).normalize();
        // 0向量特殊处理
        if (diffuse_ray_dir.is_zero_vec()) diffuse_ray_dir = scatter_point_nm;
        else diffuse_ray_dir.normalized();
        scatter_ray = Ray(scatter_point, diffuse_ray_dir);
        return true;
    }
};

class Metal : public Material {
    double fuzz;
public:
    Metal(Color a_c, double fuzz_ = 0.) noexcept : Material(a_c), fuzz(fuzz_) {}
    bool scatter(const point3d& scatter_point, const vec3d& scatter_point_nm, const vec3d& cast_ray_dir, Ray& scatter_ray) const override {
        vec3d reflect_ray_dir = cast_ray_dir - scatter_point_nm * 2 * dot(cast_ray_dir, scatter_point_nm);
        scatter_ray = Ray(scatter_point, reflect_ray_dir + get_random_vec3d(-1., 1.) * fuzz);
        return true;
    }
};

#endif