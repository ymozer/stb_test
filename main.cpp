#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <algorithm>
#include <cstdlib> // for rand() function
#include <ctime> // for time() function
#include <stb_image_write.h>

#include <glm/gtx/string_cast.hpp> // to_string()
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate

#include "model.hpp"


// function to draw a line using Bresenham's algorithm
void drawLine(glm::vec2 a, glm::vec2 b, glm::vec2 image_size, unsigned char* data) {
    int dx = abs(b.x - a.x);
    int dy = abs(b.y - a.y);
    int sx = (a.x < b.x) ? 1 : -1;
    int sy = (a.y < b.y) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        //int index = (y1 * width + x1) * 3; // 3 bytes per pixel
        int index = ((image_size.y - 1 - a.y) * image_size.x + a.x) * 3; // 3 bytes per pixel
        if (index >= 0 && index + 2 < image_size.x * image_size.y * 3) { // check if index is within bounds
            data[index] = 255; // red channel
            data[index + 1] = 255; // green channel
            data[index + 2] = 255; // blue channel
        }
        int e2 = 2 * err;

        if (a.y == b.y) {
            err -= dy;
            a.x += sx;
        }
        else {
            if (a.x== b.x && a.y == b.y) 
                break;
            if (e2 > -dy) {
                err -= dy;
                a.x+= sx;
            }
            if (e2 < dx) {
                err += dx;
                a.y += sy;
            }
        }
    }
}

void fillTriangle(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, glm::vec2 image_size, unsigned char* data) {
    // sort the vertices by y-coordinate ascending so v1 is the top-most vertice
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);
    int total_height = v2.y - v0.y;
    for (int i = 0; i < total_height; i++) {
        bool second_half = i > v1.y - v0.y || v1.y == v0.y;
        int half_height = second_half ? v2.y - v1.y : v1.y - v0.y;
        float a = (float)i / total_height;
        float b = (float)(i - (second_half ? v1.y - v0.y : 0)) / half_height;
        glm::vec2 A = v0 + (v2 - v0) * a;
        glm::vec2 B = second_half ? v1 + (v2 - v1) * b: v0 + (v1 - v0) * b;
        if (A.x > B.x) std::swap(A,B);
        for (int j = A.x; j <= B.x; j++) {
           // set pixel using stb_image_write
            int index = ((int)(image_size[0] - 1 - (v0.y + i)) * image_size[1] + j) * 3;

            data[index] = 255;
            data[index+1] = 255;
            data[index+2] = 255;
        }
    }
}

void fillTriangle2(glm::vec2 v0, glm::vec2 v1, glm::vec2 v2, glm::vec2 image_size, unsigned char* data, glm::vec3 color) {
    // sort the vertices by y-coordinate ascending so v1 is the top-most vertice
    if (v0.y > v1.y) std::swap(v0, v1);
    if (v0.y > v2.y) std::swap(v0, v2);
    if (v1.y > v2.y) std::swap(v1, v2);
    int total_height = v2.y - v0.y;

    for (int i = 0; i < total_height; i++) {
        bool second_half = i > v1.y - v0.y || v1.y == v0.y;
        int half_height = second_half ? v2.y - v1.y : v1.y - v0.y;
        float a = (float)i / total_height;
        float b = (float)(i - (second_half ? v1.y - v0.y : 0)) / half_height;
        glm::vec2 A = v0 + (v2 - v0) * a;
        glm::vec2 B = second_half ? v1 + (v2 - v1) * b : v0 + (v1 - v0) * b;
        if (A.x > B.x) std::swap(A, B);

        for (int j = A.x; j < B.x&& j < image_size.x; j++) {

            int index = ((int)(image_size[0] - 1 - (v0.y + i)) * image_size[1] + j) * 3;

            if( index >= 0 && index + 2 < image_size.x * image_size.y * 3) { // check if index is within bounds
                data[index] = color.x; // red channel
                data[index + 1] = color.y; // green channel
                data[index + 2] = color.z;// blue channel
			}

        }
    }
}



//https://learn.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp?view=msvc-170

int main() {

    const int width = 512;
    const int height = 512;
    glm::vec2 img_size = glm::vec2(width, height);
    unsigned char* data = new (std::nothrow) unsigned char[img_size.x * img_size.y * 3];
    if (data == nullptr) {
        std::cerr << "Failed to allocate memory for data." << std::endl;
        return 1;
    }
    for (int i = 0; i < width * height * 3; ++i) {
        data[i] = 0;
    }
    ModelManager model_manager;
    model_manager.loadModel("african_head.obj");
    std::vector<float>* vData = model_manager.getVertexData();
    std::vector<uint16_t>* iData = model_manager.getIndexData();
    std::cout << "vData size: " << vData->size() << std::endl;
    std::cout << "iData size: " << iData->size() << std::endl;
    std::cout << "vData[0]: " << vData->at(0) << std::endl;
    std::cout << "vData[1]: " << vData->at(1) << std::endl;
    std::cout << "vData[2]: " << vData->at(2) << std::endl;

    std::cout << "iData[0]: " << iData->at(0) << std::endl;
    std::cout << "iData[1]: " << iData->at(1) << std::endl;
    std::cout << "iData[2]: " << iData->at(2) << std::endl;
    
    for (int i = 0; i < iData->size(); i += 3) {

        glm::vec2 v0 = glm::vec2(vData->at(iData->at(i) * 4), vData->at(iData->at(i) * 4 + 1));
        glm::vec2 v1 = glm::vec2(vData->at(iData->at(i + 1) * 4), vData->at(iData->at(i + 1) * 4 + 1));
        glm::vec2 v2 = glm::vec2(vData->at(iData->at(i + 2) * 4), vData->at(iData->at(i + 2) * 4 + 1));
        glm::vec2 pixel_v0 = glm::vec2((v0.x + 1) * img_size.x / 2, (v0.y + 1) * img_size.y / 2);
        glm::vec2 pixel_v1 = glm::vec2((v1.x + 1) * img_size.x / 2, (v1.y + 1) * img_size.y / 2);
        glm::vec2 pixel_v2 = glm::vec2((v2.x + 1) * img_size.x / 2, (v2.y + 1) * img_size.y / 2);
        fillTriangle2(pixel_v0, pixel_v1, pixel_v2, img_size, data, glm::vec3(std::rand() % 256, std::rand() % 256, std::rand() % 256));
       
    }

    std::vector<glm::vec2> vertices = {
        glm::vec2(5,5),
        glm::vec2(50,50),
        glm::vec2(100,5)
    };
    
    fillTriangle(vertices[0], vertices[1], vertices[2], img_size, data);
    stbi_flip_vertically_on_write(true);
    stbi_write_tga("triangle.tga", width, height, 3, data);
 
    delete[] data;
    return 0;
}
