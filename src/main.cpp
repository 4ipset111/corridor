#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>
#include <map>

#include <ft2build.h>
#include FT_FREETYPE_H

#include "shader.h"
#include "shader_sources.h"
#include "texture.h"
#include "geometry.h"
#include "audio_manager.h"
#include "camera.h"
#include "text_renderer.h"
#include "model.h"

bool g_isMenuOpen = false;
bool g_isSettingsOpen = false;
bool g_pixelationEnabled = true;
bool g_vhsEnabled = true;
double g_mouseX = 0.0, g_mouseY = 0.0;
float g_doorShakeTime = 0.0f;
float g_doorLockedMessageTimer = 0.0f;

Camera g_camera;
AudioManager g_audioManager;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    g_mouseX = xpos;
    g_mouseY = ypos;
    if (!g_isMenuOpen)
        g_camera.updateMouse(xpos, ypos);
}

bool isHovering(float x, float y, float minX, float maxX, float minY, float maxY) {
    return x >= minX && x <= maxX && y >= minY && y <= maxY;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_isMenuOpen && button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        float screenY = 800.0f - static_cast<float>(g_mouseY);
        float screenX = static_cast<float>(g_mouseX);

        if (!g_isSettingsOpen) {
            if (isHovering(screenX, screenY, 450, 750, 440, 480)) { // Continue
                g_isMenuOpen = false;
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                g_camera.firstMouse = true;
            } else if (isHovering(screenX, screenY, 450, 750, 390, 430)) { // Settings
                g_isSettingsOpen = true;
            } else if (isHovering(screenX, screenY, 450, 750, 340, 380)) { // Exit
                glfwSetWindowShouldClose(window, true);
            }
        } else {
            if (isHovering(screenX, screenY, 450, 750, 440, 480)) { // Pixelation Toggle
                g_pixelationEnabled = !g_pixelationEnabled;
            } else if (isHovering(screenX, screenY, 450, 750, 390, 430)) { // VHS Toggle
                g_vhsEnabled = !g_vhsEnabled;
            } else if (isHovering(screenX, screenY, 450, 750, 340, 380)) { // Back
                g_isSettingsOpen = false;
            }
        }
    }
}

void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_F && action == GLFW_PRESS)
    {
        g_camera.flashlightOn = !g_camera.flashlightOn;
        g_audioManager.playFlashlightSound();
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS && !g_isMenuOpen)
    {
        glm::vec3 doorPos(0.0f, 1.0f, -10.0f);
        if (glm::distance(g_camera.position, doorPos) < 2.5f) {
            glm::vec3 viewDir = g_camera.getFlashlightDir();
            if (glm::dot(viewDir, glm::normalize(doorPos - g_camera.position)) > 0.7f) {
                g_audioManager.playDoorLockedSound();
                g_doorShakeTime = 0.25f;
                g_doorLockedMessageTimer = 3.0f;
            }
        }
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
    {
        g_isMenuOpen = !g_isMenuOpen;
        g_isSettingsOpen = false;
        if (g_isMenuOpen) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        } else {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
            g_camera.firstMouse = true;
        }
    }
}

