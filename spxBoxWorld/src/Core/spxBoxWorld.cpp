#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core\window.h>
#include "Core/spxBoxWorld.h"
#include  <Core\input.h>  
#include <Graphics/render.h>
#include <Scene/camera.h>
#include <iostream>
#include <algorithm>
#include <boxLog.h>

#include <World/blockRaycast.h>
#include <World/world.h>
#include <World/placeBricks.h>

enum class BrickEdge
{
    Left,
    Right,
    Bottom,
    Top
};

struct BrickPlacementInfo
{
    BrickEdge edge = BrickEdge::Top;

    int slotIndex = 0;

    glm::vec3 position =
        glm::vec3(0.0f);

    float rotationY = 0.0f;

    BrickDirection direction =
        BrickDirection::AlongX;
};

spxBoxWorld::spxBoxWorld()
{}

spxBoxWorld::~spxBoxWorld()
{}

static BrickPlacementInfo GetBrickPlacementInfo(
    const RaycastHit& hit)
{
    BrickPlacementInfo info;

    // ------------------------------------------------
    // Mouse hit position relative to block centre
    // ------------------------------------------------

    float localX =
        hit.hitPosition.x -
        static_cast<float>(hit.block->x);

    float localZ =
        hit.hitPosition.z -
        static_cast<float>(hit.block->z);


    // ------------------------------------------------
    // Find nearest edge
    // ------------------------------------------------

    float distLeft =
        std::abs(localX + 0.5f);

    float distRight =
        std::abs(0.5f - localX);

    float distBottom =
        std::abs(localZ + 0.5f);

    float distTop =
        std::abs(0.5f - localZ);


    info.edge =
        BrickEdge::Top;

    float nearestDistance =
        distTop;


    if (distBottom < nearestDistance)
    {
        nearestDistance = distBottom;
        info.edge = BrickEdge::Bottom;
    }

    if (distLeft < nearestDistance)
    {
        nearestDistance = distLeft;
        info.edge = BrickEdge::Left;
    }

    if (distRight < nearestDistance)
    {
        nearestDistance = distRight;
        info.edge = BrickEdge::Right;
    }


    // ------------------------------------------------
    // Work out slot
    // ------------------------------------------------

    if (info.edge == BrickEdge::Top ||
        info.edge == BrickEdge::Bottom)
    {
        float edgeX =
            localX + 0.5f;

        info.slotIndex =
            static_cast<int>(
                edgeX / 0.25f
                );
    }
    else
    {
        float edgeZ =
            localZ + 0.5f;

        info.slotIndex =
            static_cast<int>(
                edgeZ / 0.25f
                );
    }


    info.slotIndex =
        std::clamp(
            info.slotIndex,
            0,
            3
        );


    // ------------------------------------------------
    // Vertical position
    // ------------------------------------------------

    info.position.y =
        static_cast<float>(hit.block->y)
        + 0.5f
        + 0.0625f;


    // ------------------------------------------------
    // Edge-specific position + rotation
    // ------------------------------------------------

    if (info.edge == BrickEdge::Top)
    {
        info.position.x =
            static_cast<float>(hit.block->x)
            - 0.5f
            + 0.125f
            + (info.slotIndex * 0.25f);

        info.position.z =
            static_cast<float>(hit.block->z)
            + 0.5f
            - 0.0625f;

        info.rotationY = 0.0f;

        info.direction =
            BrickDirection::AlongX;
    }

    else if (info.edge == BrickEdge::Bottom)
    {
        info.position.x =
            static_cast<float>(hit.block->x)
            - 0.5f
            + 0.125f
            + (info.slotIndex * 0.25f);

        info.position.z =
            static_cast<float>(hit.block->z)
            - 0.5f
            + 0.0625f;

        info.rotationY = 0.0f;

        info.direction =
            BrickDirection::AlongX;
    }

    else if (info.edge == BrickEdge::Left)
    {
        info.position.x =
            static_cast<float>(hit.block->x)
            - 0.5f
            + 0.0625f;

        info.position.z =
            static_cast<float>(hit.block->z)
            - 0.5f
            + 0.125f
            + (info.slotIndex * 0.25f);

        info.rotationY = 90.0f;

        info.direction =
            BrickDirection::AlongZ;
    }

    else if (info.edge == BrickEdge::Right)
    {
        info.position.x =
            static_cast<float>(hit.block->x)
            + 0.5f
            - 0.0625f;

        info.position.z =
            static_cast<float>(hit.block->z)
            - 0.5f
            + 0.125f
            + (info.slotIndex * 0.25f);

        info.rotationY = 90.0f;

        info.direction =
            BrickDirection::AlongZ;
    }


    return info;
}





