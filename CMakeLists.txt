cmake_minimum_required(VERSION 3.10)
project(corridor)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.4
)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
)

FetchContent_MakeAvailable(glfw glm)

set(SOURCE_FILES
    src/main.cpp
    src/shader.cpp
    src/texture.cpp
    src/geometry.cpp
    src/camera.cpp
)

set(EXTERNAL_LIBS
    external/glad/include/glad/glad.h
    external/glad/include/KHR/khrplatform.h
    external/glad/src/glad.c
    external/stb/stb_image.h
)

add_executable(corridor ${SOURCE_FILES} ${EXTERNAL_LIBS})

target_include_directories(corridor PRIVATE
    src
    external/glad/include
    external/stb
)
target_link_libraries(corridor glfw glm::glm)