#ifndef __GLOBAL_H__
#define __GLOBAL_H__

#include "algebra.hpp"

#define MUTILTHREAD

const point3d default_o(0, 0, 1);
const point3d default_up_dir(0, 1, 0);
const point3d default_look_at(0, 0, 0);
constexpr double default_vfov = 90;

constexpr double aspect_ratio = 16.0 / 9.0;
constexpr int default_height = 900;
constexpr int default_width = default_height * aspect_ratio;
constexpr int samples_per_pixel = 100;
constexpr int max_depth = 50;

constexpr double PI = 3.1415926535;

#endif