#ifndef __CAMERA_H__
#define __CAMERA_H__

#include <vector>

struct FloorAABB {
    float minX, maxX, minZ, maxZ, height;
};

#include <glm/glm.hpp>

class Camera
{
public:
    glm::vec3 position;
    float yaw;
    float pitch;
    float lastMouseX;
    float lastMouseY;
    bool firstMouse;

    bool flashlightOn;
    float fov;
    float velocity;
    static constexpr float GRAVITY = 0.01f;
    static constexpr float FLOOR_HEIGHT = -1.0f;
    static constexpr float PLAYER_HEIGHT = 1.7f;

    Camera();

    void updateMouse(double xpos, double ypos);
    void updateMovement(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, float dt);
    void updateGravity(float dt);
    void updateFlashlight(float dt);

    glm::mat4 getViewMatrix() const;

    void setFloors(const std::vector<FloorAABB>& floors);

    glm::vec3 getFront() const;
    glm::vec3 getFlashlightDir() const;
    glm::vec3 getFlashlightPos() const;
    glm::vec3 getRight() const;

private:
    glm::vec3 flashlightPos;
    glm::vec3 flashlightDir;
    std::vector<FloorAABB> floors;
    float castRayDown() const;
    
    static constexpr float MOUSE_SENSITIVITY = 0.07f;
    static constexpr float SPEED = 0.05f;
};

#endif // __CAMERA_H__
