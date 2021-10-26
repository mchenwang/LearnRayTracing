#include <iostream>
#include "PPMImage.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "sphere.hpp"
using namespace std;

constexpr double aspect_ratio = 16.0 / 9.0;
constexpr int default_height = 900;
constexpr int default_width = default_height * aspect_ratio;;
PPMImage image(default_height, default_width);
Camera camera;

Sphere sphere(point3d(0,0,0), 0.5);

Color ray_cast(const Ray& r) {
    double t = sphere.hit(r);
    if (t >= 0.) {
        point3d p = r.at(t);
        vec3d nm = (p - sphere.o).normalize();
        return Color((nm.x + 1) * 0.5, (nm.y + 1) * 0.5, (nm.z + 1) * 0.5);
    }
    t = (r.dir.y + 1) * 0.5;
    return (1.0-t)*Color(1.0, 1.0, 1.0) + t*Color(0.5, 0.7, 1.0);
}

void render() {
    int h = image.get_height();
    int w = image.get_width();
    for (int y = 0; y < h; y++) {
        for (int x = 0; x < w; x++) {
            double v = (double)(y - h * 0.5) / (h * 0.5);
            double u = (double)(x - w * 0.5) / (w * 0.5) * aspect_ratio;
            image.set_pixel(x, y, ray_cast(Ray(camera.o, camera.get_ray_dir(u, v))));
        }
    }
}

// cmake -G"Visual Studio 16 2019" -S . -Bbuild
// cmake --build build --target raytracer
int main(int argc, char *argv[])
{
    if (argc > 1) {
        image = PPMImage(atoi(argv[1]), argc > 2 ? atoi(argv[2]) : atoi(argv[1]) * aspect_ratio);
    }
    
    render();
    
    image.write_to_file("image.ppm");
    return 0;
}