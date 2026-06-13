#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <glad/glad.h>

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

#endif // __GEOMETRY_H__
