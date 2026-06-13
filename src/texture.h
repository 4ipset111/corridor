#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glad/glad.h>
#include <vector>
#include <string>

class Texture
{
public:
    GLuint id;

    explicit Texture(const char* path);
    ~Texture();

    void bind() const;

private:
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
};

class TextureArray
{
public:
    GLuint id;
    TextureArray(const std::vector<std::string>& paths);
    ~TextureArray();
    void bind(GLuint unit = 0) const;

private:
    TextureArray(const TextureArray&) = delete;
    TextureArray& operator=(const TextureArray&) = delete;
};

#endif // __TEXTURE_H__
