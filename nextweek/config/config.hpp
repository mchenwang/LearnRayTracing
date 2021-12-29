#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "global.hpp"
#include <memory>
#include <string>
#include <sstream>
#include <vector>
#include "PPMImage.hpp"
#include "camera.hpp"
#include "ray.hpp"
#include "hittable.hpp"
#include "material.hpp"
#include "project_path.hpp"
#include "texture.hpp"

#define MUTILTHREAD
#define INIT_WORLD_WITH_CONFIG

using std::shared_ptr;
using std::make_shared;
using std::string;

class ConfigManager {
    string config_path;

    shared_ptr<Camera> camera;
    std::vector<shared_ptr<Material>> materials;
    std::vector<shared_ptr<Hittable>> objs;
    std::vector<shared_ptr<Texture>> textures;

    bool use_bvh = false;
    bool is_sample_world = false;

    double GetDouble(string& line, int& index) const {
        if (index >= line.length()) {
            std::cerr << "GetDouble error!\n";
            return 0;
        }
        double ans = 0.;
        int is_negative = 1;
        int is_frac = 0;
        if (line[index] == '-') is_negative = -1, ++index;
        while (index < line.length()) {
            if (line[index] >= '0' && line[index] <= '9') {
                if (is_frac) {
                    ans = ans + std::pow(10, -is_frac) * (line[index] - '0');
                    ++is_frac;
                }
                else ans = ans * 10 + line[index] - '0';
            }
            else if (line[index] == '.') is_frac = 1;
            else if (line[index] == '-' && is_negative == 1) is_negative = -1;
            else if (line[index] == ' ') break;
            else std::cerr << "GetDouble error!\n";
            ++index;
        }
        ++index;
        return ans * is_negative;
    }
    double GetDouble(string& line) const {
        int index = 0;
        return GetDouble(line, index);
    }

    vec3d GetVec3d(string& line) const {
        int index = 0;
        double x = GetDouble(line, index);
        double y = GetDouble(line, index);
        double z = GetDouble(line, index);
        return vec3d(x, y, z);
    }
    Color GetColor(string& line) const {
        auto temp = GetVec3d(line);
        return Color(temp.x, temp.y, temp.z);
    }
    point3d GetPoint3d(string& line) const {
        return GetVec3d(line);
    }
public:
    Color bgcolor = Color(0.7, 0.8, 1.);
    double window_ar = default_aspect_ratio;
    double window_w = default_width;
    double window_h = default_height;

    ConfigManager(const char* fileName = "config.data") noexcept {
        std::stringstream ss;
        ss << source_path << fileName;
        config_path = ss.str();
    }
    ~ConfigManager() = default;

