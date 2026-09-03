#include <World/blockRaycast.h>
#include <World/world.h>
#include <World/block.h>
#include <Scene/camera.h>

#include <cmath>
#include <glm/gtc/matrix_inverse.hpp>

RaycastHit BlockRaycast::CastFromMouse(World& world, const Camera& camera, double mouseX, double mouseY, int screenWidth, int screenHeight)
{
    float x =
        (2.0f * static_cast<float>(mouseX)) /
        static_cast<float>(screenWidth)
        - 1.0f;

    float y =
        1.0f -
        (2.0f * static_cast<float>(mouseY)) /
        static_cast<float>(screenHeight);

    glm::vec4 rayClip(
        x,
        y,
        -1.0f,
        1.0f
    );

    float aspect =
        static_cast<float>(screenWidth) /
        static_cast<float>(screenHeight);

    glm::mat4 projection =
        camera.GetProjectionMatrix(aspect);

    glm::vec4 rayEye =
        glm::inverse(projection) *
        rayClip;

    rayEye =
        glm::vec4(
            rayEye.x,
            rayEye.y,
            -1.0f,
            0.0f
        );

    glm::mat4 view = camera.GetViewMatrix();

    glm::vec3 rayWorld =
        glm::normalize(
            glm::vec3(
                glm::inverse(view) *
                rayEye
            )
        );

    return Cast(world, camera.GetPosition(), rayWorld);
}

RaycastHit BlockRaycast::Cast(
    World& world,
    const glm::vec3& origin,
    const glm::vec3& direction,
    float maxDistance,
    float stepSize)
{
    glm::vec3 rayDirection =
        glm::normalize(direction);

    glm::ivec3 previousCell(
        static_cast<int>(std::floor(origin.x + 0.5f)),
        static_cast<int>(std::floor(origin.y + 0.5f)),
        static_cast<int>(std::floor(origin.z + 0.5f))
    );

    for (float distance = 0.0f;
        distance <= maxDistance;
        distance += stepSize)
    {
        glm::vec3 point =
            origin + rayDirection * distance;

        glm::ivec3 currentCell(
            static_cast<int>(std::floor(point.x + 0.5f)),
            static_cast<int>(std::floor(point.y + 0.5f)),
            static_cast<int>(std::floor(point.z + 0.5f))
        );

        // Only do something when we enter a different grid cell
        if (currentCell != previousCell)
        {
            Block* block =
                world.GetBlock(
                    currentCell.x,
                    currentCell.y,
                    currentCell.z
                );

            if (block && block->isActive)
            {
                RaycastHit result;

                result.block = block;

                // Direction from selected block back
                // towards the empty cell we came from
                result.normal =
                    previousCell - currentCell;

                // This is where a new block should go
                result.placePosition =
                    previousCell;

                result.hit = true;

                return result;
            }

            previousCell = currentCell;
        }
    }

    return {};
}

//RaycastHit BlockRaycast::Cast(
//    World& world,
//    const glm::vec3& origin,
//    const glm::vec3& direction,
//    float maxDistance,
//    float stepSize)
//{
//    glm::vec3 rayDirection =
//        glm::normalize(direction);
//
//    for (float distance = 0.0f;
//        distance <= maxDistance;
//        distance += stepSize)
//    {
//        glm::vec3 point =
//            origin +
//            rayDirection * distance;
//
//
//        int blockX =
//            static_cast<int>(
//                std::floor(point.x + 0.5f)
//                );
//
//        int blockY =
//            static_cast<int>(
//                std::floor(point.y + 0.5f)
//                );
//
//        int blockZ =
//            static_cast<int>(
//                std::floor(point.z + 0.5f)
//                );
//
//
//        Block* block = world.GetBlock(blockX, blockY, blockZ);
//
//
//        if (block &&
//            block->isActive &&
//            block->type != BlockType::Ground)
//        {
//            RaycastHit hit;
//            hit.block = block;
//            hit.hit = true;
//            return hit;
//        }
//    }
//
//    return RaycastHit();
//}