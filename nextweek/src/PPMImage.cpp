#include "PPMImage.hpp"
#include "project_path.hpp"
#include <string>
#include <sstream>
#include <queue>

Color operator*(const Color& c, double x) { return Color(c.r * x, c.g * x, c.b * x); }
Color operator*(double x, const Color& c) { return Color(c.r * x, c.g * x, c.b * x); }
Color operator*(const Color& a, const Color& b) { return Color(a.r * b.r, a.g * b.g, a.b * b.b); }
Color operator+(const Color& a, const Color& b) { return Color(a.r + b.r, a.g + b.g, a.b + b.b); }
Color operator/(const Color& c, double t) { return c * (1./t); }

PPMImage::PPMImage() noexcept { height = width = -1, image = nullptr; }
PPMImage::~PPMImage() noexcept { if (image != nullptr) delete[] image; image = nullptr; }
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

static int get_int(std::string line, int& index) {
    if (index >= line.length()) {
        std::cerr << "get_int error!\n";
        return 0;
    }
    int ans = 0;
    while (index < line.length()) {
        if (line[index] >= '0' && line[index] <= '9') {
            ans = ans * 10 + line[index] - '0';
        }
        else if (line[index] == ' ') break;
        else std::cerr << "GetDouble error!\n";
        ++index;
    }
    ++index;
    return ans;
}

static std::string GetFullPath(std::string fileName) {
    std::stringstream ss;
    ss << source_path << fileName;
    return ss.str();
}

void PPMImage::read_from_file(std::string file_name) {
    std::ifstream f;
    f.open(GetFullPath(file_name));
    if(!f.is_open()) {
        std::cerr << file_name << " cannot be open.\n";
        f.close();
        return;
    }
    
    std::string line;
    int color_scale = -1;
    int pixel_i = 0;
    while (std::getline(f, line)) {
        if (line[0] == 'P') {
            if (line[1] != '3') {
                std::cerr << file_name << " cannot be read.\n";
                f.close();
                return;
            }
        }
        else if (line[0] == '#') continue;
        else {
            int index = 0;
            std::queue<int> nums;
            while(index < line.length()) nums.push(get_int(line, index));
            while(!nums.empty()) {
                if (width == -1) width = nums.front(), nums.pop();
                else if (height == -1) height = nums.front(), nums.pop();
                else if (color_scale == -1) color_scale = nums.front(), nums.pop();
                else {
                    if (image == nullptr) image = new Color[height * width];
                    double r = (double)nums.front() / color_scale; nums.pop();
                    double g = (double)nums.front() / color_scale; nums.pop();
                    double b = (double)nums.front() / color_scale; nums.pop();
                    image[pixel_i++] = Color(r, g, b);
                }
            }
        }
    }
    f.close();

    std::cout << "get texture " << file_name << ".\n";
}