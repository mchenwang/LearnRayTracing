#include "config.hpp"
#include "BVH.hpp"
#include <iostream>
#include "RenderThreadPool.hpp"
using namespace std;

PPMImage image(default_height, default_width);
shared_ptr<Camera> camera;
vector<shared_ptr<Hittable>> objs;
shared_ptr<BVH_Node> bvh_root;
bool use_BVH = false;

bool world_hit(const Ray& ray, hit_info& hit) {
    bool hit_flag = false;
    double t_min = 0.000001;
    double t_max = std::numeric_limits<double>::infinity();
    if (use_BVH){
        if (bvh_root->hit(ray, t_min, t_max, hit)) {
            hit.cast_ray_dir = ray.dir;
            hit.ray_time = ray.time;
            hit_flag = 1;
        }
    }
    else {
        for (auto& obj : objs) {
            if (obj->hit(ray, t_min, t_max, hit)) {
                t_max = hit.t;
                hit_flag = 1;
                hit.obj = obj;
            }
        }
        hit.cast_ray_dir = ray.dir;
        hit.ray_time = ray.time;
    }
    return hit_flag;
}

Color ray_cast(const Ray& ray, int depth = max_depth) {
    if (depth < 0) return Color();
    hit_info hit;
    if (world_hit(ray, hit)) {
        Ray scatter_ray;
        if (hit.obj->scatter(scatter_ray, hit)) {
            return hit.obj->get_material_texture(hit.u, hit.v, hit.point) * ray_cast(scatter_ray, depth - 1);
        }
    }
    double t = (ray.dir.y + 1) * 0.5;
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

#ifdef MUTILTHREAD
constexpr int thread_num = 8;
constexpr int thread_w = 2;

void render(RenderTaskParam param) {
    auto [from, to] = param;
    int h = image.get_height();
    int w = image.get_width();
    // srand(from);
    for (int y = 0; y < h; y++) {
        for (int x = from; x < to; x++) {
            Color c;
            for (int i = 0; i < samples_per_pixel; i++){
                double v = (double)(y + get_random()) / (h - 1.);
                double u = (double)(x + get_random()) / (w - 1.);
                c = c + ray_cast(camera->get_ray(u, v));
            }
            // Gamma Correction
            c = Color(std::pow(c.r/samples_per_pixel, 0.45), std::pow(c.g/samples_per_pixel, 0.45), std::pow(c.b/samples_per_pixel, 0.45));
            image.set_pixel(x, y, c);
        }
    }
    //std::cout << "render " << from << " to " << to << " end\n";
}

// 两种写法效率差不多
void render_pixel(RenderTaskParam param) {
    auto [x, y] = param;
    int h = image.get_height();
    int w = image.get_width();
    // srand(x*1000+y);
    Color c;
    for (int i = 0; i < samples_per_pixel; i++){
        double v = (double)(y + get_random()) / (h - 1.);
        double u = (double)(x + get_random()) / (w - 1.);
        c = c + ray_cast(camera->get_ray(u, v));
    }
    // Gamma Correction
    c = Color(std::pow(c.r/samples_per_pixel, 0.45), std::pow(c.g/samples_per_pixel, 0.45), std::pow(c.b/samples_per_pixel, 0.45));
    image.set_pixel(x, y, c);
}

void render_with_mutilthread() {
    RenderThreadPool pool(thread_num);
    // int from = 0, w = image.get_width();
    // int step = w / thread_num;
    // if (step > thread_w) step = thread_w;
    // while (from < w) {
    //     int to = (from + step > w ? w : from + step);
    //     pool.AddTask(render, {from, to});
    //     from = to;
    // }
    for (int x = 0; x < image.get_width(); x++) {
        for (int y = 0; y < image.get_height(); y++) {
            pool.AddTask(render_pixel, {x, y});
        }
    }
    
    DWORD t1,t2;
    t1 = GetTickCount();

    pool.Dispatch();
    pool.WaitForTaskEnding();
    
    t2 = GetTickCount();
    std::cout << "time = " << ((t2 - t1) * 1.0 / 1000) << "s" << std::endl;
}
#else
void render(){
    DWORD t1,t2;
    t1 = GetTickCount();

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
    
    t2 = GetTickCount();
    std::cout << "time = " << ((t2 - t1) * 1.0 / 1000) << "s" << std::endl;
}
#endif

void get_sample_world(ConfigManager* configManager) {
    if (configManager == nullptr) {
        camera = make_shared<Camera>(point3d(-2,0,2), default_up_dir, default_look_at, 40, 0.02, 4);
        auto material_ground = make_shared<Lambertian>(Color(0.8, 0.8, 0.0));
        auto material_center = make_shared<Lambertian>(Color(0.7, 0.3, 0.3));
        auto material_left   = make_shared<Dielectrics>(1.5);
        auto material_right  = make_shared<Metal>(Color(0.8, 0.6, 0.2), 0.9);

        objs.push_back(make_shared<Sphere>(point3d( 0., -100.5, .0), 100.0, material_ground));
        objs.push_back(make_shared<Sphere>(point3d( 0.,    0.0, .0),   0.5, material_center));
        objs.push_back(make_shared<Sphere>(point3d(-1.,    0.0, .0),   0.5, material_left));
        objs.push_back(make_shared<Sphere>(point3d(-1.,    0.0, .0),  -0.4, material_left));
        objs.push_back(make_shared<Sphere>(point3d( 1.,    0.0, .0),   0.5, material_right));
    }
    else {
        camera = configManager->GetCamera();
        objs = configManager->GetObjects();
        use_BVH = configManager->CheckUseBVH();
    }
}

void get_complex_world(ConfigManager* configManager) {
    if (configManager == nullptr) {
        camera = make_shared<Camera>(point3d(13,2,3), default_up_dir, default_look_at, 20, 0.02, 13.3, 0, 1);
    }
    else {
        use_BVH = configManager->CheckUseBVH();
        camera = configManager->GetCamera();
    }

    auto ground_texture = make_shared<CheckerTexture>(Color(0, 0, 0), Color(1, 1, 1));
    auto material_ground = make_shared<Lambertian>(ground_texture);
    objs.push_back(make_shared<Sphere>(point3d(0, -1000, 0), 1000, material_ground));

    auto big_sphere_o1 = point3d(0, 1, 0);
    auto big_sphere_o2 = point3d(-4, 1, 0);
    auto big_sphere_o3 = point3d(4, 1, 0);

    auto material1 = make_shared<Dielectrics>(1.5);
    objs.push_back(make_shared<Sphere>(big_sphere_o1, 1, material1));
    objs.push_back(make_shared<Sphere>(big_sphere_o1, -0.9, material1));

    auto texture2 = make_shared<CheckerTexture>(Color(0.2, 0.3, 0.1), Color(0.9, 0.9, 0.9));
    // auto material2 = make_shared<Lambertian>(Color(0.4, 0.2, 0.1));
    auto material2 = make_shared<Lambertian>(texture2);
    objs.push_back(make_shared<Sphere>(big_sphere_o2, 1, material2));

    auto material3 = make_shared<Metal>(Color(1, 1, 1));
    objs.push_back(make_shared<Sphere>(big_sphere_o3, 1, material3));

    auto can_be_created = [&big_sphere_o1, &big_sphere_o2, &big_sphere_o3](const point3d& o) {
        return (big_sphere_o1 - o).length2() > 1 && (big_sphere_o2 - o).length2() > 1 && (big_sphere_o3 - o).length2() > 1;
    };

    for (int i = -11; i < 11; i++) {
        for (int j = -11; j < 11; j++) {
            point3d sphere_center(i + 0.9 * get_random(), 0.2, j + 0.9 * get_random());

            if (can_be_created(sphere_center)) {
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

                double is_moving = get_random();
                if (is_moving < 0.7) {
                    point3d next_center;
                    if (is_moving < 0.35) next_center = sphere_center + vec3d(0, get_random(0, 0.4), 0);
                    else next_center = sphere_center + vec3d(0, 0, get_random(0, 0.4));
                    objs.push_back(make_shared<MovingSphere>(0, sphere_center, 1, next_center, 0.2, material));
                }
                else objs.push_back(make_shared<Sphere>(sphere_center, 0.2, material));
            }
        }
    }
}

void init_world(ConfigManager* configManager) {
    if (configManager->CheckIsSampleWorld()) get_sample_world(configManager);
    else get_complex_world(configManager);
}

// cmake -G"Visual Studio 16 2019" -S . -Bbuild
// cmake --build build --target raytracer
int main(int argc, char *argv[])
{
    //srand((unsigned)time(NULL));

    #ifdef INIT_WORLD_WITH_CONFIG
    ConfigManager* configManager = nullptr;
    
    if (argc > 1) {
        int h = atoi(argv[1]);
        image = PPMImage(h, h * aspect_ratio);
    }
    if (argc > 2) configManager = new ConfigManager(argv[2]);
    
    if (configManager == nullptr) configManager = new ConfigManager();
    configManager->GetConfig();

    init_world(configManager);

    delete configManager;
    #else
    init_world(nullptr);
    #endif

    bvh_root = make_shared<BVH_Node>(objs, 0, objs.size(), 0, 1);

    #ifdef MUTILTHREAD
    render_with_mutilthread();
    #else
    render();
    #endif

    image.write_to_file("image.ppm");
    return 0;
}