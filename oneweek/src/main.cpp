#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>
#include "PPMImage.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "hittable.hpp"
using namespace std;

constexpr double aspect_ratio = 16.0 / 9.0;
constexpr int default_height = 900;
constexpr int default_width = default_height * aspect_ratio;
constexpr int samples_per_pixel = 100;
constexpr int max_depth = 50;
PPMImage image(default_height, default_width);
Camera camera;
vector<shared_ptr<Hittable>> objs;

bool world_hit(const Ray& ray, hit_info& hit) {
    bool hit_flag = false;
    double t_min = 0.0001;
    double t_max = std::numeric_limits<double>::infinity();

    for (auto& obj : objs) {
        if (obj->hit(ray, t_min, t_max, hit)) {
            t_max = hit.t;
            hit_flag = 1;
            hit.obj = obj;
        }
    }
    hit.cast_ray_dir = ray.dir;
    return hit_flag;
}

Color ray_cast(const Ray& ray, int depth = max_depth) {
    if (depth < 0) return Color();
    hit_info hit;
    if (world_hit(ray, hit)) {
        Ray scatter_ray;
        if (hit.obj->scatter(scatter_ray, hit)) {
            return hit.obj->get_material_attenuation_coef() * ray_cast(scatter_ray, depth - 1);
        }
    }
    double t = (ray.dir.y + 1) * 0.5;
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

struct RenderThreadData { int i_from, i_to; };
DWORD WINAPI render(LPVOID range_) {
    int h = image.get_height();
    int w = image.get_width();
    RenderThreadData* range = (RenderThreadData*)range_;
    srand(range->i_from);
    for (int y = 0; y < h; y++) {
        for (int x = range->i_from; x < range->i_to; x++) {
            Color c;
            for (int i = 0; i < samples_per_pixel; i++){
                double v = (double)(y + get_random() - h * 0.5) / (h * 0.5);
                double u = (double)(x + get_random() - w * 0.5) / (w * 0.5) * aspect_ratio;
                c = c + ray_cast(Ray(camera.o, camera.get_ray_dir(u, v)));
            }
            // Gamma Correction
            c = Color(std::pow(c.r/samples_per_pixel, 0.45), std::pow(c.g/samples_per_pixel, 0.45), std::pow(c.b/samples_per_pixel, 0.45));
            image.set_pixel(x, y, c);
        }
    }
    
    std::cout << "Thread " << GetCurrentThreadId() << " end\n";
    return 0L;
}

void init_world() {
    auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    auto material_center = make_shared<Lambertian>(Color(0.7, 0.3, 0.3));
    auto material_left   = make_shared<Metal>(Color(0.8, 0.8, 0.8), 0.1);
    auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.9);

    objs.push_back(make_shared<Sphere>(point3d( 0., -100.5, .0), 100.0, material_ground));
    objs.push_back(make_shared<Sphere>(point3d( 0.,    0.0, .0),   0.5, material_center));
    objs.push_back(make_shared<Sphere>(point3d(-1.,    0.0, .0),   0.5, material_left));
    objs.push_back(make_shared<Sphere>(point3d( 1.,    0.0, .0),   0.5, material_right));

    // objs.push_back(make_shared<Sphere>(point3d(), 0.5));
    // objs.push_back(make_shared<Sphere>(point3d(0, -100.5, 0), 100));
}

void render_with_mutilthread() {
    constexpr int thread_num = 8;
    HANDLE* thread_list = new HANDLE[thread_num];
    std::vector<RenderThreadData> thread_para(thread_num);

    for(int i = 0, from = 0; i < thread_num; i++) {
        int to = (i == thread_num - 1 ? image.get_width() : from + image.get_width()/thread_num);
        thread_para[i] = {from, to};
        thread_list[i] = CreateThread(NULL, 0, render, &thread_para[i], 0, NULL);
        from = to;
    }
    WaitForMultipleObjects(thread_num, thread_list, TRUE, INFINITE);

    delete[] thread_list;
}

// cmake -G"Visual Studio 16 2019" -S . -Bbuild
// cmake --build build --target raytracer
int main(int argc, char *argv[])
{
    if (argc > 1) {
        image = PPMImage(atoi(argv[1]), argc > 2 ? atoi(argv[2]) : atoi(argv[1]) * aspect_ratio);
    }

    init_world();
    render_with_mutilthread();
    
    image.write_to_file("image.ppm");
    return 0;
}