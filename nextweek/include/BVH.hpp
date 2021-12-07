#ifndef __BVH_H__
#define __BVH_H__

#include "ray.hpp"
#include <memory>
#include <vector>
#include "hittable.hpp"

class AABB {
    point3d min_point;
    point3d max_point;
public:
    AABB() = default;
    AABB(const point3d& a, const point3d& b) noexcept;
    AABB(const AABB& temp);
    AABB& operator=(const AABB& temp);

    point3d get_min_point() const;
    point3d get_max_point() const;

    bool hit(const Ray& r, double in_t, double out_t) const;

    static AABB surrounding_box(const AABB&, const AABB&);
};

class BVH_Node : public Hittable {
    std::shared_ptr<Hittable> left;
    std::shared_ptr<Hittable> right;
    AABB box;

    static bool bbcmp(const std::shared_ptr<Hittable>&, const std::shared_ptr<Hittable>&, size_t);
public:
    BVH_Node() = default;
    BVH_Node(const std::vector<std::shared_ptr<Hittable>>&, size_t, size_t, double, double);

    bool scatter(Ray& ray_out, const hit_info& hit) const;
    bool bounding_box(const double, const double, AABB& output_box) const;

    bool hit(const Ray&, double, double, hit_info&) const;
};

#endif