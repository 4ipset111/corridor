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
float g_musicVolume = 0.5f;
float g_sfxVolume = 0.5f;
bool g_isDraggingMusic = false;
bool g_isDraggingSFX = false;
double g_mouseX = 0.0, g_mouseY = 0.0;
float g_doorShakeTime = 0.0f;
float g_doorLockedMessageTimer = 0.0f;
bool g_hasKeys = false;
bool g_isDoorOpening = false;
float g_doorOpenAngle = 0.0f;
std::string g_interactionMessage = "";

Camera g_camera;
AudioManager g_audioManager;

void mouseCallback(GLFWwindow* window, double xpos, double ypos)
{
    g_mouseX = xpos;
    g_mouseY = ypos;
    if (!g_isMenuOpen) {
        g_camera.updateMouse(xpos, ypos);
    } else if (g_isSettingsOpen) {
        if (g_isDraggingMusic) {
            g_musicVolume = glm::clamp((static_cast<float>(xpos) - 450.0f) / 300.0f, 0.0f, 1.0f);
            g_audioManager.setBackgroundMusicVolume(g_musicVolume);
        }
        if (g_isDraggingSFX) {
            g_sfxVolume = glm::clamp((static_cast<float>(xpos) - 450.0f) / 300.0f, 0.0f, 1.0f);
        }
    }
}

