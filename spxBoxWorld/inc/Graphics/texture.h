#pragma once

#include <string>
#include <glad/glad.h>

class Texture
{
public:

    Texture();
    ~Texture();

    bool LoadFromFile(
        const std::string& filePath
    );

    void Bind(
        unsigned int slot = 0
    ) const;

    void Destroy();

    GLuint ID() const
    {
        return m_textureID;
    }

private:

    GLuint m_textureID = 0;

    int m_width = 0;
    int m_height = 0;
    int m_channels = 0;
};
