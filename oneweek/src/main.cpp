#include <iostream>
#include "PPMImage.hpp"

using namespace std;

const int default_height = 256;
const int default_width = 256;
PPMImage image(default_height, default_width);

int main(int argc, char *argv[])
{
    if (argc > 1) {
        image = PPMImage(atoi(argv[1]), atoi(argv[2]));
    }
    for (int i = 0; i < image.get_height() / 2; i++) {
        for (int j = 0; j < image.get_width() / 2; j++) {
            image.set_pixel(i, j, Color(255, 0, 0));
            image.set_pixel(i + image.get_height() / 2, j, Color(0, 255, 0));
            image.set_pixel(i, j + image.get_width() / 2, Color(0, 0, 255));
            image.set_pixel(i + image.get_height() / 2, j + image.get_width() / 2, Color(0, 0, 0));
        }
    }
    image.write_to_file("image.ppm");
    return 0;
}