#include "global.hpp"
#include <iostream>
#include <vector>
#include <memory>
#include <Windows.h>
#include "PPMImage.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "hittable.hpp"
using namespace std;

PPMImage image(default_height, default_width);
Camera camera(point3d(13,2,3), default_up_dir, default_look_at, 20, 0.02, 13.3);
vector<shared_ptr<Hittable>> objs;

bool world_hit(const Ray& ray, hit_info& hit) {
    bool hit_flag = false;
    double t_min = 0.000001;
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

#ifdef MUTILTHREAD
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
                double v = (double)(y + get_random()) / (h - 1.);
                double u = (double)(x + get_random()) / (w - 1.);
                c = c + ray_cast(camera.get_ray(u, v));
            }
            // Gamma Correction
            c = Color(std::pow(c.r/samples_per_pixel, 0.45), std::pow(c.g/samples_per_pixel, 0.45), std::pow(c.b/samples_per_pixel, 0.45));
            image.set_pixel(x, y, c);
        }
    }
    
    std::cout << "Thread " << GetCurrentThreadId() << " end\n";
    return 0L;
}
#else
void render(){
    int h = image.get_height();
    int w = image.get_width();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            Color c;
            for (int i = 0; i < samples_per_pixel; i++){
                double v = (double)(y + get_random()) / (h - 1.);
                double u = (double)(x + get_random()) / (w - 1.);
                c = c + ray_cast(camera.get_ray(u, v));
            }
            // Gamma Correction
            c = Color(std::pow(c.r/samples_per_pixel, 0.45), std::pow(c.g/samples_per_pixel, 0.45), std::pow(c.b/samples_per_pixel, 0.45));
            image.set_pixel(x, y, c);
        }
    }
}
#endif

void init_world() {
    // auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
    // auto material_center = make_shared<Lambertian>(Color(0.7, 0.3, 0.3));
    // auto material_left   = make_shared<Dielectrics>(1.5);
    // auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.9);

    // objs.push_back(make_shared<Sphere>(point3d( 0., -100.5, .0), 100.0, material_ground));
    // objs.push_back(make_shared<Sphere>(point3d( 0.,    0.0, .0),   0.5, material_center));
    // objs.push_back(make_shared<Sphere>(point3d(-1.,    0.0, .0),   0.5, material_left));
    // objs.push_back(make_shared<Sphere>(point3d(-1.,    0.0, .0),  -0.4, material_left));
    // objs.push_back(make_shared<Sphere>(point3d( 1.,    0.0, .0),   0.5, material_right));

    auto material_ground = make_shared<Lambertian>(Color(0.5, 0.5, 0.5));
    objs.push_back(make_shared<Sphere>(point3d(0, -1000, 0), 1000, material_ground));

    auto big_sphere_o1 = point3d(0, 1, 0);
    auto big_sphere_o2 = point3d(-4, 1, 0);
    auto big_sphere_o3 = point3d(4, 1, 0);

    auto material1 = make_shared<Dielectrics>(1.5);
    objs.push_back(make_shared<Sphere>(big_sphere_o1, 1, material1));
    objs.push_back(make_shared<Sphere>(big_sphere_o1, -0.9, material1));

    auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    objs.push_back(make_shared<Sphere>(big_sphere_o2, 1, material2));

    auto material3 = make_shared<Metal>(Color(1, 1, 1));
    objs.push_back(make_shared<Sphere>(big_sphere_o3, 1, material3));

    auto can_be_created = [&big_sphere_o1, &big_sphere_o2, &big_sphere_o3](const point3d& o) {
        return (big_sphere_o1 - o).length2() > 1 && (big_sphere_o2 - o).length2() > 1 && (big_sphere_o3 - o).length2() > 1;
    };

    for (int i = -11; i < 11; i++) {
        for (int j = -11; j < 11; j++) {
            point3d shpere_center(i + 0.9 * get_random(), 0.2, j + 0.9 * get_random());

            if (can_be_created(shpere_center)) {
                double choose_material = get_random();
                shared_ptr<Material> material;

                if (choose_material < 0.8) {
                    auto diffuse_color = Color(get_random(), get_random(), get_random());
                    material = make_shared<Lambertian>(diffuse_color);
                }
                else if (choose_material < 0.95) {
                    auto metal_color = Color(get_random(0.5), get_random(0.5), get_random(0.5));
                    auto fuzz = get_random() > 0.5 ? 0. : get_random(0, .5);
                    material = make_shared<Metal>(metal_color, fuzz);
                }
                else {
                    material = make_shared<Dielectrics>(1.5);
                }

                objs.push_back(make_shared<Sphere>(shpere_center, 0.2, material));
            }
        }
    }
}

#ifdef MUTILTHREAD
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
#endif

// cmake -G"Visual Studio 16 2019" -S . -Bbuild
// cmake --build build --target raytracer
int main(int argc, char *argv[])
{
    if (argc > 1) {
        image = PPMImage(atoi(argv[1]), argc > 2 ? atoi(argv[2]) : atoi(argv[1]) * aspect_ratio);
    }

    srand((unsigned)time(NULL));

    init_world();
    
    #ifdef MUTILTHREAD
    render_with_mutilthread();
    #else
    render();
    #endif

    image.write_to_file("image.ppm");
    return 0;
}