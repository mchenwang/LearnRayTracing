#ifndef __PPMIMAGE_H__
#define __PPMIMAGE_H__
#include <iostream>
#include <fstream>

struct Color {
    float r, g, b;
    Color(float r_=0, float g_=0, float b_=0) noexcept
    : r(r_), g(g_), b(b_) {}

    template<typename Tstream>
    friend Tstream& operator<<(Tstream& out, const Color& c) {
        out << (int)(c.r * 255) << " " << (int)(c.g * 255) << " " << (int)(c.b * 255);
        return out;
    }
};

Color operator*(const Color& c, float x) { return Color(c.r * x, c.g * x, c.b * x); }
Color operator*(float x, const Color& c) { return Color(c.r * x, c.g * x, c.b * x); }


class PPMImage {
private:
    int height;
    int width;
    Color* image;
public:
    PPMImage() = default;
    ~PPMImage() noexcept { if (image != nullptr) delete image; }
    PPMImage(int h, int w) noexcept
    : height(h), width(w) {
        image = new Color[h*w];
    }
    PPMImage(const PPMImage& other) noexcept {
        height = other.height;
        width = other.width;
        std::copy(other.image, other.image + width * height, image);
    }
    PPMImage& operator=(const PPMImage& other) noexcept {
        height = other.height;
        width = other.width;
        std::copy(other.image, other.image + width * height, image);
        return *this;
    }

    void set_pixel(const int u, const int v, const Color& c) {
        if(u < 0 || u >= width || v < 0 || v >= height) return;
        image[u + v*width] = c;
    }

    Color& get_color(const int u, const int v) {
        if(u < 0 || u >= width || v < 0 || v >= height) return Color();
        return image[u + v*width];
    }

    int get_height() { return height; }
    int get_width() { return width; }

    void write_to_file(const char* file_name) {
        std::ofstream f;
        f.open(file_name);
        if(!f.is_open()) {
            std::cerr << file_name << " cannot be open.\n";
            f.close();
            return;
        }
        f << "P3\n" << width << ' ' << height << "\n255\n";
        // 图片左下角为(0,0)
        for(int y = height-1; y >= 0; y--) {
            for(int x = 0; x < width; x++) {
                f << image[x + y*width] << "\n";
            }
        }
        f.close();

        std::cout << file_name <<" has been saved.\n";
    }
};

#endif