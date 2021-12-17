#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "algebra.hpp"
#include "Color.hpp"
#include "global.hpp"
#include "noise.hpp"

class Texture
{
public:
    Texture() = default;
    ~Texture() = default;
    virtual Color GetTexture(const double u, const double v, const point3d&) const = 0;
};

class SolidTexture : public Texture {
    Color texture_color;
public:
    SolidTexture(Color c = Color(0, 0, 0)) : texture_color(c) {}

    Color GetTexture(const double u, const double v, const point3d&) const override {
        return texture_color;
    }
};

#define MAP_SPHERE_TO_CUBEx
#define TEXTURE_WITH_UVx

class CheckerTexture : public Texture {
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
public:
    CheckerTexture(Color even_ = Color(0, 0, 0), Color odd_ = Color(1, 1, 1)) noexcept
    : even(std::make_shared<SolidTexture>(even_)), odd(std::make_shared<SolidTexture>(odd_)) {}
    CheckerTexture(std::shared_ptr<Texture> even_, std::shared_ptr<Texture> odd_) noexcept
    : even(even_), odd(odd_) {}

    Color GetTexture(const double u, const double v, const point3d& p) const override {
        #if defined(MAP_SPHERE_TO_CUBE)
        auto sines = sin(p.x) * sin(p.y) * sin(p.z);
        return sines < 0 ? odd->GetTexture(u, v, p) : even->GetTexture(u, v, p);
        #elif defined(TEXTURE_WITH_UV)
        // double tu = u * 10;
        // double tv = v * 10;
        // tu = tu - (int)tu;
        // tv = tv - (int)tv;
        // return (tu + tv < 0.5 || tu + tv > 1.5 || tv - tu > 0.5 || tv - tu < -0.5) ? odd->GetTexture(u, v, p) : even->GetTexture(u, v, p);
        return sin(u) * sin(v) < 0 ? odd->GetTexture(u, v, p) : even->GetTexture(u, v, p);
        #else 
        auto sines = sin(p.x * 10) * sin(p.y * 10) * sin(p.z * 10);
        return sines < 0 ? odd->GetTexture(u, v, p) : even->GetTexture(u, v, p);
        #endif
    }
};

class NoiseTexture : public Texture {
    std::shared_ptr<Noise> noise;
    Color color;
    double scale;
public:
    NoiseTexture() = delete;
    NoiseTexture(std::shared_ptr<Noise> n, Color c = Color(1, 1, 1), double s = 1.) noexcept 
    : noise(n), color(c), scale(s) {}
    Color GetTexture(const double u, const double v, const point3d& p) const override {
        return color * 0.5 * (1 + sin(scale * p.z + 10. * noise->Turb(p)));
    }
};

#endif