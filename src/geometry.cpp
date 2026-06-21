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

        -1.0f, -1.0f,  10.0f,  0.0f,  0.0f,       0.0f,  0.0f, -1.0f,
         1.0f, -1.0f,  10.0f,  2.0f,  0.0f,       0.0f,  0.0f, -1.0f,
         1.0f,  3.0f,  10.0f,  2.0f,  4.0f,       0.0f,  0.0f, -1.0f,
        -1.0f,  3.0f,  10.0f,  0.0f,  4.0f,       0.0f,  0.0f, -1.0f,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 13, 14, 12, 14, 15,
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

static void addQuad(std::vector<float>& vertices, std::vector<unsigned int>& indices,
                    const glm::vec3& p0, const glm::vec2& uv0,
                    const glm::vec3& p1, const glm::vec2& uv1,
                    const glm::vec3& p2, const glm::vec2& uv2,
                    const glm::vec3& p3, const glm::vec2& uv3,
                    const glm::vec3& normal, float texID)
{
    unsigned int baseIndex = static_cast<unsigned int>(vertices.size() / 9);

    vertices.push_back(p0.x); vertices.push_back(p0.y); vertices.push_back(p0.z);
    vertices.push_back(uv0.x); vertices.push_back(uv0.y);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
    vertices.push_back(texID);

    vertices.push_back(p1.x); vertices.push_back(p1.y); vertices.push_back(p1.z);
    vertices.push_back(uv1.x); vertices.push_back(uv1.y);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
    vertices.push_back(texID);

    vertices.push_back(p2.x); vertices.push_back(p2.y); vertices.push_back(p2.z);
    vertices.push_back(uv2.x); vertices.push_back(uv2.y);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
    vertices.push_back(texID);

    vertices.push_back(p3.x); vertices.push_back(p3.y); vertices.push_back(p3.z);
    vertices.push_back(uv3.x); vertices.push_back(uv3.y);
    vertices.push_back(normal.x); vertices.push_back(normal.y); vertices.push_back(normal.z);
    vertices.push_back(texID);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 1);
    indices.push_back(baseIndex + 2);

    indices.push_back(baseIndex);
    indices.push_back(baseIndex + 2);
    indices.push_back(baseIndex + 3);
}

