#include "hittable.hpp"
#include "BVH.hpp"

// ray.o + ray.dir * t == hit_point
// (hit_point - o).length() == r
// (ray.dir * t + (ray.o - o))^2 == r^2
// ray.dir^2 * t^2 + 2*ray.dir*(ray.o-o) * t + ((ray.o-o)^2 - r^2) == 0
// 当 b^2 - 4ac >=0 时，t 有解
bool Sphere::hit(const Ray& ray, double t_min, double t_max, hit_info& ret) const {
    vec3d or = ray.o - get_origin(ray.time);
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
    ret.normal = (ret.point - get_origin(ray.time)).normalize();
    if (dot(ret.normal, ray.dir) > 0.) {
        ret.normal = vec3d() - ret.normal;
        ret.inside_obj = true;
    }
    else ret.inside_obj = false;
    return true;
}

bool Sphere::scatter(Ray& ray_out, const hit_info& hit) const {
    shared_ptr<scatter_info> info;
    if (std::dynamic_pointer_cast<Dielectrics>(material)) {
        auto dielectrics_material = std::dynamic_pointer_cast<Dielectrics>(material);
        double refraction_ratio;
        // 从介质射向空气，当r<0时，指当前物体是空心的，与实心物体处理相反
        if ((hit.inside_obj || get_radius() < 0) && !(hit.inside_obj && get_radius() < 0)) refraction_ratio = dielectrics_material->get_refraction_eta() / global_air.get_refraction_eta();
        else refraction_ratio = global_air.get_refraction_eta() / dielectrics_material->get_refraction_eta();
        
        info = std::make_shared<dielectrics_scatter_info>(hit.point, hit.normal, hit.cast_ray_dir, hit.ray_time, refraction_ratio);
    }
    else info = std::make_shared<scatter_info>(hit.point, hit.normal, hit.cast_ray_dir, hit.ray_time);
    material->scatter(info);
    ray_out = info->scatter_ray;
    return true;
}

bool Sphere::bounding_box(const double, const double, AABB& output_box) const {
    double abs_r = r > 0 ? r : -r;
    output_box = AABB(o - vec3d(abs_r, abs_r, abs_r), o + vec3d(abs_r, abs_r, abs_r));
    // output_box = AABB(o - vec3d(r, r, r), o + vec3d(r, r, r));
    return true;
}

point3d MovingSphere::get_origin(const double time) const {
    if (std::abs(t1 - t2) < 0.00005) return o;
    return o + ((time - t1) / (t2 - t1)) * o_move_dir;
}

bool MovingSphere::bounding_box(const double time1, const double time2, AABB& output_box) const {
    auto o1 = get_origin(time1);
    auto o2 = get_origin(time2);
    AABB b1(o1 - vec3d(r, r, r), o1 + vec3d(r, r, r));
    AABB b2(o2 - vec3d(r, r, r), o2 + vec3d(r, r, r));
    output_box = AABB::surrounding_box(b1, b2);
    return true;
}