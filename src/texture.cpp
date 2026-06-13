#include "texture.h"
#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize2.h"

Texture::Texture(const char* path)
{
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true);
    unsigned char* data = stbi_load(path, &width, &height, &nrChannels, 0);
    
    if (data)
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        std::cout << "Texture loaded: " << path << std::endl;
    }
    else
    {
        std::cerr << "Failed to load texture: " << path << std::endl;
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &id);
}

void Texture::bind() const
{
    glBindTexture(GL_TEXTURE_2D, id);
}

TextureArray::TextureArray(const std::vector<std::string>& paths)
{
    int storageWidth, storageHeight, nrChannels;
    stbi_set_flip_vertically_on_load(true);

    unsigned char* data = stbi_load(paths[0].c_str(), &storageWidth, &storageHeight, &nrChannels, 4);
    if (!data) return;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGBA8, storageWidth, storageHeight, (GLsizei)paths.size());

    for (int i = 0; i < paths.size(); ++i) {
        int currentWidth, currentHeight;
        if (i > 0) {
            data = stbi_load(paths[i].c_str(), &currentWidth, &currentHeight, &nrChannels, 4);
        } else {
            currentWidth = storageWidth;
            currentHeight = storageHeight;
        }

        if (data) {
            unsigned char* uploadData = data;
            unsigned char* resizedData = nullptr;

            if (currentWidth != storageWidth || currentHeight != storageHeight) {
                std::cout << "Resizing texture '" << paths[i] << "' from " << currentWidth << "x" << currentHeight 
                          << " to " << storageWidth << "x" << storageHeight << "..." << std::endl;
                
                resizedData = (unsigned char*)malloc(storageWidth * storageHeight * 4);
                stbir_resize_uint8_srgb(data, currentWidth, currentHeight, 0, 
                                   resizedData, storageWidth, storageHeight, 0, 
                                   STBIR_RGBA);
                
                uploadData = resizedData;
            }
            glTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, storageWidth, storageHeight, 1, GL_RGBA, GL_UNSIGNED_BYTE, uploadData);
            
            if (resizedData) free(resizedData);
            stbi_image_free(data);
            std::cout << "Texture Array Layer " << i << " loaded: " << paths[i] << std::endl;
        } else {
            std::cerr << "Failed to load Texture Array Layer: " << paths[i] << std::endl;
        }
    }

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_REPEAT);
    
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

TextureArray::~TextureArray()
{
    glDeleteTextures(1, &id);
}

void TextureArray::bind(GLuint unit) const
{
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D_ARRAY, id);
}
