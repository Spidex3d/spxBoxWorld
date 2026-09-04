#version 460 core

out vec4 FragColor;

in vec2 TexCoord;

uniform sampler2D baseTexture;

uniform vec3 blockColor;
uniform bool useTexture;

void main()
{
    if (useTexture)
    {
        FragColor = texture(baseTexture, TexCoord);
    }
    else
    {
        FragColor = vec4(blockColor, 1.0);
    }
}

//out vec4 FragColor;
//
//uniform vec3 blockColor;
//
//void main()
//{
//    FragColor = vec4(blockColor, 1.0);
//}