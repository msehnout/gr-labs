#ifndef MODEL_LOADER_H
#define MODEL_LOADER_H

#include <GL/glew.h>
#include <vector>
#include <glm/glm.hpp>

void loadOBJ(
    std::string path,
    std::vector<glm::vec3>& verticies,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = std::vector<unsigned int>()
);

void loadOBJWithTiny(
    std::string path,
    std::vector<glm::vec3>& verticies,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = std::vector<unsigned int>()
);

void loadVTP(
    std::string path,
    std::vector<glm::vec3>& verticies,
    std::vector<glm::vec2>& uvs,
    std::vector<glm::vec3>& normals,
    std::vector<unsigned int>& indices = std::vector<unsigned int>()
);

class Drawable
{
public:
    GLuint VAO, verticesVBO, uvsVBO, normalsVBO, elementVBO;
    unsigned int numIndices;

    Drawable(std::string path);
    ~Drawable();
    void bind();
    /* Bind VAO before calling draw */
    void draw();
};

#endif