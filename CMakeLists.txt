cmake_minimum_required(VERSION 3.10)
project(corridor)

set(CMAKE_CXX_STANDARD 17)

include(FetchContent)

FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw
    GIT_TAG 3.4
)

set(GLFW_BUILD_DLL ON CACHE BOOL "" FORCE)

FetchContent_Declare(
    glm
    GIT_REPOSITORY https://github.com/g-truc/glm
    GIT_TAG 1.0.1
)

FetchContent_Declare(
    freetype
    GIT_REPOSITORY https://github.com/freetype/freetype
    GIT_TAG VER-2-14-3
)

FetchContent_MakeAvailable(glfw glm freetype)

set(FMOD_SDK_PATH "external/fmod" CACHE PATH "Path to the FMOD SDK root")

set(_FMOD_CORE_SEARCH_DIRS
    "${FMOD_SDK_PATH}/core/lib"
    "${FMOD_SDK_PATH}/core/lib/x64"
    "${FMOD_SDK_PATH}/core/lib/win/x86_64"
    "${FMOD_SDK_PATH}/core/lib/linux/x86_64"
    "${FMOD_SDK_PATH}/core/lib/mac"
)

set(_FMOD_STUDIO_SEARCH_DIRS
    "${FMOD_SDK_PATH}/studio/lib"
    "${FMOD_SDK_PATH}/studio/lib/x64"
    "${FMOD_SDK_PATH}/studio/lib/win/x86_64"
    "${FMOD_SDK_PATH}/studio/lib/linux/x86_64"
    "${FMOD_SDK_PATH}/studio/lib/mac"
)

find_library(FMOD_CORE_LIBRARY
    NAMES
        # Windows
        fmod_vc fmod
        # Linux
        libfmod.so libfmod.so.14 libfmod.so.14.14
        fmod
        libfmod.a
        # macOS
        fmod
    PATHS ${_FMOD_CORE_SEARCH_DIRS}
    NO_DEFAULT_PATH
)

if(NOT FMOD_CORE_LIBRARY)
    message(FATAL_ERROR
        "FMOD core library not found.\n"
        "Download FMOD Engine from https://www.fmod.com/download "
        "and place it at ${FMOD_SDK_PATH}, or set -DFMOD_SDK_PATH=<path>.")
endif()

find_library(FMOD_CORE_L_LIBRARY
    NAMES
        libfmodL.so libfmodL.so.14 libfmodL.so.14.14
        fmodL
        libfmodL.a
    PATHS ${_FMOD_CORE_SEARCH_DIRS}
    NO_DEFAULT_PATH
)

find_library(FMOD_STUDIO_LIBRARY
    NAMES
        fmodstudio_vc fmodstudio
        libfmodstudio.so libfmodstudio.so.14 libfmodstudio.so.14.14
        fmodstudio
        libfmodstudio.a
    PATHS ${_FMOD_STUDIO_SEARCH_DIRS}
    NO_DEFAULT_PATH
)

if(NOT FMOD_STUDIO_LIBRARY)
    message(FATAL_ERROR
        "FMOD Studio library not found.\n"
        "Download FMOD Studio from https://www.fmod.com/download "
        "and place it at ${FMOD_SDK_PATH}, or set -DFMOD_SDK_PATH=<path>.")
endif()

find_library(FMOD_FSBANK_LIBRARY
    NAMES
        fsbank fsbank_vc
        libfsbank.so libfsbank.so.14 libfsbank.so.14.14
        fsbank
        libfsbank.a
    PATHS ${_FMOD_CORE_SEARCH_DIRS}
    NO_DEFAULT_PATH
)

message(STATUS "FMOD core    : ${FMOD_CORE_LIBRARY}")
message(STATUS "FMOD core L  : ${FMOD_CORE_L_LIBRARY}")
message(STATUS "FMOD studio  : ${FMOD_STUDIO_LIBRARY}")
message(STATUS "FMOD fsbank  : ${FMOD_FSBANK_LIBRARY}")

set(SOURCE_FILES
    src/main.cpp
    src/shader.cpp
    src/texture.cpp
    src/geometry.cpp
    src/audio_manager.cpp
    src/camera.cpp
    src/text_renderer.cpp
    src/model.cpp
)

set(EXTERNAL_LIBS
    external/glad/include/glad/glad.h
    external/glad/include/KHR/khrplatform.h
    external/glad/src/glad.c
    external/stb/stb_image_resize2.h
    external/stb/stb_image.h
)

add_executable(corridor ${SOURCE_FILES} ${EXTERNAL_LIBS})

target_include_directories(corridor PRIVATE
    src
    external/glad/include
    external/stb
    "${FMOD_SDK_PATH}/core/inc"
    "${FMOD_SDK_PATH}/studio/inc"
)

set(_FMOD_LINK_LIBS
    ${FMOD_CORE_LIBRARY}
    ${FMOD_STUDIO_LIBRARY}
)
if(FMOD_CORE_L_LIBRARY)
    list(APPEND _FMOD_LINK_LIBS ${FMOD_CORE_L_LIBRARY})
endif()
if(FMOD_FSBANK_LIBRARY)
    list(APPEND _FMOD_LINK_LIBS ${FMOD_FSBANK_LIBRARY})
endif()

target_link_libraries(corridor PRIVATE
    glfw
    glm::glm
    freetype
    ${_FMOD_LINK_LIBS}
)

if(WIN32)
    file(GLOB FMOD_CORE_DLLS   "${FMOD_SDK_PATH}/core/lib/x64/*.dll")
    file(GLOB FMOD_STUDIO_DLLS "${FMOD_SDK_PATH}/studio/lib/x64/*.dll")

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

elseif(UNIX AND NOT APPLE)
    file(GLOB FMOD_LINUX_SOS
        "${FMOD_SDK_PATH}/core/lib/linux/x86_64/*.so*"
        "${FMOD_SDK_PATH}/studio/lib/linux/x86_64/*.so*"
    )

    if(FMOD_LINUX_SOS)
        add_custom_command(TARGET corridor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${FMOD_LINUX_SOS}
                $<TARGET_FILE_DIR:corridor>
            COMMENT "Copying FMOD shared libraries to output directory"
        )
    endif()

    set_target_properties(corridor PROPERTIES
        BUILD_RPATH "$ORIGIN"
        INSTALL_RPATH "$ORIGIN"
    )

elseif(APPLE)
    file(GLOB FMOD_MAC_DYLIBS
        "${FMOD_SDK_PATH}/core/lib/mac/*.dylib"
        "${FMOD_SDK_PATH}/studio/lib/mac/*.dylib"
    )

    if(FMOD_MAC_DYLIBS)
        add_custom_command(TARGET corridor POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
                ${FMOD_MAC_DYLIBS}
                $<TARGET_FILE_DIR:corridor>
            COMMENT "Copying FMOD dylibs to output directory"
        )
    endif()

    set_target_properties(corridor PROPERTIES
        BUILD_RPATH "@loader_path"
        INSTALL_RPATH "@loader_path"
    )
endif()