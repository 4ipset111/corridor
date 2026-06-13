#include "camera.h"
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <iostream>

Camera::Camera()
    : position(0.0f, FLOOR_HEIGHT + PLAYER_HEIGHT, 0.0f),
      yaw(-90.0f),
      pitch(0.0f),
      lastMouseX(0.0f),
      lastMouseY(0.0f),
      firstMouse(true),
      velocity(0.0f),
      fov(45.0f)
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

void Camera::updateMovement(bool moveForward, bool moveBackward, bool moveLeft, bool moveRight)
{
    glm::vec3 front = getFront();
    front.y = 0.0f;
    if (glm::length(front) > 0.001f) {
        front = glm::normalize(front);
    } else {
        front = glm::vec3(0.0f);
    }

    glm::vec3 right = getRight();

    if (moveForward)
        position += front * SPEED;
    if (moveBackward)
        position -= front * SPEED;

    if (moveLeft)
        position -= right * SPEED;
    if (moveRight)
        position += right * SPEED;
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

void Camera::updateGravity() {
    velocity -= GRAVITY;

    float nextY = position.y + velocity;
    float hitLevel = castRayDown();

    if (hitLevel > -999.0f && nextY <= hitLevel + PLAYER_HEIGHT) {
        position.y = hitLevel + PLAYER_HEIGHT;
        velocity = 0.0f;
    } else {
        position.y = nextY;

        if (position.y < -20.0f) {
            position = glm::vec3(0.0f, FLOOR_HEIGHT + PLAYER_HEIGHT, 0.0f);
            velocity = 0.0f;
        }
    }

    static int frameCounter = 0;
    if (frameCounter++ % 60 == 0) {
        std::cout << "Cam pos: (" << position.x << ", "
                  << position.y << ", " << position.z << ")" << std::endl;
    }
}

void Camera::updateFlashlight()
{
    flashlightPos = glm::mix(flashlightPos, position, 0.15f);
    flashlightDir = glm::mix(flashlightDir, getFront(), 0.07f);
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
    return glm::lookAt(
        position,
        position + getFront(),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
}
