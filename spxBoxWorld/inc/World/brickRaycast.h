#pragma once
#include <glm/glm.hpp>
#include <vector>

class Camera;
struct PlacedBrick;

struct BrickRaycastHit
{
    PlacedBrick* brick = nullptr;

    glm::vec3 hitPosition =
        glm::vec3(0.0f);

    bool hit = false;
};

class BrickRaycast
{
public:

    static BrickRaycastHit CastFromMouse(std::vector<PlacedBrick>& bricks,
        const Camera& camera, double mouseX, double mouseY, int screenWidth, int screenHeight);
};