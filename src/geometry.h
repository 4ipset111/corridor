#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <vector>

enum class RoomType {
    START,
    EMPTY_LONG_BIG,
    KEY_ROOM,
    LEVER_ROOM
};

struct Room {
    RoomType type;
    float startZ;
    float endZ;
    float width;
    float height;
    
    bool hasKey;
    glm::vec3 keyPos;
    bool keyPickedUp;

    bool hasLever;
    glm::vec3 leverPos;
    bool leverPulled;

    bool hasDoor;
    float doorOpenAngle;
    bool doorLocked;
    bool isDoorOpening;
    float doorShakeTime;
};

class Geometry
{
public:
    GLuint VAO, VBO, EBO;
    int indexCount;

    Geometry();
    ~Geometry();

    void render() const;

private:
    void createWallGeometry();
};

class FloorGeometry
{
public:
    GLuint VAO, VBO, EBO;
    int indexCount;

    FloorGeometry();
    ~FloorGeometry();

    void render() const;

private:
    void createFloorGeometry();
};

class DoorGeometry
{
public:
    GLuint VAO, VBO, EBO;
    int indexCount;

    DoorGeometry();
    ~DoorGeometry();

    void render() const;

private:
    void createDoorGeometry();
};

class CubeGeometry
{
public:
    GLuint VAO, VBO, EBO;
    int indexCount;

    CubeGeometry();
    ~CubeGeometry();

    void render() const;

private:
    void createCubeGeometry();
};

class BatchedGeometry
{
public:
    GLuint VAO, VBO, EBO;
    int indexCount;

    BatchedGeometry(const std::vector<Room>& rooms);
    ~BatchedGeometry();
    void render() const;
};

#endif // __GEOMETRY_H__
