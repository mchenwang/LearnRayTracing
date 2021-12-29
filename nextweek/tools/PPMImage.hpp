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
    PPMImage() noexcept;
    ~PPMImage() noexcept;
    PPMImage(int h, int w) noexcept;
    PPMImage(PPMImage&& other) noexcept;
    PPMImage& operator=(PPMImage&& other) noexcept;
    void set_pixel(const int u, const int v, const Color& c);
    Color get_color(const int u, const int v);
    int get_height() const;
    int get_width() const;
    void write_to_file(const char* file_name);
    void read_from_file(std::string file_name);
};

#endif