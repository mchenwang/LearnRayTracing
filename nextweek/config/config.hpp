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
    ConfigManager(const char* fileName = "config.data") noexcept {
        std::stringstream ss;
        ss << project_path << fileName;
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
                    camera = make_shared<Camera>(o, up_dir, look_at, vfov, lr, dist, t1, t2);
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
                    camera = make_shared<Camera>(o, up_dir, look_at, vfov, lr, dist, t1, t2);
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
                    std::shared_ptr<Noise> noise;
                    if (line.compare("Perlin") == 0) noise = make_shared<PerlinNoise>();
                    else if (line.compare("TriPerlin") == 0) noise = make_shared<TriPerlin>();
                    std::getline(f, line);
                    Color color = GetColor(line);
                    textures.emplace_back(make_shared<NoiseTexture>(noise, color));
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
                else if (line.compare("Sphere") == 0) {
                    std::getline(f, line);
                    point3d o = GetPoint3d(line);
                    std::getline(f, line);
                    double r = GetDouble(line);
                    std::getline(f, line);
                    int material_index = (int) GetDouble(line) -1 ;
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
                    int material_index = (int) GetDouble(line) -1 ;
                    if (material_index < 0 || material_index >= materials.size()) std::cerr << "Sphere material error!\n";
                    else {
                        auto material = materials[material_index];
                        objs.push_back(make_shared<MovingSphere>(t1, o1, t2, o2, r, material));
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