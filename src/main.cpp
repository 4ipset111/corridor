#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "shader_sources.h"
#include "texture.h"
#include "geometry.h"
#include "camera.h"

Camera g_camera;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    g_camera.updateMouse(xpos, ypos);
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(1200, 800, "corridor", nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, 1200, 800);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
    glEnable(GL_DEPTH_TEST);

    Shader shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

    Texture brickTexture("assets/brick.png");
    Texture grassTexture("assets/grass.png");

    Geometry wallGeometry;
    FloorGeometry floorGeometry;

    std::vector<FloorAABB> floors;
    floors.push_back({-2.0f, 2.0f, -10.0f, 10.0f, -1.0f});
    g_camera.setFloors(floors);

    float pixelSize = 0.02f;

    while (!glfwWindowShouldClose(window))
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        g_camera.updateMovement(moveForward, moveBackward, moveLeft, moveRight);
        g_camera.updateGravity();
        g_camera.updateFlashlight();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = g_camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(g_camera.fov),
            1200.0f / 800.0f,
            0.1f,
            100.0f
        );

        shader.use();
        shader.setMatrix4fv("model", model);
        shader.setMatrix4fv("view", view);
        shader.setMatrix4fv("projection", projection);
        shader.setFloat("time", static_cast<float>(glfwGetTime()));
        shader.setFloat("pixelSize", pixelSize);
        shader.setVec3("viewPos", g_camera.getFlashlightPos());
        shader.setVec3("viewDir", g_camera.getFlashlightDir());

        brickTexture.bind();
        wallGeometry.render();

        grassTexture.bind();
        floorGeometry.render();

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}