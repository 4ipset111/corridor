#include "geometry.h"
#include <vector>

Geometry::Geometry() : indexCount(0)
{
    createWallGeometry();
}

Geometry::~Geometry()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void Geometry::createWallGeometry()
{
    std::vector<float> vertices = {
        -1.0f, -1.0f, -10.0f,  0.0f,  0.0f,       1.0f, 0.0f, 0.0f,
        -1.0f,  3.0f, -10.0f,  0.0f,  4.0f,       1.0f, 0.0f, 0.0f,
        -1.0f,  3.0f,  10.0f, 20.0f,  4.0f,       1.0f, 0.0f, 0.0f,
        -1.0f, -1.0f,  10.0f, 20.0f,  0.0f,       1.0f, 0.0f, 0.0f,

         1.0f, -1.0f, -10.0f,  0.0f,  0.0f,      -1.0f, 0.0f, 0.0f,
         1.0f,  3.0f, -10.0f,  0.0f,  4.0f,      -1.0f, 0.0f, 0.0f,
         1.0f,  3.0f,  10.0f, 20.0f,  4.0f,      -1.0f, 0.0f, 0.0f,
         1.0f, -1.0f,  10.0f, 20.0f,  0.0f,      -1.0f, 0.0f, 0.0f,

        -1.0f,  3.0f, -10.0f,  0.0f,  0.0f,       0.0f, -1.0f, 0.0f,
         1.0f,  3.0f, -10.0f,  2.0f,  0.0f,       0.0f, -1.0f, 0.0f,
         1.0f,  3.0f,  10.0f,  2.0f, 20.0f,       0.0f, -1.0f, 0.0f,
        -1.0f,  3.0f,  10.0f,  0.0f, 20.0f,       0.0f, -1.0f, 0.0f,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
    };

    indexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Geometry::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

FloorGeometry::FloorGeometry() : indexCount(0)
{
    createFloorGeometry();
}

FloorGeometry::~FloorGeometry()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void FloorGeometry::createFloorGeometry()
{
    std::vector<float> vertices = {
        -2.0f, -1.0f, -10.0f,  0.0f,  0.0f,       0.0f, 1.0f, 0.0f,
         2.0f, -1.0f, -10.0f,  4.0f,  0.0f,       0.0f, 1.0f, 0.0f,
         2.0f, -1.0f,  10.0f,  4.0f, 20.0f,       0.0f, 1.0f, 0.0f,
        -2.0f, -1.0f,  10.0f,  0.0f, 20.0f,       0.0f, 1.0f, 0.0f,
    };

    std::vector<unsigned int> indices = {
        0, 2, 1, 0, 3, 2,
    };

    indexCount = static_cast<int>(indices.size());

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void FloorGeometry::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
