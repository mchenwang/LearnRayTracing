#include "BVH.hpp"
#include "hittable.hpp"

AABB::AABB(const point3d& a, const point3d& b) noexcept {
    // 保证所有 max_point - min_point 向量的方向夹角是锐角，以确保 BVH 构建时排序的正确性
    min_point = a;
    max_point = b;
}

bool AABB::hit(const Ray& r, double in_t, double out_t) const {
    for (int i = 0; i < 3; i++) {
        double invD = 1. / r.dir[i];
        double t0 = (min_point[i] - r.o[i]) * invD;
        double t1 = (max_point[i] - r.o[i]) * invD;
        if (t0 > t1) std::swap(t0, t1);
        if (in_t < t0) in_t = t0;
        if (out_t > t1) out_t = t1;
        if (in_t >= out_t) return false; // 最迟进的时间比最先出的时间大，没有hit
    }
    return true;
}

AABB AABB::surrounding_box(const AABB& box1, const AABB& box2) {
    auto b1min = box1.get_min_point();
    auto b1max = box1.get_max_point();
    auto b2min = box2.get_min_point();
    auto b2max = box2.get_max_point();
    point3d bmin(std::min(b1min.x, b2min.x), std::min(b1min.y, b2min.y), std::min(b1min.z, b2min.z));
    point3d bmax(std::max(b1max.x, b2max.x), std::max(b1max.y, b2max.y), std::max(b1max.z, b2max.z));
    return AABB(bmin, bmax);
}

bool BVH_Node::scatter(Ray& ray_out, const hit_info& hit) const { return false; }
bool BVH_Node::bounding_box(const double, const double, AABB& output_box) const { output_box = box; return true; }

bool BVH_Node::bbcmp(const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b, size_t axis) {
    AABB b1, b2;
    if (!a->bounding_box(0, 0, b1) || !b->bounding_box(0, 0, b2)) std::cerr<<"BVH bounding box error\n";
    return b1.get_min_point()[axis] < b2.get_min_point()[axis];
}

bool BVH_Node::hit(const Ray& ray, double t_min, double t_max, hit_info& ret) const {
    if (!box.hit(ray, t_min, t_max)) return false;
    bool hit_left = left->hit(ray, t_min, t_max, ret);
    bool hit_right = right->hit(ray, t_min, hit_left && ret.t < t_max ? ret.t : t_max, ret);

    if (hit_left && std::dynamic_pointer_cast<Sphere>(left)) ret.obj = left;
    if (hit_right && std::dynamic_pointer_cast<Sphere>(right)) ret.obj = right;
    return hit_left || hit_right;
}

BVH_Node::BVH_Node(const std::vector<std::shared_ptr<Hittable>>& src_objects, size_t start, size_t end, double time0, double time1) {
    auto objects = src_objects;
    int axis = get_random(0, 3);
    auto cmp = [axis](const std::shared_ptr<Hittable>& a, const std::shared_ptr<Hittable>& b) { return BVH_Node::bbcmp(a, b, axis); };
    // auto cmp = (axis == 0) ? cmpx : (axis == 1 ? cmpy : cmpz);
    size_t object_span = end - start;

    if (object_span == 1) left = right = objects[start];
    else if (object_span == 2) {
        bool cmp_ret = cmp(objects[start], objects[start+1]);
        left = objects[start + !cmp_ret];
        right = objects[start + cmp_ret];
    }
    else {
        std::sort(objects.begin() + start, objects.begin() + end, cmp);

        auto mid = start + object_span / 2;
        left = std::make_shared<BVH_Node>(objects, start, mid, time0, time1);
        right = std::make_shared<BVH_Node>(objects, mid, end, time0, time1);
    }

    AABB box_left, box_right;

    if (!left->bounding_box(time0, time1, box_left) || !right->bounding_box(time0, time1, box_right))
        std::cerr << "No bounding box in bvh_node constructor.\n";

    box = AABB::surrounding_box(box_left, box_right);
}