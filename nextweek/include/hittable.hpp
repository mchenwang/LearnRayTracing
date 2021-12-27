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

class XYRect : public Hittable, public std::enable_shared_from_this<XYRect> {
    point3d p1, p2;
public:
    XYRect() = default;
    XYRect(double x1, double y1, double x2, double y2, double z, std::shared_ptr<Material> m) noexcept
    : p1(x1, y1, z), p2(x2, y2, z), Hittable(m) {}
    XYRect(point3d& p1_, point3d& p2_, std::shared_ptr<Material> m) noexcept
    : p1(p1_), p2(p2_), Hittable(m) {}
    
    bool hit(const Ray& ray, double t_min, double t_max, hit_info& ret) override;
    bool scatter(Ray& ray_out, const hit_info& hit) const override;
    bool bounding_box(const double, const double, AABB& output_box) const override;
    void GetUV(double&, double&, const point3d&) const override;
};

#endif