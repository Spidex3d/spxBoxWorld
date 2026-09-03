#pragma once
#include <string>
#include <vector>

#include <glm/glm.hpp>


struct MBXVertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 uv;
};


struct MBXMaterial
{
    std::string name;

    glm::vec4 baseColor = glm::vec4(1.0f);

    float metallic = 0.0f;
    float roughness = 0.5f;
    float alpha = 1.0f;

    glm::vec3 emissionColor = glm::vec3(0.0f);
    float emissionStrength = 0.0f;

    std::string baseColorMap;
};


struct MBXTriangle
{
    unsigned int indices[3];

    int materialIndex = 0;
};


struct MBXModel
{
    std::string name;

    std::vector<MBXVertex> vertices;
    std::vector<MBXTriangle> triangles;
    std::vector<MBXMaterial> materials;
};


class MBXLoader
{
public:

    bool Load(const std::string& filePath, MBXModel& outModel);
};
