#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "global.hpp"
#include "ray.hpp"

struct Camera
{
    point3d o; // 相机位置
    vec3d up_dir; // 相机向上的方向
    point3d look_at; // 相机看向的点
    double vfov; // 视野角度，vertical field-of-view in degrees，视锥顶角的一半
    double lens_r; // 透镜的半径

    vec3d cx, cy, cz;

    Camera() noexcept : o(default_o), up_dir(default_up_dir), look_at(default_look_at), vfov(default_vfov), lens_r(1. )
    { set_view_transform_matrix((o - look_at).length()); }
    Camera(const point3d& o_, const vec3d& up_dir_, const point3d& look_at_, const double vfov_ = default_vfov,
             const double lr = 1., const double dist_to_focus = 1.) noexcept
    : o(o_), up_dir(up_dir_), look_at(look_at_), vfov(vfov_), lens_r(lr)
    { set_view_transform_matrix(dist_to_focus); }

    point3d lower_left_corner;
    vec3d horizontal;
    vec3d vertical;
    void set_view_transform_matrix(const double dist_to_focus) {
        double h = std::tan(vfov * PI / 180. / 2) * 2.;
        double w = h * aspect_ratio;

        cz = (o - look_at).normalize();
        cx = cross(up_dir, cz).normalize();
        cy = cross(cz, cx).normalize();
        vertical = cx * w * dist_to_focus;
        horizontal = cy * h * dist_to_focus;
        lower_left_corner = o - vertical / 2. - horizontal / 2. - cz * dist_to_focus;
    }
    // vec3d get_ray_dir(double u, double v) {
    //     return (mat[0] * u + mat[1] * v + mat[2] - o).normalize();
    // }

    Ray get_ray(double u, double v) {
        auto disk = (point3d(get_random(-1, 1), get_random(-1, 1), 0.)).normalize() * lens_r;
        vec3d offset = disk.x * cx + disk.y * cy;
        point3d ro = o + offset;
        return Ray(ro, (vertical * u + horizontal * v + lower_left_corner - ro).normalize());
    }
};


#endif