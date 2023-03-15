#define STB_IMAGE_WRITE_IMPLEMENTATION
#define NOMINMAX

#include <iostream>
#include <vector>
#include <limits>
#include <algorithm>
#include <cstdint>
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

void fillTriangle2(glm::vec3 v0, glm::vec3 v1, glm::vec3 v2, glm::vec3 ity0, glm::vec3 ity1, glm::vec3 ity2, glm::vec2 image_size, unsigned char* data,int* zbuffer, glm::vec3 color) {
    // sort the vertices by y-coordinate ascending so v1 is the top-most vertice
    if (v0.y > v1.y) { std::swap(v0, v1); std::swap(ity0, ity1); }
    if (v0.y > v2.y){std::swap(v0, v2); std::swap(ity0, ity1);}
    if (v1.y > v2.y){std::swap(v1, v2); std::swap(ity0, ity1);}
    v0 *= 800;
    v1 *= 800;
    v2 *= 800;
    ity0 *= 800;
    ity1 *= 800;
    ity2 *= 800;
    int total_height= v2.y - v0.y;
    //std::cout << "total_height : " << total_height << std::endl;

    for (int i = 0; i < total_height; i++) {
        bool second_half = i > v1.y - v0.y || v1.y == v0.y;
        int half_height = second_half ? v2.y - v1.y : v1.y - v0.y;
        float a = (float)i / total_height;
        float b = (float)(i - (second_half ? v1.y - v0.y : 0)) / half_height;
        glm::vec3 A = v0 + (v2 - v0) * a;
        glm::vec3 B = second_half ? v1 + (v2 - v1) * b : v0 + (v1 - v0) * b;
        glm::vec3 iA = ity0 + (ity2 - ity0) * a;
        glm::vec3 iB = second_half ? ity1 + (ity2 - ity1) * b : ity0+(ity1-ity0)*b;

        if (A.x > B.x) { std::swap(A, B); std::swap(iA, iB); }

        for (int j = A.x; j < B.x; j++) {
            float phi = B.x == A.x ? 1. : (j - A.x) / (float)(B.x - A.x);
            glm::vec3 P = A + phi * B - A;
            glm::vec3 ityP = iA + (iB - iA) * phi;
            int index = P.x + P.y * image_size.x;
            if (P.x >= image_size.x || P.y >= image_size.y || P.x < 0 || P.y < 0) continue;
            if( zbuffer[index]<P.z) { 
                zbuffer[index] = P.z;
      
                data[index] = ityP.x*255; // red channel
                data[index + 1] = ityP.x * 255; // green channel
                data[index + 2] = ityP.x * 255;// blue channel
			}

        }
    }
}

glm::mat4 viewport(int x, int y, int w, int h) {
    glm::mat4 m(1.0f);
    m[0][3] = x + w / 2.f;
    m[1][3] = y + h / 2.f;
    m[2][3] = 255 / 2.f;

    m[0][0] = w / 2.f;
    m[1][1] = h / 2.f;
    m[2][2] = 255 / 2.f;
    return m;
}

//https://learn.microsoft.com/en-us/cpp/build/walkthrough-creating-and-using-a-dynamic-link-library-cpp?view=msvc-170

