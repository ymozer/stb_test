#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include "stb_image_write.h"

// function to draw a line using Bresenham's algorithm
void drawLine(int x1, int y1, int x2, int y2, int width, int height, unsigned char* data) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        int index = (y1 * width + x1) * 3; // 3 bytes per pixel
        data[index] = 255; // red channel
        data[index + 1] = 255; // green channel
        data[index + 2] = 255; // blue channel

        if (x1 == x2 && y1 == y2) {
            break;
        }
        int e2 = 2 * err;
        if (e2 > -dy) {
            err -= dy;
            x1 += sx;
        }
        if (e2 < dx) {
            err += dx;
            y1 += sy;
        }
    }
}

int main() {
    const int width = 512;
    const int height = 512;
    unsigned char* data = new unsigned char[width * height * 3];
    for (int i = 0; i < width * height * 3; ++i) {
        data[i] = 0;
    }
    // draw a line from (100,100) to (400,400)
    drawLine(100, 100, 400, 400, width, height, data);

    // write the pixel data to a TGA file
    stbi_write_tga("line.tga", width, height, 3, data);

    std::cout << "Line saved to line.tga" << std::endl;
    delete[] data;
    return 0;
}
