#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "algebra.hpp"
#include "Color.hpp"
#include "global.hpp"

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

class CheckerTexture : public Texture {
    std::shared_ptr<Texture> odd;
    std::shared_ptr<Texture> even;
public:
    CheckerTexture(Color even_ = Color(0, 0, 0), Color odd_ = Color(1, 1, 1)) noexcept
    : even(std::make_shared<SolidTexture>(even_)), odd(std::make_shared<SolidTexture>(odd_)) {}
    CheckerTexture(std::shared_ptr<Texture> even_, std::shared_ptr<Texture> odd_) noexcept
    : even(even_), odd(odd_) {}

    Color GetTexture(const double u, const double v, const point3d& p) const override {
        // auto sines = std::abs(cos(10 * p.x)) * sin(10 * p.y) * sin(10 * p.z);
        // auto sines = sin(10 * p.x) * sin(10 * p.y) * std::abs(sin(10 * p.z));
        auto sines = sin(10 * u * 2 * PI) * sin(10 * v * 2 * PI);
        return sines < 0 ? odd->GetTexture(u, v, p) : even->GetTexture(u, v, p);
    }
};

#endif