#pragma once
#include <glm/glm.hpp>

enum class BrickDirection
{
    AlongX,
    AlongZ
};

enum class BrickType
{
    Full,
    Half
};

struct PlacedBrick
{
    glm::vec3 position = glm::vec3(0.0f);

    float rotationY = 0.0f;

    BrickDirection direction = BrickDirection::AlongX;

    BrickType type = BrickType::Full;

    int layerIndex = 0;

    bool active = true;
};

