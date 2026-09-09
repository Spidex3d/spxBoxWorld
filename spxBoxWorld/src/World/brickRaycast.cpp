#include "World/brickRaycast.h"
#include <World/placeBricks.h>
#include <Scene/camera.h>

//#include <glm/gtc/matrix_inverse.hpp>

#include <limits>
#include <algorithm>

BrickRaycastHit BrickRaycast::CastFromMouse(std::vector<PlacedBrick>& bricks, const Camera& camera, double mouseX, double mouseY, int screenWidth, int screenHeight)
{
    BrickRaycastHit result;

    // ------------------------------------------------
    // Build mouse ray
    // ------------------------------------------------

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


    glm::mat4 view =
        camera.GetViewMatrix();


    glm::vec3 rayDirection =
        glm::normalize(
            glm::vec3(
                glm::inverse(view) *
                rayEye
            )
        );


    glm::vec3 rayOrigin =
        camera.GetPosition();


    // ------------------------------------------------
    // Find closest brick hit
    // ------------------------------------------------

    float closestT =
        std::numeric_limits<float>::max();


    for (PlacedBrick& brick : bricks)
    {
        if (!brick.active)
            continue;


        // Brick half-size
        glm::vec3 halfSize;

        if (brick.direction ==
            BrickDirection::AlongX)
        {
            halfSize =
                glm::vec3(
                    0.125f,
                    0.0625f,
                    0.0625f
                );
        }
        else
        {
            halfSize =
                glm::vec3(
                    0.0625f,
                    0.0625f,
                    0.125f
                );
        }


        glm::vec3 boxMin =
            brick.position - halfSize;

        glm::vec3 boxMax =
            brick.position + halfSize;


        // ------------------------------------------------
        // Ray vs AABB
        // ------------------------------------------------

        float tMin = 0.0f;
        float tMax = closestT;

        bool hitBox = true;


        for (int axis = 0; axis < 3; ++axis)
        {
            if (std::abs(rayDirection[axis]) < 0.000001f)
            {
                // Ray parallel to this axis
                if (rayOrigin[axis] < boxMin[axis] ||
                    rayOrigin[axis] > boxMax[axis])
                {
                    hitBox = false;
                    break;
                }
            }
            else
            {
                float invD =
                    1.0f /
                    rayDirection[axis];

                float t1 =
                    (boxMin[axis] -
                        rayOrigin[axis]) * invD;

                float t2 =
                    (boxMax[axis] -
                        rayOrigin[axis]) * invD;


                if (t1 > t2)
                {
                    std::swap(t1, t2);
                }


                tMin =
                    std::max(
                        tMin,
                        t1
                    );

                tMax =
                    std::min(
                        tMax,
                        t2
                    );


                if (tMin > tMax)
                {
                    hitBox = false;
                    break;
                }
            }
        }


        if (hitBox &&
            tMin >= 0.0f &&
            tMin < closestT)
        {
            closestT = tMin;

            result.hit = true;

            result.brick =
                &brick;

            result.hitPosition =
                rayOrigin +
                rayDirection * tMin;
        }
    }


    return result;
}