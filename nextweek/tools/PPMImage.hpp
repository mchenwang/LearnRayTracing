#ifndef __PPMIMAGE_H__
#define __PPMIMAGE_H__
#include <iostream>
#include <fstream>
#include "Color.hpp"

class PPMImage {
private:
    int height;
    int width;
    Color* image;
public:
    PPMImage() = default;
    ~PPMImage() noexcept;
    PPMImage(int h, int w) noexcept;
    PPMImage(const PPMImage& other) noexcept;
    PPMImage& operator=(const PPMImage& other) noexcept;
    void set_pixel(const int u, const int v, const Color& c);
    Color get_color(const int u, const int v);
    int get_height() const;
    int get_width() const;
    void write_to_file(const char* file_name);
};

#endif