bool isHovering(float x, float y, float minX, float maxX, float minY, float maxY) {
    return x >= minX && x <= maxX && y >= minY && y <= maxY;
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (g_isMenuOpen && button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_RELEASE) {
            g_isDraggingMusic = false;
            g_isDraggingSFX = false;
            return;
        }

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
            if (isHovering(screenX, screenY, 450, 750, 440, 480)) { // Pixelation Checkbox
                g_pixelationEnabled = !g_pixelationEnabled;
            } else if (isHovering(screenX, screenY, 450, 750, 390, 430)) { // VHS Checkbox
                g_vhsEnabled = !g_vhsEnabled;
            } else if (isHovering(screenX, screenY, 450, 750, 340, 380)) { // Music Slider
                g_isDraggingMusic = true;
                g_musicVolume = glm::clamp((screenX - 450.0f) / 300.0f, 0.0f, 1.0f);
                g_audioManager.setBackgroundMusicVolume(g_musicVolume);
            } else if (isHovering(screenX, screenY, 450, 750, 290, 330)) { // SFX Slider
                g_isDraggingSFX = true;
                g_sfxVolume = glm::clamp((screenX - 450.0f) / 300.0f, 0.0f, 1.0f);
            } else if (isHovering(screenX, screenY, 450, 750, 240, 280)) { // Back
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
        glm::vec3 keyPos(0.75f, 1.05f, -9.45f);
        if (!g_hasKeys && glm::distance(g_camera.position, keyPos) < 1.5f) {
            glm::vec3 viewDir = g_camera.getFront();
            if (glm::dot(viewDir, glm::normalize(keyPos - g_camera.position)) > 0.8f) {
                g_hasKeys = true;
                g_audioManager.playKeySound();
                return;
            }
        }

        glm::vec3 doorPos(0.0f, 1.0f, -10.0f);
        if (glm::distance(g_camera.position, doorPos) < 2.5f) {
            glm::vec3 viewDir = g_camera.getFlashlightDir();
            if (glm::dot(viewDir, glm::normalize(doorPos - g_camera.position)) > 0.7f) {
                if (g_hasKeys) {
                    g_isDoorOpening = true;
                } else {
                    g_audioManager.playDoorLockedSound();
                    g_doorShakeTime = 0.25f;
                    g_interactionMessage = "Hmm... Seems its locked";
                    g_doorLockedMessageTimer = 3.0f;
                }
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
    GLFWmonitor* monitor = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(monitor);

    int monitorX, monitorY;
    glfwGetMonitorPos(monitor, &monitorX, &monitorY);

    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);

    int centerX = monitorX + (mode->width - windowWidth) / 2;
    int centerY = monitorY + (mode->height - windowHeight) / 2;

    glfwSetWindowPos(window, centerX, centerY);

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
    Shader postShader(POST_VERTEX_SOURCE, POST_FRAGMENT_SOURCE);

    unsigned int fbo;
    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1200, 800, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);

    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 1200, 800);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cerr << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    unsigned int quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindVertexArray(quadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    glBindVertexArray(0);

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
    DoorGeometry doorGeom;
    
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

    if (!g_audioManager.loadKeySound("assets/door_key.mp3")) {
        std::cerr << "Failed to load key sound" << std::endl;
    }

    while (glfwGetTime() < 2.0) {
        glfwPollEvents();
    }

    float pixelSize = 0.004f;
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!glfwWindowShouldClose(window))
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        std::string title = "corridor | FPS: " + std::to_string(static_cast<int>(1.0f / deltaTime));
        glfwSetWindowTitle(window, title.c_str());

        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glEnable(GL_DEPTH_TEST);
        glClearColor(0.1f, 0.1f, 0.15f, 1.0f);
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

            if (g_isDoorOpening && g_doorOpenAngle < 90.0f) {
                g_doorOpenAngle += deltaTime * 120.0f;
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
        shader.setInt("textureArray", 0);
        shader.setVec3("viewPos", g_camera.getFlashlightPos());
        shader.setFloat("flashlightOn", g_camera.flashlightOn ? 1.0f : 0.0f);
        shader.setVec3("viewDir", g_camera.getFlashlightDir());

        float doorShakeAmount = 0.0f;
        if (g_doorShakeTime > 0.0f) {
            doorShakeAmount = static_cast<float>(sin(glfwGetTime() * 100.0) * 0.02);
        }
        shader.setFloat("doorShake", doorShakeAmount);
        shader.setMatrix4fv("model", glm::mat4(1.0f));
        corridorTextures.bind(0);
        batchedScene.render();

        glm::mat4 doorModel = glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -10.0f));
        doorModel = glm::rotate(doorModel, glm::radians(-g_doorOpenAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        doorModel = glm::translate(doorModel, glm::vec3(1.0f, 0.0f, 10.0f));
        
        if (g_doorShakeTime > 0.0f) {
            doorModel = glm::translate(doorModel, glm::vec3(doorShakeAmount, 0.0f, 0.0f));
        }
        shader.setMatrix4fv("model", doorModel);
        glVertexAttrib1f(3, 2.0f);
        doorGeom.render();

        if (!g_hasKeys) {
            glm::mat4 keyTransform = glm::translate(glm::mat4(1.0f), glm::vec3(0.75f, 1.05f, -9.45f));
            keyTransform = glm::scale(keyTransform, glm::vec3(0.2f));
            shader.setMatrix4fv("model", keyTransform);
            shader.setFloat("doorShake", 0.0f);
            keyModel.render();
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        postShader.use();
        postShader.setFloat("time", static_cast<float>(glfwGetTime()));
        postShader.setFloat("pixelSizeX", pixelSize);
        postShader.setFloat("pixelSizeY", pixelSize * 1.5f);
        postShader.setFloat("pixelationOn", g_pixelationEnabled ? 1.0f : 0.0f);
        postShader.setFloat("vhsOn", g_vhsEnabled ? 1.0f : 0.0f);
        postShader.setInt("screenTexture", 0);

        glBindVertexArray(quadVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);

        if (g_doorLockedMessageTimer > 0.0f && !g_isMenuOpen) {
            glDisable(GL_DEPTH_TEST);
            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            float scale = 0.5f;
            float x = 600.0f - (textRenderer.getTextWidth(g_interactionMessage, scale) / 2.0f);
            textRenderer.renderText(textShader, g_interactionMessage, x, 100.0f, scale, glm::vec3(0.9f));

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
                std::string musicText = "MUSIC VOL: " + std::to_string(int(g_musicVolume * 100)) + "%";
                std::string sfxText   = "SFX VOL: " + std::to_string(int(g_sfxVolume * 100)) + "%";
                
                glm::vec3 pixColor = isHovering(screenX, screenY, 450, 750, 440, 480) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 vhsColor = isHovering(screenX, screenY, 450, 750, 390, 430) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 musicColor = isHovering(screenX, screenY, 450, 750, 340, 380) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 sfxColor   = isHovering(screenX, screenY, 450, 750, 290, 330) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(1.0f);
                glm::vec3 backColor = isHovering(screenX, screenY, 450, 750, 240, 280) ? glm::vec3(1.0f, 1.0f, 0.0f) : glm::vec3(0.7f);

                float pixX = centerX - (textRenderer.getTextWidth(pixText, 0.6f) / 2.0f);
                float vhsX = centerX - (textRenderer.getTextWidth(vhsText, 0.6f) / 2.0f);
                float musicX = centerX - (textRenderer.getTextWidth(musicText, 0.6f) / 2.0f);
                float sfxX   = centerX - (textRenderer.getTextWidth(sfxText, 0.6f) / 2.0f);
                float backX = centerX - (textRenderer.getTextWidth("BACK", 0.6f) / 2.0f);

                textRenderer.renderText(textShader, pixText, pixX, 450.0f, 0.6f, pixColor);
                textRenderer.renderText(textShader, vhsText, vhsX, 400.0f, 0.6f, vhsColor);
                textRenderer.renderText(textShader, musicText, musicX, 350.0f, 0.6f, musicColor);
                textRenderer.renderText(textShader, sfxText, sfxX, 300.0f, 0.6f, sfxColor);
                textRenderer.renderText(textShader, "BACK", backX, 250.0f, 0.6f, backColor);

                std::string controlsTitle = "CONTROLS";
                std::string controlsInfo = "WASD - Move | E - Interact | F - Flashlight | ESC - Menu";
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

    glDeleteFramebuffers(1, &fbo);
    glDeleteTextures(1, &textureColorbuffer);
    glDeleteRenderbuffers(1, &rbo);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteBuffers(1, &quadVBO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}