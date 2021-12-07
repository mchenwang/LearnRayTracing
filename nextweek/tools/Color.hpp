#ifndef __COLOR_H__
#define __COLOR_H__
#include <iostream>

struct Color {
    double r, g, b;
    Color(double r_=0, double g_=0, double b_=0) noexcept
    : r(r_), g(g_), b(b_) {}

    template<typename Tstream>
    friend Tstream& operator<<(Tstream& out, const Color& c) {
        out << (int)(c.r * 255) << " " << (int)(c.g * 255) << " " << (int)(c.b * 255);
        return out;
    }
};

Color operator*(const Color& c, double x);
Color operator*(double x, const Color& c);
Color operator*(const Color& a, const Color& b);
Color operator+(const Color& a, const Color& b);
Color operator/(const Color& c, double t);

#endif