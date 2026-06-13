#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include <glad/glad.h>

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

#endif // __TEXTURE_H__
