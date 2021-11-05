#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "global.hpp"

struct Camera
{
    point3d o; // 相机位置
    vec3d up_dir; // 相机向上的方向
    point3d look_at; // 相机看向的点
    double vfov; // 视野角度，vertical field-of-view in degrees，视锥顶角的一半
    double lens_r; // 透镜的半径

    Camera() noexcept : o(default_o), up_dir(default_up_dir), look_at(default_look_at), vfov(default_vfov), lens_r(1. )
    { set_view_transform_matrix(); }
    Camera(const point3d& o_, const vec3d& up_dir_, const point3d& look_at_, const double vfov_ = default_vfov, const double lr = 1.) noexcept
    : o(o_), up_dir(up_dir_), look_at(look_at_), vfov(vfov_), lens_r(lr)
    { set_view_transform_matrix(); }

    vec3d mat[3]; // 视图变化矩阵
    void set_view_transform_matrix() {
        double h = std::tan(vfov * PI / 180. / 2) * 2.;
        double w = h * aspect_ratio;
        double dist_to_focus = (o - look_at).length();
        
        vec3d z = (o - look_at).normalize();
        vec3d x = cross(up_dir, z).normalize();
        vec3d y = cross(z, x).normalize();
        mat[0] = x * w * dist_to_focus;
        mat[1] = y * h * dist_to_focus;
        mat[2] = look_at - mat[0] / 2. - mat[1] / 2. - z * dist_to_focus;
    }
    // vec3d get_ray_dir(double u, double v) {
    //     return (mat[0] * u + mat[1] * v + mat[2] - o).normalize();
    // }

    Ray get_ray(double u, double v) {
        point3d ro = o + (point3d(get_random(-1, 1), get_random(-1, 1), 0.)).normalize() * lens_r;
        return Ray(ro, (mat[0] * u + mat[1] * v + mat[2] - ro).normalize());
    }
};


#endif