bool spxBoxWorld::Initialize()
{
    BOX_LOG_INFO("spxBoxWorld initialized from the library.");

    m_window = std::make_unique<Window>();

    if (!m_window->Create(1280, 720, "spxBoxWorld"))
    {
        std::cout << "Failed to create window." << std::endl;
        return false;
    }
    // -----------------------------------------
    // Initialize the camera
    // -----------------------------------------
    m_camera = std::make_unique<Camera>(glm::vec3(0.5f, 2.0f, 10.0f));

    Input::SetupMouse(m_window->GetNativeWindow(), m_camera.get());

    // -----------------------------------------
	// Initialize the renderer
	// -----------------------------------------

    m_renderer = std::make_unique<Render>();

    if (!m_renderer->Initialize())
    {
        std::cout << "Failed to initialize renderer." << std::endl;
        return false;
    }

    return true;
}

void spxBoxWorld::Run()
{
    float lastFrame = 0.0f;

    bool lastLeftMouse = false;
    bool lastRightMouse = false;

    std::vector<PlacedBrick> placedBricks;
    BuildMode m_buildMode = BuildMode::Block;

    while (!m_window->ShouldClose())
    {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        float deltaTime =
            currentFrame - lastFrame;

        lastFrame = currentFrame;

        m_window->PollEvents();

        if (glfwGetKey(
            m_window->GetNativeWindow(),
            GLFW_KEY_1) == GLFW_PRESS)
        {
            m_buildMode = BuildMode::Block;
        }

        if (glfwGetKey(
            m_window->GetNativeWindow(),
            GLFW_KEY_2) == GLFW_PRESS)
        {
            m_buildMode = BuildMode::Brick;

        }

        Input::ProcessKeyboard(
            m_window->GetNativeWindow(),
            *m_camera,
            deltaTime
        );

        double mouseX;
        double mouseY;

        glfwGetCursorPos(
            m_window->GetNativeWindow(),
            &mouseX,
            &mouseY
        );

        // ------------------------------------------------
        // Raycast
        // ------------------------------------------------
        RaycastHit hit =
            BlockRaycast::CastFromMouse(
                *m_renderer->GetWorld(),
                *m_camera,
                mouseX,
                mouseY,
                1280,
                720
            );

        Block* selectedBlock = hit.hit ? hit.block : nullptr;


        // ------------------------------------------------
        // Remove block - Left Mouse
        // ------------------------------------------------
        // GLFW_MOUSE_BUTTON_LEFT
        bool leftMouse =
            glfwGetMouseButton(
                m_window->GetNativeWindow(),
                GLFW_MOUSE_BUTTON_RIGHT
            ) == GLFW_PRESS;

        if (leftMouse &&
            !lastLeftMouse)
        {
            // ---------------------------------------------
            // Remove normal world block
            // ---------------------------------------------
            if (m_buildMode == BuildMode::Block)
            {
                if (hit.hit && hit.block)
                {
                    m_renderer->GetWorld()->RemoveBlock(
                        hit.block->x,
                        hit.block->y,
                        hit.block->z
                    );

                    selectedBlock = nullptr;
                }
            }

            // ---------------------------------------------
            // Remove placed brick
            // ---------------------------------------------

            else if (m_buildMode == BuildMode::Brick)
            {
                if (hit.hit && hit.block)
                {
                    BrickPlacementInfo info =
                        GetBrickPlacementInfo(hit);

                    for (int i = 0;
                        i < static_cast<int>(placedBricks.size());
                        ++i)
                    {
                        if (glm::distance(
                            placedBricks[i].position,
                            info.position
                        ) < 0.001f)
                        {
                            placedBricks.erase(
                                placedBricks.begin() + i
                            );

                            std::cout
                                << "Removed brick from slot "
                                << info.slotIndex
                                << std::endl;

                            break;
                        }
                    }
                }
            }

        }

        lastLeftMouse = leftMouse;

        // ------------------------------------------------
        // Add block / brick - Right Mouse
        // ------------------------------------------------

        // GLFW_MOUSE_BUTTON_RIGHT
        bool rightMouse =
            glfwGetMouseButton(
                m_window->GetNativeWindow(),
                GLFW_MOUSE_BUTTON_LEFT
            ) == GLFW_PRESS;

        if (rightMouse &&
            !lastRightMouse &&
            hit.hit &&
            hit.block)
        {
            // ------------------------------------------------
            // Normal block placement
            // ------------------------------------------------
       
            BrickPlacementInfo info =
                GetBrickPlacementInfo(hit);

            PlacedBrick newBrick;

            newBrick.position =
                info.position;

            newBrick.rotationY =
                info.rotationY;

            newBrick.direction =
                info.direction;

            newBrick.active = true;


            // Prevent duplicate bricks
            bool occupied = false;

            for (const PlacedBrick& brick : placedBricks)
            {
                if (glm::distance(
                    brick.position,
                    info.position
                ) < 0.001f)
                {
                    occupied = true;
                    break;
                }
            }


            if (!occupied)
            {
                placedBricks.push_back(newBrick);
            }


            std::cout
                << "Placed brick slot: "
                << info.slotIndex
                << std::endl;
        }


        // ------------------------------------------------
        // ################ Brick placement ###############
        // ------------------------------------------------
        else if (m_buildMode == BuildMode::Brick)
        {
            // ------------------------------------------------
            // Mouse hit position relative to block centre
            // ------------------------------------------------

            BrickPlacementInfo info = GetBrickPlacementInfo(hit);


            PlacedBrick newBrick;

            newBrick.position =
                info.position;

            newBrick.rotationY =
                info.rotationY;

            newBrick.direction =
                info.direction;

            newBrick.active = true;




            // ------------------------------------------------
            // Prevent duplicate bricks
            // ------------------------------------------------

            bool occupied = false;

            for (const PlacedBrick& brick :
                placedBricks)
            {
                if (glm::distance(
                    brick.position,
                    info.position
                ) < 0.001f)
                {
                    occupied = true;
                    break;
                }
            }

            if (!occupied)
            {
                placedBricks.push_back(newBrick);
            }



        }

        lastRightMouse = rightMouse;


        // ------------------------------------------------
        // Debug selection output
        // ------------------------------------------------
        static Block* lastSelected = nullptr;

        if (selectedBlock != lastSelected)
        {
            lastSelected = selectedBlock;

            if (selectedBlock)
            {
                std::cout
                    << "Selected block: "
                    << selectedBlock->x << ", "
                    << selectedBlock->y << ", "
                    << selectedBlock->z
                    << std::endl;
            }
        }


        // ------------------------------------------------
        // Render
        // ------------------------------------------------
        m_renderer->RenderFrame(
            *m_camera,
            selectedBlock,
            placedBricks
        );

        m_window->SwapBuffers();
        }
    }


void spxBoxWorld::Shutdown()
{
    if (m_renderer)
    {
        m_renderer->Shutdown();
        m_renderer.reset();
    }

    if (m_window)
    {
        m_window->Destroy();
        m_window.reset();
    }

    BOX_LOG_INFO("spxBoxWorld shutdown.");
}




//    glm::vec3 brickPos;

               //   // brickPos.x = static_cast<float>(hit.block->x) + -edgeLength + brickDepth;
               //    brickPos.x =
               //        static_cast<float>(hit.block->x)
               //        - 0.5f
               //        + 0.125f
               //        + 0.25f;

               //    brickPos.y =
               //        static_cast<float>(hit.block->y)
               //        + edgeLength
               //        + 0.0625f;

               //    brickPos.z =
               //        static_cast<float>(hit.block->z)
               //        + 0.4375f;
               //   
               //    PlacedBrick newBrick;

               //    newBrick.position = brickPos;
               //    newBrick.rotationY = 0.0f;
               //    newBrick.active = true;

               //    placedBricks.push_back(newBrick);

               //    std::cout
               //        << "Placed brick at: "
               //        << brickPos.x << ", "
               //        << brickPos.y << ", "
               //        << brickPos.z
               //        << std::endl;
               //}