int main()
{
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    if (!g_audioManager.init()) {
        std::cerr << "Failed to initialize FMOD Audio Manager" << std::endl;
        glfwTerminate();
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
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseButtonCallback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to load OpenGL functions" << std::endl;
        return -1;
    }

    std::cout << "OpenGL Version: " << glGetString(GL_VERSION) << std::endl;

    glViewport(0, 0, 1200, 800);
    glClearColor(0.1f, 0.1f, 0.15f, 1.0f);

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);

    glEnable(GL_DEPTH_TEST);

    Shader shader(VERTEX_SHADER_SOURCE, FRAGMENT_SHADER_SOURCE);

    std::vector<std::string> texturePaths = {
        "assets/brick.png",
        "assets/grass.png",
        "assets/door.png",
        "assets/key_model/key_001.png",
        "assets/key_model/key_002.png",
        "assets/key_model/key_003.png",
        "assets/key_model/ring.png"
    };
    TextureArray corridorTextures(texturePaths);

    BatchedGeometry batchedScene;
    
    Model keyModel("assets/key_model/door_keys.obj", texturePaths);

    TextRenderer textRenderer;
    textRenderer.load("C:/Windows/Fonts/arial.ttf", 48);

    TextRenderer titleRenderer;
    titleRenderer.load("assets/menufont.ttf", 64);

    Shader textShader(TEXT_VERTEX_SOURCE, TEXT_FRAGMENT_SOURCE);
    glm::mat4 projectionText = glm::ortho(0.0f, 1200.0f, 0.0f, 800.0f);
    textShader.use();
    textShader.setMatrix4fv("projection", projectionText);

    std::vector<FloorAABB> floors;
    floors.push_back({-1.0f, 1.0f, -10.0f, 10.0f, -1.0f});
    g_camera.setFloors(floors);

    if (!g_audioManager.loadBackgroundMusic("assets/bgsong.mp3")) {
        std::cerr << "Failed to load background music" << std::endl;
    } else {
        g_audioManager.playBackgroundMusic();
    }

    if (!g_audioManager.loadFlashlightSound("assets/flashlight.mp3")) {
        std::cerr << "Failed to load flashlight sound" << std::endl;
    }

    if (!g_audioManager.loadDoorLockedSound("assets/closed_door.mp3")) {
        std::cerr << "Failed to load door sound" << std::endl;
    }

    while (glfwGetTime() < 2.0) {
        glfwPollEvents();
    }

    float pixelSize = 0.02f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::string title = "corridor | FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime));
        glfwSetWindowTitle(window, title.c_str());

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        bool moveForward = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS;
        bool moveBackward = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS;
        bool moveLeft = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS;
        bool moveRight = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS || glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS;

        if (!g_isMenuOpen) {
            g_camera.updateMovement(moveForward, moveBackward, moveLeft, moveRight, deltaTime);
            g_camera.updateGravity(deltaTime);
            g_camera.updateFlashlight(deltaTime);
            
            if (g_doorShakeTime > 0.0f) {
                g_doorShakeTime -= deltaTime;
            }

            if (g_doorLockedMessageTimer > 0.0f) {
                g_doorLockedMessageTimer -= deltaTime;
            }
        }
        
        g_audioManager.update();

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = g_camera.getViewMatrix();
        glm::mat4 projection = glm::perspective(
            glm::radians(g_camera.fov),
            1200.0f / 800.0f,
            0.1f,
            100.0f
        );

        shader.use();
        shader.setMatrix4fv("view", view);
        shader.setMatrix4fv("projection", projection);
        shader.setFloat("time", static_cast<float>(glfwGetTime()));
        shader.setFloat("pixelSize", pixelSize);
        shader.setInt("textureArray", 0);
        shader.setVec3("viewPos", g_camera.getFlashlightPos());
        shader.setFloat("flashlightOn", g_camera.flashlightOn ? 1.0f : 0.0f);
        shader.setFloat("pixelationOn", g_pixelationEnabled ? 1.0f : 0.0f);
        shader.setFloat("vhsOn", g_vhsEnabled ? 1.0f : 0.0f);
        shader.setVec3("viewDir", g_camera.getFlashlightDir());

        float doorShakeAmount = 0.0f;
        if (g_doorShakeTime > 0.0f) {
            doorShakeAmount = static_cast<float>(sin(glfwGetTime() * 100.0) * 0.02);
        }
        shader.setFloat("doorShake", doorShakeAmount);
        shader.setMatrix4fv("model", glm::mat4(1.0f));
        corridorTextures.bind(0);
        batchedScene.render();

        glm::mat4 keyTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.75f, 1.05f, -9.45f));
        keyTransform = glm::scale(keyTransform, glm::vec3(0.2f));
        shader.setMatrix4fv("model", keyTransform);
        shader.setFloat("doorShake", 0.0f);
        keyModel.render();

        if (g_doorLockedMessageTimer > 0.0f && !g_isMenuOpen) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            std::string msg = "Hmm... Seems its locked";
            float scale = 0.5f;
            float x = 600.0f - (textRenderer.getTextWidth(msg, scale) / 2.0f);
            textRenderer.renderText(textShader, msg, x, 100.0f, scale, glm::vec3(0.9f));

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }

        if (g_isMenuOpen) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            float screenY = 800.0f - static_cast<float>(g_mouseY);
            float screenX = static_cast<float>(g_mouseX);

            float centerX = 600.0f;

            std::string headerText = "Pause";

            if (!g_isSettingsOpen) {
                headerText = "Pause";
            }
            else {
                headerText = "Settings";
            }
            float headerX = centerX - (titleRenderer.getTextWidth(headerText, 1.0f) / 2.0f);
            titleRenderer.renderText(textShader, headerText, headerX, 550.0f, 1.0f, glm::vec3(1.0f));

            if (!g_isSettingsOpen) {
                glm::vec3 continueColor = isHovering(screenX, screenY, 450, 750, 440, 480) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 settingsColor = isHovering(screenX, screenY, 450, 750, 390, 430) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 exitColor     = isHovering(screenX, screenY, 450, 750, 340, 380) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f, 0.3f, 0.3f);
                
                float contX = centerX - (textRenderer.getTextWidth("CONTINUE", 0.7f) / 2.0f);
                float settX = centerX - (textRenderer.getTextWidth("SETTINGS", 0.7f) / 2.0f);
                float exitX = centerX - (textRenderer.getTextWidth("EXIT", 0.7f) / 2.0f);

                textRenderer.renderText(textShader, "CONTINUE", contX, 450.0f, 0.7f, continueColor);
                textRenderer.renderText(textShader, "SETTINGS", settX, 400.0f, 0.7f, settingsColor);
                textRenderer.renderText(textShader, "EXIT", exitX, 350.0f, 0.7f, exitColor);
            } else {
                std::string pixText = (g_pixelationEnabled ? "[X] PIXELATION" : "[ ] PIXELATION");
                std::string vhsText = (g_vhsEnabled ? "[X] VHS EFFECT" : "[ ] VHS EFFECT");
                
                glm::vec3 pixColor = isHovering(screenX, screenY, 450, 750, 440, 480) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 vhsColor = isHovering(screenX, screenY, 450, 750, 390, 430) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 backColor = isHovering(screenX, screenY, 450, 750, 340, 380) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.7f);

                float pixX = centerX - (textRenderer.getTextWidth(pixText, 0.6f) / 2.0f);
                float vhsX = centerX - (textRenderer.getTextWidth(vhsText, 0.6f) / 2.0f);
                float backX = centerX - (textRenderer.getTextWidth("BACK", 0.6f) / 2.0f);

                textRenderer.renderText(textShader, pixText, pixX, 450.0f, 0.6f, pixColor);
                textRenderer.renderText(textShader, vhsText, vhsX, 400.0f, 0.6f, vhsColor);
                textRenderer.renderText(textShader, "BACK", backX, 350.0f, 0.6f, backColor);

                std::string controlsTitle = "CONTROLS";
                std::string controlsInfo = "WASD - Move | F - Flashlight | ESC - Menu";
                float titleX = centerX - (textRenderer.getTextWidth(controlsTitle, 0.5f) / 2.0f);
                float infoX = centerX - (textRenderer.getTextWidth(controlsInfo, 0.4f) / 2.0f);
                
                textRenderer.renderText(textShader, controlsTitle, titleX, 150.0f, 0.5f, glm::vec3(0.8f));
                textRenderer.renderText(textShader, controlsInfo, infoX, 110.0f, 0.4f, glm::vec3(0.6f));
            }

            glDisable(GL_BLEND);
            glEnable(GL_DEPTH_TEST);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}