BatchedGeometry::BatchedGeometry(const std::vector<Room>& rooms)
{
    std::vector<float> vertices;
    std::vector<unsigned int> indices;

    for (size_t i = 0; i < rooms.size(); ++i) {
        const auto& room = rooms[i];
        float w = room.width;
        float h = room.height;
        float zStart = room.startZ;
        float zEnd = room.endZ;
        float length = zStart - zEnd;

        addQuad(vertices, indices,
                glm::vec3(-w, -1.0f, zEnd),   glm::vec2(0.0f, 0.0f),
                glm::vec3(w, -1.0f, zEnd),    glm::vec2(2.0f * w, 0.0f),
                glm::vec3(w, -1.0f, zStart),  glm::vec2(2.0f * w, length),
                glm::vec3(-w, -1.0f, zStart), glm::vec2(0.0f, length),
                glm::vec3(0.0f, 1.0f, 0.0f),  1.0f);

        addQuad(vertices, indices,
                glm::vec3(-w, h, zStart),  glm::vec2(0.0f, 0.0f),
                glm::vec3(w, h, zStart),   glm::vec2(2.0f * w, 0.0f),
                glm::vec3(w, h, zEnd),     glm::vec2(2.0f * w, length),
                glm::vec3(-w, h, zEnd),    glm::vec2(0.0f, length),
                glm::vec3(0.0f, -1.0f, 0.0f), 0.0f);

        addQuad(vertices, indices,
                glm::vec3(-w, -1.0f, zStart), glm::vec2(0.0f, 0.0f),
                glm::vec3(-w, h, zStart),      glm::vec2(0.0f, h + 1.0f),
                glm::vec3(-w, h, zEnd),        glm::vec2(length, h + 1.0f),
                glm::vec3(-w, -1.0f, zEnd),   glm::vec2(length, 0.0f),
                glm::vec3(1.0f, 0.0f, 0.0f),   0.0f);

        addQuad(vertices, indices,
                glm::vec3(w, -1.0f, zEnd),   glm::vec2(0.0f, 0.0f),
                glm::vec3(w, h, zEnd),       glm::vec2(0.0f, h + 1.0f),
                glm::vec3(w, h, zStart),     glm::vec2(length, h + 1.0f),
                glm::vec3(w, -1.0f, zStart), glm::vec2(length, 0.0f),
                glm::vec3(-1.0f, 0.0f, 0.0f),  0.0f);

        if (i == 0) {
            addQuad(vertices, indices,
                    glm::vec3(-w, -1.0f, zStart), glm::vec2(0.0f, 0.0f),
                    glm::vec3(-w, h, zStart),      glm::vec2(0.0f, h + 1.0f),
                    glm::vec3(w, h, zStart),       glm::vec2(2.0f * w, h + 1.0f),
                    glm::vec3(w, -1.0f, zStart),  glm::vec2(2.0f * w, 0.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f),  0.0f);
        }

        if (i < rooms.size() - 1) {
            float nextW = rooms[i+1].width;
            float nextH = rooms[i+1].height;
            float maxW = std::max(w, nextW);
            float maxH = std::max(h, nextH);

            addQuad(vertices, indices,
                    glm::vec3(-1.0f, -1.0f, zEnd), glm::vec2(0.0f, 0.0f),
                    glm::vec3(-maxW, -1.0f, zEnd), glm::vec2(maxW - 1.0f, 0.0f),
                    glm::vec3(-maxW, maxH, zEnd),  glm::vec2(maxW - 1.0f, maxH + 1.0f),
                    glm::vec3(-1.0f, maxH, zEnd),  glm::vec2(0.0f, maxH + 1.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f),   0.0f);
            addQuad(vertices, indices,
                    glm::vec3(maxW, -1.0f, zEnd),  glm::vec2(0.0f, 0.0f),
                    glm::vec3(1.0f, -1.0f, zEnd),  glm::vec2(maxW - 1.0f, 0.0f),
                    glm::vec3(1.0f, maxH, zEnd),   glm::vec2(maxW - 1.0f, maxH + 1.0f),
                    glm::vec3(maxW, maxH, zEnd),   glm::vec2(0.0f, maxH + 1.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f),   0.0f);
            addQuad(vertices, indices,
                    glm::vec3(1.0f, 2.0f, zEnd),   glm::vec2(0.0f, 0.0f),
                    glm::vec3(-1.0f, 2.0f, zEnd),  glm::vec2(2.0f, 0.0f),
                    glm::vec3(-1.0f, maxH, zEnd),  glm::vec2(2.0f, maxH - 2.0f),
                    glm::vec3(1.0f, maxH, zEnd),   glm::vec2(0.0f, maxH - 2.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f),   0.0f);

            addQuad(vertices, indices,
                    glm::vec3(-maxW, -1.0f, zEnd), glm::vec2(0.0f, 0.0f),
                    glm::vec3(-1.0f, -1.0f, zEnd), glm::vec2(maxW - 1.0f, 0.0f),
                    glm::vec3(-1.0f, maxH, zEnd),  glm::vec2(maxW - 1.0f, maxH + 1.0f),
                    glm::vec3(-maxW, maxH, zEnd),  glm::vec2(0.0f, maxH + 1.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f),  0.0f);
            addQuad(vertices, indices,
                    glm::vec3(1.0f, -1.0f, zEnd),  glm::vec2(0.0f, 0.0f),
                    glm::vec3(maxW, -1.0f, zEnd),  glm::vec2(maxW - 1.0f, 0.0f),
                    glm::vec3(maxW, maxH, zEnd),   glm::vec2(maxW - 1.0f, maxH + 1.0f),
                    glm::vec3(1.0f, maxH, zEnd),   glm::vec2(0.0f, maxH + 1.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f),  0.0f);
            addQuad(vertices, indices,
                    glm::vec3(-1.0f, 2.0f, zEnd),  glm::vec2(0.0f, 0.0f),
                    glm::vec3(1.0f, 2.0f, zEnd),   glm::vec2(2.0f, 0.0f),
                    glm::vec3(1.0f, maxH, zEnd),   glm::vec2(2.0f, maxH - 2.0f),
                    glm::vec3(-1.0f, maxH, zEnd),  glm::vec2(0.0f, maxH - 2.0f),
                    glm::vec3(0.0f, 0.0f, -1.0f),  0.0f);
        } else {
            addQuad(vertices, indices,
                    glm::vec3(w, -1.0f, zEnd),    glm::vec2(0.0f, 0.0f),
                    glm::vec3(-w, -1.0f, zEnd),   glm::vec2(2.0f * w, 0.0f),
                    glm::vec3(-w, h, zEnd),       glm::vec2(2.0f * w, h + 1.0f),
                    glm::vec3(w, h, zEnd),        glm::vec2(0.0f, h + 1.0f),
                    glm::vec3(0.0f, 0.0f, 1.0f),   0.0f);
        }
    }

    indexCount = static_cast<int>(indices.size());
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(5 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindVertexArray(0);
}

BatchedGeometry::~BatchedGeometry() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void BatchedGeometry::render() const {
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

CubeGeometry::CubeGeometry() : indexCount(0)
{
    createCubeGeometry();
}

CubeGeometry::~CubeGeometry()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void CubeGeometry::createCubeGeometry()
{
    std::vector<float> vertices = {
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,    0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    0.0f,  0.0f,  1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,    0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   -1.0f,  0.0f,  0.0f,

         0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f, 1.0f,    1.0f,  0.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 0.0f,    0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 0.0f,    0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,    0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,    0.0f,  1.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,    0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,    0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 1.0f,    0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 1.0f,    0.0f, -1.0f,  0.0f,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
        4, 6, 5, 4, 7, 6,
        8, 9, 10, 8, 10, 11,
        12, 14, 13, 12, 15, 14,
        16, 18, 17, 16, 19, 18,
        20, 21, 22, 20, 22, 23
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

void CubeGeometry::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}

DoorGeometry::DoorGeometry() : indexCount(0)
{
    createDoorGeometry();
}

DoorGeometry::~DoorGeometry()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}

void DoorGeometry::createDoorGeometry()
{
    std::vector<float> vertices = {
        -1.0f, -1.0f, -10.0f,  0.0f, 0.0f,   0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, -10.0f,  1.0f, 0.0f,   0.0f, 0.0f, 1.0f,
         1.0f,  3.0f, -10.0f,  1.0f, 1.0f,   0.0f, 0.0f, 1.0f,
        -1.0f,  3.0f, -10.0f,  0.0f, 1.0f,   0.0f, 0.0f, 1.0f,
    };

    std::vector<unsigned int> indices = {
        0, 1, 2, 0, 2, 3,
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
    glBindVertexArray(0);
}

void DoorGeometry::render() const
{
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);
}
