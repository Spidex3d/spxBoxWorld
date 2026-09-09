#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core\window.h>
#include "Core/spxBoxWorld.h"
#include  <Core\input.h>  
#include <Graphics/render.h>
#include <Scene/camera.h>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <boxLog.h>

#include <World/blockRaycast.h>
#include <World/world.h>
#include <World/placeBricks.h>
#include <World/brickRaycast.h>

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

    int layerIndex = 0;

    glm::vec3 position =
        glm::vec3(0.0f);

    float rotationY = 0.0f;

    BrickDirection direction =
        BrickDirection::AlongX;
};

static PlacedBrick* FindBrickUnderMouse(
    std::vector<PlacedBrick>& placedBricks,
    const RaycastHit& hit)
{
    PlacedBrick* closestBrick = nullptr;
    float closestDistance = 0.20f;

    for (PlacedBrick& brick : placedBricks)
    {
        float distance =
            glm::distance(
                brick.position,
                hit.hitPosition
            );

        if (distance < closestDistance)
        {
            closestDistance = distance;
            closestBrick = &brick;
        }
    }

    return closestBrick;
}

spxBoxWorld::spxBoxWorld()
{}

spxBoxWorld::~spxBoxWorld()
{}

static BrickPlacementInfo GetBrickPlacementInfo(const RaycastHit& hit, int layerIndex)
{
    BrickPlacementInfo info;

    // ------------------------------------------------
    // Mouse hit position relative to block centre
    // ------------------------------------------------

    float localX = hit.hitPosition.x -
        static_cast<float>(hit.block->x);

    float localZ = hit.hitPosition.z -
        static_cast<float>(hit.block->z);

    // ------------------------------------------------
    // Find nearest edge
    // ------------------------------------------------

    float distLeft = std::abs(localX + 0.5f);

    float distRight = std::abs(0.5f - localX);

    float distBottom = std::abs(localZ + 0.5f);

    float distTop = std::abs(0.5f - localZ);


    info.edge = BrickEdge::Top;

    float nearestDistance = distTop;


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
        // X edges have 4 full brick slots
        float edgeX =
            localX + 0.5f;

        info.slotIndex =
            static_cast<int>(
                edgeX / 0.25f
                );

        info.slotIndex =
            std::clamp(
                info.slotIndex,
                0,
                3
            );
    }
    else
    {
        // Z edges have 3 full brick slots.
        // The X-running bricks own both corners.

        float edgeZ =
            localZ + 0.5f;

        info.slotIndex =
            static_cast<int>(
                edgeZ / 0.25f
                );

        info.slotIndex =
            std::clamp(
                info.slotIndex,
                0,
                2
            );
    }



    // ------------------------------------------------
    // Vertical position
    // ------------------------------------------------

    constexpr float brickHeight = 0.125f;

    // TEMP test:
    // layer 0 = first row
    // layer 1 = second row
    info.layerIndex = layerIndex;

    info.position.y =
        static_cast<float>(hit.block->y)
        + 0.5f
        + (brickHeight * 0.5f)
        + (info.layerIndex * brickHeight);

   

    // ------------------------------------------------
    // Edge-specific position + rotation
    // ------------------------------------------------

    if (info.edge == BrickEdge::Top)
    {

        if (info.layerIndex % 2 == 0)
        {
            // Normal row
            info.position.x =
                static_cast<float>(hit.block->x)
                - 0.5f
                + 0.125f
                + (info.slotIndex * 0.25f);
        }
        else
        {
            // Staggered row
            info.position.x =
                static_cast<float>(hit.block->x)
                - 0.25f
                + (info.slotIndex * 0.25f);
        }

       
        info.position.z =
            static_cast<float>(hit.block->z)
            + 0.5f
            - 0.0625f;

        info.rotationY = 0.0f;

        info.direction = BrickDirection::AlongX;
    }

    else if (info.edge == BrickEdge::Bottom)
    {
        // Stagger X position
        if (info.layerIndex % 2 == 0)
        {
            // Normal row
            info.position.x =
                static_cast<float>(hit.block->x)
                - 0.5f
                + 0.125f
                + (info.slotIndex * 0.25f);
        }
        else
        {
            // Staggered row
            info.position.x =
                static_cast<float>(hit.block->x)
                - 0.25f
                + (info.slotIndex * 0.25f);
        }

        // Keep Bottom edge Z position
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

        if (info.layerIndex % 2 == 0)
        {
            // Normal row
            info.position.z =
                static_cast<float>(hit.block->z)
                - 0.25f
                + (info.slotIndex * 0.25f);
        }
        else
        {
            // Staggered row - move half a brick
            info.position.z =
                static_cast<float>(hit.block->z)
                - 0.125f
                + (info.slotIndex * 0.25f);
        }

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

        if (info.layerIndex % 2 == 0)
        {
            // Normal row
            info.position.z =
                static_cast<float>(hit.block->z)
                - 0.25f
                + (info.slotIndex * 0.25f);
        }
        else
        {
            // Staggered row - move half a brick
            info.position.z =
                static_cast<float>(hit.block->z)
                - 0.125f
                + (info.slotIndex * 0.25f);
        }

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

    PlacedBrick* selectedBrick = nullptr;

    


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

        PlacedBrick* selectedBrick = nullptr;

        if (hit.hit)
        {
            selectedBrick = FindBrickUnderMouse(placedBricks, hit);
        }

        // -----------------------------------------------
		// Temp: Output hit information
        // -----------------------------------------------
        static PlacedBrick* lastSelectedBrick =
            nullptr;

        if (selectedBrick != lastSelectedBrick)
        {
            lastSelectedBrick = selectedBrick;

            if (selectedBrick)
            {
                std::cout
                    << "Selected brick layer: "
                    << selectedBrick->layerIndex
                    << " position: "
                    << selectedBrick->position.x << ", "
                    << selectedBrick->position.y << ", "
                    << selectedBrick->position.z
                    << std::endl;
            }
        }

		// -------------------------------------------------
		// brick raycast temp test
		// -------------------------------------------------
        BrickRaycastHit brickHit =
            BrickRaycast::CastFromMouse(
                placedBricks,
                *m_camera,
                mouseX,
                mouseY,
                1280,
                720
            );

        static PlacedBrick* lastBrickHit = nullptr;

        if (m_buildMode == BuildMode::Brick &&
            brickHit.hit &&
            brickHit.brick)
        {
            selectedBrick =
                brickHit.brick;
        }
        else
        {
            selectedBrick =
                nullptr;
        }



        if (brickHit.brick != lastBrickHit)
        {
            lastBrickHit = brickHit.brick;

            if (brickHit.hit && brickHit.brick)
            {
                std::cout
                    << "Brick ray hit - layer "
                    << brickHit.brick->layerIndex
                    << " position: "
                    << brickHit.brick->position.x << ", "
                    << brickHit.brick->position.y << ", "
                    << brickHit.brick->position.z
                    << std::endl;
            }
        }


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
            // Remove Selected brick
            // ---------------------------------------------

            else if (m_buildMode == BuildMode::Brick)
            {
                if (brickHit.hit &&
                    brickHit.brick)
                {
                    for (int i = 0;
                        i < static_cast<int>(placedBricks.size());
                        ++i)
                    {
                        // Is this the exact brick the raycast hit?
                        if (&placedBricks[i] ==
                            brickHit.brick)
                        {
                            std::cout
                                << "Removed brick layer "
                                << placedBricks[i].layerIndex
                                << std::endl;

                            placedBricks.erase(
                                placedBricks.begin() + i
                            );

                            selectedBrick = nullptr;

                            break;
                        }
                    }
                }
            }

        }

        lastLeftMouse = leftMouse;

        

        // GLFW_MOUSE_BUTTON_RIGHT
        // ------------------------------------------------
        // Add block / brick - Left Mouse
        // ------------------------------------------------

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
            // test

            if (brickHit.hit && brickHit.brick)
            {
                int nextLayer = brickHit.brick->layerIndex + 1;

                std::cout
                    << "Clicked brick layer "
                    << brickHit.brick->layerIndex
                    << " -> next layer "
                    << nextLayer
                    << std::endl;

                glm::vec3 nextBrickPos =
                    brickHit.brick->position;

                nextBrickPos.y += 0.125f;

                // Stagger every new layer by half a brick
                
                
                
                if (brickHit.brick->direction ==
                    BrickDirection::AlongX)
                {
                    nextBrickPos.x += 0.125f;
                }
                else if (brickHit.brick->direction ==
                    BrickDirection::AlongZ)
                {
                    nextBrickPos.z += 0.125f;
                }



                std::cout
                    << "Next brick position: "
                    << nextBrickPos.x << ", "
                    << nextBrickPos.y << ", "
                    << nextBrickPos.z
                    << std::endl;
            }

            // ------------------------------------------------
            // Normal block placement
            // ------------------------------------------------

            if (m_buildMode == BuildMode::Block)
            {
                int newX = hit.placePosition.x;
                int newY = hit.placePosition.y;
                int newZ = hit.placePosition.z;

                bool added =
                    m_renderer->GetWorld()->AddBlock(
                        BlockType::Grass,
                        newX,
                        newY,
                        newZ
                    );

                if (added)
                {
                    std::cout
                        << "Added block: "
                        << newX << ", "
                        << newY << ", "
                        << newZ
                        << std::endl;

                    selectedBlock = nullptr;
                }
            }

            // ------------------------------------------------
            // Brick placement
            // ------------------------------------------------

            else if (m_buildMode == BuildMode::Brick)
            {
                // ------------------------------------------------
                // CASE 1:
                // Mouse is pointing directly at an existing brick
                // ------------------------------------------------

                if (brickHit.hit && brickHit.brick)
                {
                    PlacedBrick& baseBrick =
                        *brickHit.brick;

                    int nextLayer =
                        baseBrick.layerIndex + 1;

                    glm::vec3 nextBrickPos =
                        baseBrick.position;

                    // Move up one brick height
                    nextBrickPos.y += 0.125f;


                    // ------------------------------------------------
					// Stagger by half a brick both ways, depending on the layer index
                    // ------------------------------------------------

                    if (baseBrick.direction == BrickDirection::AlongX)
                    {
                        // ------------------------------------------------
                        // Brick underneath is a HALF brick
                        // ------------------------------------------------
                        if (baseBrick.type == BrickType::Half)
                        {
                            float blockCenterX =
                                std::round(baseBrick.position.x);

                            // Right-hand half brick
                            if (baseBrick.position.x > blockCenterX)
                            {
                                nextBrickPos.x -= 0.0625f;
                            }

                            // Left-hand half brick
                            else
                            {
                                nextBrickPos.x += 0.0625f;
                            }
                        }

                        // ------------------------------------------------
                        // Brick underneath is a FULL brick
                        // ------------------------------------------------
                        else
                        {
                            if (nextLayer % 2 == 0)
                            {
                                // Back to normal alignment
                                nextBrickPos.x -= 0.125f;
                            }
                            else
                            {
                                // Staggered row
                                nextBrickPos.x += 0.125f;
                            }
                        }
                    }

                    // ------------------------------------------------
                    // Prevent duplicate brick
                    // ------------------------------------------------

                    bool occupied = false;

                    for (const PlacedBrick& brick :
                        placedBricks)
                    {
                        if (glm::distance(
                            brick.position,
                            nextBrickPos
                        ) < 0.001f)
                        {
                            occupied = true;
                            break;
                        }
                    }


                    if (!occupied)
                    {
                        PlacedBrick newBrick;

                        newBrick.position =
                            nextBrickPos;

                        newBrick.rotationY =
                            baseBrick.rotationY;

                        newBrick.direction =
                            baseBrick.direction;
                        
                        

                        // ------------------------------------------------
                        // Half-brick placement - Right Mouse
                        // ------------------------------------------------
                        // ------------------------------------------------
                        // Full brick / Half brick
                        // ------------------------------------------------
                        bool ctrlPressed =
                            glfwGetKey(
                                m_window->GetNativeWindow(),
                                GLFW_KEY_LEFT_CONTROL
                            ) == GLFW_PRESS ||
                            glfwGetKey(
                                m_window->GetNativeWindow(),
                                GLFW_KEY_RIGHT_CONTROL
                            ) == GLFW_PRESS;


                        // Default to full brick
                        newBrick.type = BrickType::Full;


                        // ------------------------------------------------
                        // Half brick - AlongX
                        // ------------------------------------------------

                        if (ctrlPressed &&
                            baseBrick.direction == BrickDirection::AlongX)
                        {
                            newBrick.type =
                                BrickType::Half;

                            // Find the centre of the original 1x1 ground block
                            // from the brick itself - NOT from hit.block.
                            float blockCenterX =
                                std::round(baseBrick.position.x);

                            bool placeRightEnd =
                                brickHit.hitPosition.x >
                                blockCenterX;

                            if (placeRightEnd)
                            {
                                // Right-hand half brick
                                newBrick.position.x =
                                    blockCenterX
                                    + 0.5f
                                    - 0.0625f;
                            }
                            else
                            {
                                // Left-hand half brick
                                newBrick.position.x =
                                    blockCenterX
                                    - 0.5f
                                    + 0.0625f;
                            }
                        }


                        newBrick.layerIndex =
                            nextLayer;

                        newBrick.active = true;


                        // Store AFTER position and type are finished
                        placedBricks.push_back(newBrick);




                        if (ctrlPressed)
                        {
                            std::cout
                                << "Placed HALF brick - layer "
                                << nextLayer
                                << std::endl;
                        }
                        else
                        {
                            std::cout
                                << "Placed FULL brick - layer "
                                << nextLayer
                                << std::endl;
                        }

                       
                    }
                }


            
                // ------------------------------------------------
                // CASE 2:
                // No brick hit - place first row from ground block
                // ------------------------------------------------

                else if (hit.hit &&
                    hit.block &&
                    hit.normal.y == 1)
                {
                    BrickPlacementInfo info =
                        GetBrickPlacementInfo(
                            hit,
                            0
                        );

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
                        PlacedBrick newBrick;

                        newBrick.position =
                            info.position;

                        newBrick.rotationY =
                            info.rotationY;

                        newBrick.direction =
                            info.direction;

                        newBrick.layerIndex = 0;

                        newBrick.active = true;


                        bool ctrlPressed =
                            glfwGetKey(
                                m_window->GetNativeWindow(),
                                GLFW_KEY_LEFT_CONTROL
                            ) == GLFW_PRESS ||
                            glfwGetKey(
                                m_window->GetNativeWindow(),
                                GLFW_KEY_RIGHT_CONTROL
                            ) == GLFW_PRESS;

                        newBrick.type =
                            ctrlPressed
                            ? BrickType::Half
                            : BrickType::Full;


                        placedBricks.push_back(newBrick);
                        

                        std::cout
                            << "Placed ground brick - layer 0 slot "
                            << info.slotIndex
                            << std::endl;
                    }
                }
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
            placedBricks,
            selectedBrick
        );

       /* m_renderer->RenderFrame(
            *m_camera,
            selectedBlock,
            placedBricks
        );*/

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
