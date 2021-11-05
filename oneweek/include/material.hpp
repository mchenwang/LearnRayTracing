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
    virtual bool scatter(const point3d& scatter_point, const vec3d& scatter_point_nm, const vec3d& cast_ray_dir, Ray& scatter_ray) const { return false; }
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
        vec3d fuzzy_dir = reflect_ray_dir + get_random_vec3d(-1., 1.) * fuzz;
        scatter_ray = Ray(scatter_point, fuzzy_dir.normalize());
        return true;
    }
};

class Dielectrics : public Material {
    double n; // 介质折射率n
    double get_reflection_coefficient(double cos_theta, double refraction_ratio) const {
        double r0 = (1. - refraction_ratio) / (1. + refraction_ratio);
        r0 = r0*r0;
        return r0 + (1 - r0) * pow((1 - cos_theta), 5);
    }
public:
    Dielectrics(double n_ = 1.) noexcept : Material(Color(1, 1, 1)), n(n_) {}

    double get_refraction_eta() const { return n; }
    bool scatter(const point3d& scatter_point, const vec3d& scatter_point_nm, const vec3d& cast_ray_dir, Ray& scatter_ray, const double refraction_ratio) const {
        double cos_theta = fmin(dot(vec3d() - cast_ray_dir, scatter_point_nm), 1.0);
        double sin_theta = sqrt(1 - cos_theta * cos_theta);

        vec3d scatter_dir;
        if (sin_theta * refraction_ratio > 1. || get_reflection_coefficient(cos_theta, refraction_ratio) > get_random()) {
            scatter_dir = cast_ray_dir - scatter_point_nm * 2 * dot(cast_ray_dir, scatter_point_nm);
        }
        else {
            vec3d r_out_perp =  refraction_ratio * (cast_ray_dir + scatter_point_nm * cos_theta);
            vec3d r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length2())) * scatter_point_nm;
            scatter_dir = r_out_perp + r_out_parallel;
        }
        scatter_ray = Ray(scatter_point, scatter_dir.normalize());
        return true;
    }
};

const Dielectrics global_air(1.);

#endif