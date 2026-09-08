#pragma once
#include <glm/glm.hpp>

enum class BrickDirection
{
    AlongX,
    AlongZ
};


struct PlacedBrick
{
    glm::vec3 position = glm::vec3(0.0f);

    float rotationY = 0.0f;

    BrickDirection direction = BrickDirection::AlongX;

    bool active = true;
};

