#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "algebra.hpp"

const point3d default_o(0, 0, 1);
const point3d default_up_dir(0, 1, 0);
const point3d default_look_at(0, 0, 0);

struct Camera
{
    point3d o; // 相机位置
    vec3d up_dir; // 相机向上的方向
    point3d look_at; // 相机看向的点

    Camera() noexcept : o(default_o), up_dir(default_up_dir), look_at(default_look_at) { set_view_transform_matrix(); }
    Camera(const point3d& o_, const vec3d& up_dir_, const point3d& look_at_) noexcept
    : o(o_), up_dir(up_dir_), look_at(look_at_) { set_view_transform_matrix(); }

    vec3d mat[3]; // 视图变化矩阵
    void set_view_transform_matrix() {
        vec3d z = (o - look_at).normalize();
        vec3d x = cross(up_dir, z).normalize();
        vec3d y = cross(z, x).normalize();
        mat[0] = x;
        mat[1] = y;
        mat[2] = z;
    }
    vec3d get_ray_dir(double u, double v) {
        point3d screen_p(u, v, 0);
        return (point3d(dot(mat[0], screen_p), dot(mat[1], screen_p), dot(mat[2], screen_p)) - o).normalize();
    }
};


#endif