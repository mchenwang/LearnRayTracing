#ifndef __HITTABLE_H__
#define __HITTABLE_H__

#include "algebra.hpp"
#include "ray.hpp"
#include "material.hpp"
#include "texture.hpp"

class AABB;
class Hittable;

struct hit_info{
    double t;
    double ray_time; // 光线发出时间
    double u;
    double v;
    point3d point;
    bool inside_obj;
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
    Hittable(const Hittable& temp) noexcept { material = temp.material; }
    Hittable(Hittable&& temp) noexcept { std::swap(material, temp.material); }
    Hittable& operator=(const Hittable& temp) { material = temp.material; return *this; }
    Hittable& operator=(Hittable&& temp) { std::swap(material, temp.material); return *this; }
    virtual bool hit(const Ray& ray, double t_min, double t_max, hit_info& ret) = 0;
    virtual bool scatter(Ray& ray_out, const hit_info& hit) const = 0;
    virtual bool bounding_box(const double, const double, AABB& output_box) const = 0;
    virtual void GetUV(double&, double&, const point3d&) const {}
    // Color get_material_attenuation_coef() const { return material->get_color_attenuation_coef(); }
    virtual Color get_material_texture(const double u, const double v, const point3d& p) const { return material->get_texture(u, v, p); }
    Color get_material_emitted(const double u, const double v, const point3d& p) const { return material->emitted(u, v, p); }
};

class Sphere : public Hittable, public std::enable_shared_from_this<Sphere> {
protected:
    point3d o;
    double r;
public:
    Sphere() = default;
    Sphere(const point3d& o_, const double r_, std::shared_ptr<Material> m) noexcept 
    : Hittable(m), o(o_), r(r_) {}
    virtual point3d get_origin(const double) const { return o; }
    double get_radius() const { return r; }
    
    bool hit(const Ray&, double, double, hit_info&) override;
    bool scatter(Ray&, const hit_info&) const override;
    bool bounding_box(const double, const double, AABB&) const override;

    void GetUV(double&, double&, const point3d&) const override;
    Color get_material_texture(const double u, const double v, const point3d& p) const override;
};

class MovingSphere : public Sphere {
    vec3d o_move_dir; // 球心移动方向
    double t1, t2; // 球心移动的开始结束时间
public:
    MovingSphere() = default;
    MovingSphere(const double t1_, const point3d& o1, const double t2_, const point3d& o2,
        const double r_, std::shared_ptr<Material> m) noexcept
        : Sphere(o1, r_, m), o_move_dir(o2 - o1), t1(t1_), t2(t2_) {}
    
    point3d get_origin(const double time) const override;
    double get_radius() const { return r; }

    bool MovingSphere::bounding_box(const double, const double, AABB&) const override;
};

template<uint32_t axis>
class Rect : public Hittable, public std::enable_shared_from_this<Rect<axis>> {
    static_assert(axis == 0 || axis == 1 || axis == 2);
    point3d p1, p2;

    static uint32_t GetNextAxis(uint32_t now) {
        uint32_t next = now + 1;
        return next > 2 ? 0 : next;
    }
public:
    Rect() = default;
    Rect(point3d& p1_, point3d& p2_, std::shared_ptr<Material> m) noexcept
    : p1(p1_), p2(p2_), Hittable(m) {}
    
    bool hit(const Ray& ray, double t_min, double t_max, hit_info& ret) override {
        double t = (p1[axis] - ray.o[axis]) / ray.dir[axis];
        if (t < t_min || t > t_max) return false;
        auto p = ray.at(t);
        uint32_t t_axis = GetNextAxis(axis);
        if ((p[t_axis] < p1[t_axis] && p[t_axis] < p2[t_axis]) ||
            (p[t_axis] > p1[t_axis] && p[t_axis] > p2[t_axis]))
            return false;
        t_axis = GetNextAxis(t_axis);
        if ((p[t_axis] < p1[t_axis] && p[t_axis] < p2[t_axis]) ||
            (p[t_axis] > p1[t_axis] && p[t_axis] > p2[t_axis]))
            return false;
        ret.t = t;
        ret.point = p;
        ret.normal = vec3d(0, 0, 1);
        if (dot(ret.normal, ray.dir) > 0.) {
            ret.normal = vec3d() - ret.normal;
            ret.inside_obj = true;
        }
        else ret.inside_obj = false;
        GetUV(ret.u, ret.v, ret.point);
        ret.obj = shared_from_this();
        return true;
    }
    bool scatter(Ray& ray_out, const hit_info& hit) const override {
        shared_ptr<scatter_info> info;
        if (std::dynamic_pointer_cast<Dielectrics>(material)) {
            auto dielectrics_material = std::dynamic_pointer_cast<Dielectrics>(material);
            double refraction_ratio = global_air.get_refraction_eta() / dielectrics_material->get_refraction_eta();
            
            info = std::make_shared<dielectrics_scatter_info>(hit.point, hit.normal, hit.cast_ray_dir, hit.ray_time, refraction_ratio);
        }
        else info = std::make_shared<scatter_info>(hit.point, hit.normal, hit.cast_ray_dir, hit.ray_time);
        bool is_scatter = material->scatter(info);
        ray_out = info->scatter_ray;
        return is_scatter;
    }
    bool bounding_box(const double, const double, AABB& output_box) const override {
        auto aabb_p1 = p1;
        auto aabb_p2 = p2;
        aabb_p1[axis] -= 0.0001;
        aabb_p2[axis] += 0.0001;
        output_box = AABB(aabb_p1, aabb_p2);
        return true;
    }
    void GetUV(double& u, double& v, const point3d& p) const override {
        auto t = GetNextAxis(axis);
        double maxx = std::max(p1[t], p2[t]);
        double minx = std::min(p1[t], p2[t]);
        double w = maxx - minx;
        u = (p[t] - minx) / w;
        
        t = GetNextAxis(t);
        double maxy = std::max(p1[t], p2[t]);
        double miny = std::min(p1[t], p2[t]);
        double h = maxy - miny;
        v = (p.y - miny) / h;
    }
};
#endif