int main() {

    const int width = 800;
    const int height = 800;
    glm::fvec3 LIGHT_DIR(1,-1,1);
    LIGHT_DIR = glm::normalize(LIGHT_DIR);
    glm::vec2 img_size = glm::vec2(width, height);

    int* zbuffer = new int[width * height];
    for (int i = 0; i < width * height; ++i) {
        zbuffer[i] = std::numeric_limits<int>::min();
	}


    unsigned char* data = new (std::nothrow) unsigned char[img_size.x * img_size.y * 3];
    if (data == nullptr) {
        std::cerr << "Failed to allocate memory for data." << std::endl;
        return 1;
    }
    for (int i = 0; i < width * height * 3; ++i) {
        data[i] = 0;
    }
    {// draw model
        glm::mat4 ModelView = glm::lookAt(glm::vec3(1, 1, 3), glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
        glm::mat4 identity = glm::mat4(1.0f);
        glm::mat4 Projection(identity);
        glm::mat4 Viewport = viewport(img_size.x / 8, img_size.y / 8, img_size.x * 3 / 4, img_size.y * 3 / 4); // FALSE PROBABLY
        Projection[3][2] = (glm::fvec3(1, 1, 1) / glm::normalize(glm::vec3(1, 1, 3) - glm::vec3(0, 0, 0))).y;

        std::cerr << "ModelView: " << to_string(ModelView) << std::endl;
        std::cerr << "Projection: "  << to_string(Projection) << std::endl;
        std::cerr << "Viewport: "  << to_string(Viewport) << "\n\n" << std::endl;


        glm::mat4 MVP = Viewport * Projection * ModelView;
        std::cout << to_string(MVP) << std::endl;

        const aiScene* scene = aiImportFile("media/african_head.obj", aiProcess_Triangulate);
        if (scene == nullptr || !scene->HasMeshes()) {
            std::cerr << "Failed to load model." << std::endl;
            return 1;
        }

        std::vector<glm::vec3> pos;
        const aiMesh* mesh = scene->mMeshes[0];
        std::cout << "Number of Faces: " << mesh->mNumFaces << std::endl;
        glm::mat4 bbb = {
           1, 1, 1, 1,
           1, 1, 1, 1,
           1, 1, 1, 1,
           1, 1, 1, 1
        };

        std::cout << glm::to_string(bbb) << std::endl;
        /// <summary>
        /// https://github.com/ssloy/tinyrenderer/blob/10723326bb631d081948e5346d2a64a0dd738557/main.cpp
        /// </summary>
        /// <returns></returns>
        for (unsigned int i = 0; i != mesh->mNumFaces; i++)
        {
            const aiFace& face = mesh->mFaces[i];
            glm::fvec3 screen_coord[3];
            glm::fvec3 world_coord[3];
            glm::fvec3 intensity[3];
            for (unsigned int j = 0; j != face.mNumIndices; j++)
            {
                const aiVector3D& vertex = mesh->mVertices[face.mIndices[j]];
                bbb[0][0] = vertex.x;
                bbb[1][0] = vertex.y;
                bbb[2][0] = vertex.z;
                //std::cout << glm::to_string(bbb) << std::endl;
                glm::mat4 matrix = Viewport * Projection * ModelView * bbb;
                screen_coord[j] = glm::fvec3(
                    (float)matrix[0][0] / (float)matrix[3][0],
                    (float)matrix[1][0] / (float)matrix[3][0],
                    (float)matrix[2][0] / (float)matrix[3][0]
                );
                world_coord[j] = glm::fvec3(vertex.x, vertex.y, vertex.z);
                pos.push_back(glm::vec3(vertex.x, vertex.y, vertex.z));
                const aiVector3D& n = mesh->mNormals[i, j];
                intensity[j] = glm::fvec3(n.x, n.y, n.z) * LIGHT_DIR;
                //std::cout << to_string(screen_coord[j]) << std::endl;
            }
            //std::cout << "fillTriangle2 called: " << i << std::endl;
            fillTriangle2(screen_coord[0], screen_coord[1], screen_coord[2], intensity[0], intensity[1], intensity[2], img_size, data, zbuffer, glm::vec3(std::rand() % 255, std::rand() % 255, std::rand() % 255));
        }
    }



    std::vector<glm::vec2> vertices = {
        glm::vec2(5,5),
        glm::vec2(50,50),
        glm::vec2(100,5)
    };
    
    fillTriangle(vertices[0], vertices[1], vertices[2], img_size, data);
    stbi_flip_vertically_on_write(false);
    stbi_write_tga("triangle.tga", width, height, 3, data);

    // test z buffer
    for (int i = 0; i < img_size.x; i++) {
        for (int j = 0; j < img_size.y; j++) {
            data[i] = zbuffer[i + j * width]; // red channel
            data[i + 1] = zbuffer[i + j * width]; // green channel
            data[i + 2] = zbuffer[i + j * width];// blue channel
        }
    }
    stbi_write_tga("zbuffer.tga", width, height, 3, data);

    

    delete[] data;


    return 0;
}