    void GetConfig() {
        std::ifstream f;
        f.open(config_path);
        if(!f.is_open()) {
            std::cerr << config_path << " cannot be open.\n";
            f.close();
            return;
        }
        std::cout << "start read configuration.\n";

        string line;
        int type = 0;
        while (std::getline(f, line)) {
            if (line.length() > 0) {
                if (line[0] == '#') continue;
                
                if (line.compare("BVH") == 0) use_bvh = true;
                else if (line.compare("SAMPLE_WORLD") == 0) is_sample_world = true;
                else if (line.compare("Camera") == 0 && is_sample_world) {
                    std::getline(f, line);
                    point3d o = GetPoint3d(line);
                    std::getline(f, line);
                    vec3d up_dir = GetVec3d(line);
                    std::getline(f, line);
                    point3d look_at = GetPoint3d(line);
                    std::getline(f, line);
                    double vfov = GetDouble(line);
                    std::getline(f, line);
                    double lr = GetDouble(line);
                    std::getline(f, line);
                    double dist = GetDouble(line);
                    std::getline(f, line);
                    double t1 = GetDouble(line);
                    std::getline(f, line);
                    double t2 = GetDouble(line);
                    camera = make_shared<Camera>(o, up_dir, look_at, vfov, lr, dist, t1, t2, window_ar);
                }
                else if (line.compare("XCamera") == 0 && !is_sample_world) {
                    std::getline(f, line);
                    point3d o = GetPoint3d(line);
                    std::getline(f, line);
                    vec3d up_dir = GetVec3d(line);
                    std::getline(f, line);
                    point3d look_at = GetPoint3d(line);
                    std::getline(f, line);
                    double vfov = GetDouble(line);
                    std::getline(f, line);
                    double lr = GetDouble(line);
                    std::getline(f, line);
                    double dist = GetDouble(line);
                    std::getline(f, line);
                    double t1 = GetDouble(line);
                    std::getline(f, line);
                    double t2 = GetDouble(line);
                    camera = make_shared<Camera>(o, up_dir, look_at, vfov, lr, dist, t1, t2, window_ar);
                }
                else if (line.compare("aspect_ratio") == 0) {
                    std::getline(f, line);
                    window_ar = GetDouble(line);
                }
                else if (line.compare("Height") == 0) {
                    std::getline(f, line);
                    window_h = GetDouble(line);
                }
                else if (line.compare("Width") == 0) {
                    std::getline(f, line);
                    window_w = line[0] == '-' ? window_h * window_ar : GetDouble(line);
                }
                else if (line.compare("BgColor") == 0) {
                    std::getline(f, line);
                    bgcolor = GetColor(line);
                }
                else if (line.compare("Solid") == 0) {
                    std::getline(f, line);
                    Color color = GetColor(line);
                    textures.emplace_back(make_shared<SolidTexture>(color));
                }
                else if (line.compare("Checker") == 0) {
                    std::getline(f, line);
                    Color color0 = GetColor(line);
                    std::getline(f, line);
                    Color color1 = GetColor(line);
                    textures.emplace_back(make_shared<CheckerTexture>(color0, color1));
                }
                else if (line.compare("Noise") == 0) {
                    std::getline(f, line);
                    double scale = GetDouble(line);
                    std::getline(f, line);
                    Color color = GetColor(line);
                    textures.emplace_back(make_shared<NoiseTexture>(make_shared<PerlinNoise>(), color, scale));
                }
                else if (line.compare("Image") == 0) {
                    std::getline(f, line);
                    auto image = make_shared<PPMImage>();
                    image->read_from_file(line);
                    textures.emplace_back(make_shared<ImageTexture>(image));
                }
                else if (line.compare("Lambertian") == 0) {
                    std::getline(f, line);
                    int index = GetDouble(line) - 1;
                    if (index >= 0 && index < textures.size())
                        materials.push_back(make_shared<Lambertian>(textures[index]));
                }
                else if (line.compare("Dielectrics") == 0) {
                    std::getline(f, line);
                    double n = GetDouble(line);
                    materials.push_back(make_shared<Dielectrics>(n));
                }
                else if (line.compare("Metal") == 0) {
                    std::getline(f, line);
                    int index = GetDouble(line) - 1;
                    std::getline(f, line);
                    double fuzz = GetDouble(line);
                    if (index >= 0 && index < textures.size())
                        materials.push_back(make_shared<Metal>(textures[index], fuzz));
                }
                else if (line.compare("DiffuseLight") == 0) {
                    std::getline(f, line);
                    int index = GetDouble(line) - 1;
                    if (index >= 0 && index < textures.size())
                        materials.push_back(make_shared<DiffuseLight>(textures[index]));
                }
                else if (line.compare("Sphere") == 0) {
                    std::getline(f, line);
                    point3d o = GetPoint3d(line);
                    std::getline(f, line);
                    double r = GetDouble(line);
                    std::getline(f, line);
                    int material_index = (int) GetDouble(line) -1;
                    if (material_index < 0 || material_index >= materials.size()) std::cerr << "Sphere material error!\n";
                    else {
                        auto material = materials[material_index];
                        objs.push_back(make_shared<Sphere>(o, r, material));
                    }
                }
                else if (line.compare("MovingSphere") == 0) {
                    std::getline(f, line);
                    point3d o1 = GetPoint3d(line);
                    std::getline(f, line);
                    point3d o2 = GetPoint3d(line);
                    std::getline(f, line);
                    double r = GetDouble(line);
                    std::getline(f, line);
                    double t1 = GetDouble(line);
                    std::getline(f, line);
                    double t2 = GetDouble(line);
                    std::getline(f, line);
                    int material_index = (int) GetDouble(line) -1;
                    if (material_index < 0 || material_index >= materials.size()) std::cerr << "Sphere material error!\n";
                    else {
                        auto material = materials[material_index];
                        objs.push_back(make_shared<MovingSphere>(t1, o1, t2, o2, r, material));
                    }
                }
                else if (line.compare("Rect") == 0) {
                    std::getline(f, line);
                    char axis = line[0];
                    std::getline(f, line);
                    point3d p1 = GetPoint3d(line);
                    std::getline(f, line);
                    point3d p2 = GetPoint3d(line);
                    std::getline(f, line);
                    int material_index = (int) GetDouble(line) -1;
                    if (material_index < 0 || material_index >= materials.size()) std::cerr << "Rect material error!\n";
                    else {
                        auto material = materials[material_index];
                        if (axis == 'x') objs.push_back(make_shared<Rect<0>>(p1, p2, material));
                        else if (axis == 'y') objs.push_back(make_shared<Rect<1>>(p1, p2, material));
                        else if (axis == 'z') objs.push_back(make_shared<Rect<2>>(p1, p2, material));
                        // if (axis == 'x') objs.push_back(make_shared<YZRect>(p1, p2, material));
                        // else if (axis == 'y') objs.push_back(make_shared<XZRect>(p1, p2, material));
                        // else if (axis == 'z') objs.push_back(make_shared<XYRect>(p1, p2, material));
                        else std::cerr << "Rect error!\n";
                    }
                }
            }
        }

        f.close();
        std::cout << "read configuration end.\n";
    }

    shared_ptr<Camera>& GetCamera() { return camera; }
    
    std::vector<shared_ptr<Hittable>>& GetObjects() { return objs; }

    bool CheckUseBVH() const { return use_bvh; }
    bool CheckIsSampleWorld() const { return is_sample_world; }
};

#endif