#pragma once

#include <glm/glm.hpp>

class World;
struct Block;
class Camera;



struct RaycastHit
{
    Block* block = nullptr;

    glm::ivec3 normal = glm::ivec3(0);

    // Empty cell immediately before the selected block
    glm::ivec3 placePosition = glm::ivec3(0);

    glm::vec3 hitPosition = glm::vec3(0.0f);

    bool hit = false;
};

class BlockRaycast
{
public:

    static RaycastHit CastFromMouse(
        World& world,
        const Camera& camera,
        double mouseX,
        double mouseY,
        int screenWidth,
        int screenHeight
    );

    static RaycastHit Cast(
        World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance = 10.0f,
        float stepSize = 0.05f
    );


    /*static Block* CastFromMouse(
        World& world,
        const Camera& camera,
        double mouseX,
        double mouseY,
        int screenWidth,
        int screenHeight
    );


    static Block* Cast(
        World& world,
        const glm::vec3& origin,
        const glm::vec3& direction,
        float maxDistance = 10.0f,
        float stepSize = 0.05f
    );*/
};
