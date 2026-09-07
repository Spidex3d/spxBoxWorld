#pragma once
#include <glm/glm.hpp>

struct PlacedBrick
{
    glm::vec3 position = glm::vec3(0.0f);

    float rotationY = 0.0f;

    bool active = true;
};
