#include "PPMImage.hpp"

Color operator*(const Color& c, double x) { return Color(c.r * x, c.g * x, c.b * x); }
Color operator*(double x, const Color& c) { return Color(c.r * x, c.g * x, c.b * x); }
Color operator*(const Color& a, const Color& b) { return Color(a.r * b.r, a.g * b.g, a.b * b.b); }
Color operator+(const Color& a, const Color& b) { return Color(a.r + b.r, a.g + b.g, a.b + b.b); }
Color operator/(const Color& c, double t) { return c * (1./t); }

PPMImage::~PPMImage() noexcept { if (image != nullptr) delete image; image = nullptr; }
PPMImage::PPMImage(int h, int w) noexcept
: height(h), width(w) {
    image = new Color[h*w];
}
PPMImage::PPMImage(const PPMImage& other) noexcept {
    height = other.height;
    width = other.width;
    std::copy(other.image, other.image + width * height, image);
}
PPMImage& PPMImage::operator=(const PPMImage& other) noexcept {
    height = other.height;
    width = other.width;
    std::copy(other.image, other.image + width * height, image);
    return *this;
}

void PPMImage::set_pixel(const int u, const int v, const Color& c) {
    if(u < 0 || u >= width || v < 0 || v >= height) return;
    image[u + v*width] = c;
}

Color PPMImage::get_color(const int u, const int v) {
    if(u < 0 || u >= width || v < 0 || v >= height) return Color();
    return image[u + v*width];
}

int PPMImage::get_height() const { return height; }
int PPMImage::get_width() const { return width; }

void PPMImage::write_to_file(const char* file_name) {
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