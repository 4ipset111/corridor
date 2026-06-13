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

set(GLFW_BUILD_DLL ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glm)

set(FMOD_SDK_PATH "C:/Program Files (x86)/FMOD SoundSystem/FMOD Studio API Windows")

find_library(FMOD_CORE_LIBRARY
    NAMES fmod fmod_vc
    PATHS
        "${FMOD_SDK_PATH}/api/core/lib"
        "${FMOD_SDK_PATH}/api/core/lib/x64"
        "${FMOD_SDK_PATH}/api/core/lib/win/x86_64"
        "${FMOD_SDK_PATH}/api/core/lib/linux/x86_64"
        "${FMOD_SDK_PATH}/api/core/lib/mac"
    NO_DEFAULT_PATH
)

find_library(FMOD_STUDIO_LIBRARY
    NAMES fmodstudio fmodstudio_vc
    PATHS
        "${FMOD_SDK_PATH}/api/studio/lib"
        "${FMOD_SDK_PATH}/api/studio/lib/x64"
        "${FMOD_SDK_PATH}/api/studio/lib/win/x86_64"
        "${FMOD_SDK_PATH}/api/studio/lib/linux/x86_64"
        "${FMOD_SDK_PATH}/api/studio/lib/mac"
    NO_DEFAULT_PATH
)

set(SOURCE_FILES
    src/main.cpp
    src/shader.cpp
    src/texture.cpp
    src/geometry.cpp
    src/audio_manager.cpp
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
    "${FMOD_SDK_PATH}/api/core/inc"
    "${FMOD_SDK_PATH}/api/studio/inc"
)

target_link_libraries(corridor PRIVATE glfw glm::glm
    ${FMOD_CORE_LIBRARY}
    ${FMOD_STUDIO_LIBRARY}
)

if(WIN32)
    file(GLOB FMOD_CORE_DLLS
        "${FMOD_SDK_PATH}/api/core/lib/x64/*.dll"
    )

    file(GLOB FMOD_STUDIO_DLLS
        "${FMOD_SDK_PATH}/api/studio/lib/x64/*.dll"
    )

    add_custom_command(TARGET corridor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FMOD_CORE_DLLS}
            $<TARGET_FILE_DIR:corridor>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${FMOD_STUDIO_DLLS}
            $<TARGET_FILE_DIR:corridor>

        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            $<TARGET_FILE:glfw>
            $<TARGET_FILE_DIR:corridor>
    )
endif()