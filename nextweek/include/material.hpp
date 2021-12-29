#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "ray.hpp"
#include "Color.hpp"
#include "texture.hpp"
#include <memory>

using std::shared_ptr;

struct scatter_info {
    const point3d scatter_point;
    const vec3d scatter_point_nm;
    const vec3d cast_ray_dir;
    double ray_in_time;
    
    Ray scatter_ray;

    scatter_info(const point3d& p, const vec3d& nm, const vec3d& dir, const double t) noexcept
    : scatter_point(p), scatter_point_nm(nm), cast_ray_dir(dir), ray_in_time(t) {}
};

struct dielectrics_scatter_info : scatter_info {
    const double refraction_ratio;

    dielectrics_scatter_info(const point3d& p, const vec3d& nm, const vec3d& dir, const double t, const double rr) noexcept
    : scatter_info(p, nm, dir, t), refraction_ratio(rr) {}
};

class Material {
protected:
    shared_ptr<Texture> texture;
    // Color attenuation_coef;
public:
    Material() = default;
    Material(Color a_c) noexcept : texture(std::make_shared<SolidTexture>(a_c)) {}
    Material(shared_ptr<Texture> texture_) noexcept : texture(texture_) {}
    virtual bool scatter(shared_ptr<scatter_info>& info) const = 0;
    // Color get_color_attenuation_coef() const { return attenuation_coef; }
    Color get_texture(const double u, const double v, const point3d& p) const { return texture->GetTexture(u, v, p); }
    virtual Color emitted(double u, double v, const point3d& p) const { return Color(0,0,0); }
};

class Lambertian : public Material {
public:
    Lambertian(Color a_c) noexcept : Material(a_c) {}
    Lambertian(shared_ptr<Texture> texture) noexcept : Material(texture) {}
    bool scatter(shared_ptr<scatter_info>& info) const override {
        // one week 中 An Alternative Diffuse Formulation 说明正确的漫反射反射光线的方向是通过随机生成半球的方向得到，
        // 这里使用法线与随机单位球方向的向量和的方向近似，可能生成反正光线方向的概率不相同，不过个人感觉影响不大
        // vec3d diffuse_ray_dir = info->scatter_point_nm + random_unit_vector();
        vec3d diffuse_ray_dir = info->scatter_point_nm + get_random_vec3d(-1., 1.).normalize();
        // 0向量特殊处理
        if (diffuse_ray_dir.is_zero_vec()) diffuse_ray_dir = info->scatter_point_nm;
        else diffuse_ray_dir.normalized();
        info->scatter_ray = Ray(info->scatter_point, diffuse_ray_dir, info->ray_in_time);
        return true;
    }
};

class Metal : public Material {
    double fuzz;
public:
    Metal(Color a_c, double fuzz_ = 0.) noexcept : Material(a_c), fuzz(fuzz_) {}
    Metal(shared_ptr<Texture> texture, double fuzz_ = 0.) noexcept : Material(texture), fuzz(fuzz_) {}
    bool scatter(shared_ptr<scatter_info>& info) const override {
        vec3d reflect_ray_dir = info->cast_ray_dir - info->scatter_point_nm * 2 * dot(info->cast_ray_dir, info->scatter_point_nm);
        vec3d fuzzy_dir = reflect_ray_dir + get_random_vec3d(-1., 1.) * fuzz;
        info->scatter_ray = Ray(info->scatter_point, fuzzy_dir.normalize(), info->ray_in_time);
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
    bool scatter(shared_ptr<scatter_info>& tinfo) const override {

        auto info = std::static_pointer_cast<dielectrics_scatter_info>(tinfo);

        double cos_theta = fmin(dot(vec3d() - info->cast_ray_dir, info->scatter_point_nm), 1.0);
        double sin_theta = sqrt(1 - cos_theta * cos_theta);

        vec3d scatter_dir;
        if (sin_theta * info->refraction_ratio > 1. || get_reflection_coefficient(cos_theta, info->refraction_ratio) > get_random()) {
            scatter_dir = info->cast_ray_dir - info->scatter_point_nm * 2 * dot(info->cast_ray_dir, info->scatter_point_nm);
        }
        else {
            vec3d r_out_perp =  info->refraction_ratio * (info->cast_ray_dir + info->scatter_point_nm * cos_theta);
            vec3d r_out_parallel = -sqrt(fabs(1.0 - r_out_perp.length2())) * info->scatter_point_nm;
            scatter_dir = r_out_perp + r_out_parallel;
        }
        info->scatter_ray = Ray(info->scatter_point, scatter_dir.normalize(), info->ray_in_time);
        return true;
    }
};

const Dielectrics global_air(1.);

class DiffuseLight : public Material {
public:
    DiffuseLight(shared_ptr<Texture>& texture) noexcept : Material(texture) {}
    DiffuseLight(Color& color) noexcept : Material(color) {}
    bool scatter(shared_ptr<scatter_info>& info) const { return false; }
    Color emitted(double u, double v, const point3d& p) const override {
        return texture->GetTexture(u, v, p);
    }
};

#endif