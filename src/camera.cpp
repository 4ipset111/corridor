#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>
#include "geometry.h"

extern std::vector<Room> g_rooms;

Camera::Camera()
    : position(0.0f, FLOOR_HEIGHT + PLAYER_HEIGHT, 9.7f),
      yaw(-90.0f),
      pitch(0.0f),
      lastMouseX(0.0f),
      lastMouseY(0.0f),
      firstMouse(true),
      velocity(0.0f),
      fov(45.0f),
      flashlightOn(true),
      bobTime(0.0f),
      bobOffset(0.0f)
{
    flashlightPos = position;
    flashlightDir = getFront();
}

glm::vec3 Camera::getFront() const
{
    glm::vec3 front;
    front.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    front.y = sin(glm::radians(pitch));
    front.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    return glm::normalize(front);
}

glm::vec3 Camera::getRight() const
{
    return glm::normalize(glm::cross(getFront(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void Camera::updateMouse(double xpos, double ypos)
{
    if (firstMouse)
    {
        lastMouseX = static_cast<float>(xpos);
        lastMouseY = static_cast<float>(ypos);
        firstMouse = false;
        return;
    }

    float xoffset = static_cast<float>(xpos) - lastMouseX;
    float yoffset = lastMouseY - static_cast<float>(ypos);

    lastMouseX = static_cast<float>(xpos);
    lastMouseY = static_cast<float>(ypos);

    xoffset *= MOUSE_SENSITIVITY;
    yoffset *= MOUSE_SENSITIVITY;

    yaw += xoffset;
    pitch += yoffset;

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;
}

void Camera::updateMovement(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight, bool sprint, float dt)
{
    float speedMultiplier = sprint ? 1.7f : 1.0f;
    float currentSpeed = SPEED * speedMultiplier * dt * 60.0f;

    glm::vec3 front = getFront();
    front.y = 0.0f;
    if (glm::length(front) > 0.001f) {
        front = glm::normalize(front);
    } else {
        front = glm::vec3(0.0f);
    }

    glm::vec3 right = getRight();

    if (moveForward)
        position += front * currentSpeed;
    if (moveBackward)
        position -= front * currentSpeed;

    if (moveLeft)
        position -= right * currentSpeed;
    if (moveRight)
        position += right * currentSpeed;

    float margin = 0.3f;

    int currentRoomIdx = -1;
    for (size_t i = 0; i < g_rooms.size(); ++i) {
        if (position.z <= g_rooms[i].startZ && position.z >= g_rooms[i].endZ) {
            currentRoomIdx = static_cast<int>(i);
            break;
        }
    }

    if (currentRoomIdx != -1) {
        const auto& room = g_rooms[currentRoomIdx];

        if (position.x < -room.width + margin)
            position.x = -room.width + margin;
        if (position.x > room.width - margin)
            position.x = room.width - margin;

        if (currentRoomIdx == 0) {
            if (position.z > room.startZ - margin)
                position.z = room.startZ - margin;
        } else {
            if (position.z > room.startZ - margin) {
                if (position.x < -1.0f + margin || position.x > 1.0f - margin) {
                    position.z = room.startZ - margin;
                }
            }
        }

        bool canPass = (room.doorOpenAngle >= 89.0f);
        if (!canPass) {
            if (position.z < room.endZ + margin) {
                position.z = room.endZ + margin;
            }
        } else {
            if (position.z < room.endZ + margin) {
                if (position.x < -1.0f + margin || position.x > 1.0f - margin) {
                    position.z = room.endZ + margin;
                }
            }
        }
    } else {
        if (position.z > 10.0f - margin) position.z = 10.0f - margin;
        if (!g_rooms.empty()) {
            float minZ = g_rooms.back().endZ;
            if (position.z < minZ + margin) position.z = minZ + margin;
        }
    }

    bool isMoving = moveForward || moveBackward || moveLeft || moveRight;
    if (isMoving) {
        float frequency = sprint ? 14.0f : 8.0f;
        float amplitude = sprint ? 0.08f : 0.04f;
        bobTime += dt * frequency;
        bobOffset = sin(bobTime) * amplitude;
    } else {
        bobOffset = glm::mix(bobOffset, 0.0f, 0.1f * dt * 60.0f);
        if (std::abs(bobOffset) < 0.001f) {
            bobOffset = 0.0f;
            bobTime = 0.0f;
        }
    }
}

void Camera::setFloors(const std::vector<FloorAABB>& newFloors) {
    floors = newFloors;
}

float Camera::castRayDown() const {
    float hitLevel = -1000.0f;

    for (const auto& floor : floors) {
        if (position.x >= floor.minX && position.x <= floor.maxX &&
            position.z >= floor.minZ && position.z <= floor.maxZ)
        {
            if (position.y >= floor.height) {
                hitLevel = std::max(hitLevel, floor.height);
            }
        }
    }

    return hitLevel;
}

void Camera::updateGravity(float dt) {
    velocity -= GRAVITY * dt * 60.0f;

    float nextY = position.y + velocity * dt * 60.0f;
    float hitLevel = castRayDown();

    if (hitLevel > -999.0f && nextY <= hitLevel + PLAYER_HEIGHT) {
        position.y = hitLevel + PLAYER_HEIGHT;
        velocity = 0.0f;
    } else {
        position.y = nextY;

        if (position.y < -20.0f) {
            position = glm::vec3(0.0f, FLOOR_HEIGHT + PLAYER_HEIGHT, 9.7f);
            velocity = 0.0f;
        }
    }

    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0) {
        std::cout << "Cam pos: (" << position.x << ", "
                  << position.y << ", " << position.z << ")" << std::endl;
    }
}

void Camera::updateFlashlight(float dt)
{
    float posLerp = 1.0f - pow(1.0f - 0.15f, dt * 60.0f);
    float dirLerp = 1.0f - pow(1.0f - 0.07f, dt * 60.0f);

    flashlightPos = glm::mix(flashlightPos, position, posLerp);
    flashlightDir = glm::mix(flashlightDir, getFront(), dirLerp);
    flashlightDir = glm::normalize(flashlightDir);
}

glm::vec3 Camera::getFlashlightDir() const
{
    return flashlightDir;
}

glm::vec3 Camera::getFlashlightPos() const
{
    return flashlightPos;
}

glm::mat4 Camera::getViewMatrix() const
{
    glm::vec3 bobbedPos = position;
    bobbedPos.y += bobOffset;
    return glm::lookAt(
        bobbedPos,
        bobbedPos + getFront(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}
