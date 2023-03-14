#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>
#include <vector>
#include <algorithm>

#include <stb_image_write.h>
#include <assimp/pbrmaterial.h>
#include <assimp/postprocess.h>

#include <glm/gtx/string_cast.hpp> // to_string()
#include <glm/glm.hpp>
#include <glm/ext.hpp> // perspective, translate, rotate


// function to draw a line using Bresenham's algorithm
void drawLine(int x1, int y1, int x2, int y2, int width, int height, unsigned char* data) {
    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true) {
        //int index = (y1 * width + x1) * 3; // 3 bytes per pixel
        int index = ((height - 1 - y1) * width + x1) * 3; // 3 bytes per pixel

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

void fillTriangle(int x1, int y1, int x2, int y2, int x3, int y3, int width, int height, unsigned char* data) {
    // sort the vertices by y-coordinate ascending so v1 is the top-most vertice
    if (y1 > y2) {
		std::swap(y1, y2);
		std::swap(x1, x2);
	}
    if (y2 > y3) {
		std::swap(y2, y3);
		std::swap(x2, x3);
	}
    if (y1 > y2) {
		std::swap(y1, y2);
		std::swap(x1, x2);
	}

	// compute the slopes
	float m12 = (float)(x2 - x1) / (float)(y2 - y1);
	float m23 = (float)(x3 - x2) / (float)(y3 - y2);
	float m13 = (float)(x3 - x1) / (float)(y3 - y1);

	// draw the first half of the triangle
    for (int y = y1; y <= y2; ++y) {
		int xStart = x1 + (y - y1) * m12;
		int xEnd = x1 + (y - y1) * m13;
		drawLine(xStart, y, xEnd, y, width, height, data);
	}

	// draw the second half of the triangle
    for (int y = y2; y <= y3; ++y) {
		int xStart = x1 + (y - y1) * m13;
		int xEnd = x2 + (y - y2) * m23;
		drawLine(xStart, y, xEnd, y, width, height, data);
	}
    
}

glm::mat4 transform(glm::vec2 const& Orientation, glm::vec3 const& Translate, glm::vec3 const& Up)
{
    glm::mat4 Proj = glm::perspective(glm::radians(45.f), 1.33f, 0.1f, 10.f);
    glm::mat4 ViewTranslate = glm::translate(glm::mat4(1.f), Translate);
    glm::mat4 ViewRotateX = glm::rotate(ViewTranslate, Orientation.y, Up);
    glm::mat4 View = glm::rotate(ViewRotateX, Orientation.x, Up);
    glm::mat4 Model = glm::mat4(1.0f);
    return Proj * View * Model;
}

int main() {
    const int width = 512;
    const int height = 512;
    unsigned char* data = new unsigned char[width * height * 3];
    for (int i = 0; i < width * height * 3; ++i) {
        data[i] = 0;
    }

    drawLine(50, 10, 500, 400, width, height, data);

    fillTriangle(400, 234,30,30,259,450,width,height, data);

    // write the pixel data to a TGA file
    stbi_write_tga("triangle.tga", width, height, 3, data);
    std::cout << "Line saved to triangle.tga" << std::endl;
    std::cout <<  "_________" << std::endl;

    glm::mat4 transform_mat=transform(glm::vec2(1,2),glm::vec3(12,12,-79),glm::vec3(99,45,0) );
    std::cout << glm::to_string(transform_mat) << std::endl;
    delete[] data;
    return 0;
}
