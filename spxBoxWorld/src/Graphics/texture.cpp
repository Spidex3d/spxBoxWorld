#include <Graphics/texture.h>
#include <stb/stb_image.h>

#include <iostream>


Texture::Texture()
{
}


Texture::~Texture()
{
    Destroy();
}


bool Texture::LoadFromFile(
    const std::string& filePath)
{
    Destroy();

    stbi_set_flip_vertically_on_load(true);

    unsigned char* data =
        stbi_load(
            filePath.c_str(),
            &m_width,
            &m_height,
            &m_channels,
            0
        );

    if (!data)
    {
        std::cout
            << "Failed to load texture: "
            << filePath
            << std::endl;

        return false;
    }


    GLenum format = GL_RGB;

    if (m_channels == 1)
    {
        format = GL_RED;
    }
    else if (m_channels == 3)
    {
        format = GL_RGB;
    }
    else if (m_channels == 4)
    {
        format = GL_RGBA;
    }


    glGenTextures(
        1,
        &m_textureID
    );

    glBindTexture(
        GL_TEXTURE_2D,
        m_textureID
    );


    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_REPEAT
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR_MIPMAP_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );


    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        format,
        m_width,
        m_height,
        0,
        format,
        GL_UNSIGNED_BYTE,
        data
    );

    glGenerateMipmap(
        GL_TEXTURE_2D
    );


    stbi_image_free(data);

    glBindTexture(
        GL_TEXTURE_2D,
        0
    );


    std::cout
        << "Texture loaded: "
        << filePath
        << std::endl;


    return true;
}


void Texture::Bind(
    unsigned int slot) const
{
    glActiveTexture(
        GL_TEXTURE0 + slot
    );

    glBindTexture(
        GL_TEXTURE_2D,
        m_textureID
    );
}


void Texture::Destroy()
{
    if (m_textureID)
    {
        glDeleteTextures(
            1,
            &m_textureID
        );

        m_textureID = 0;
    }
}