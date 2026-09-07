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

spxBoxWorld::spxBoxWorld()
{
}

spxBoxWorld::~spxBoxWorld()
{
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
                    // Work out which brick slot the mouse is over
                    float localX =
                        hit.hitPosition.x -
                        static_cast<float>(hit.block->x);

                    float edgeX =
                        localX + 0.5f;

                    int slotIndex =
                        static_cast<int>(
                            edgeX / 0.25f
                            );

                    slotIndex =
                        std::clamp(
                            slotIndex,
                            0,
                            3
                        );

                    // Calculate the exact brick position for that slot
                    glm::vec3 targetBrickPos;

                    targetBrickPos.x =
                        static_cast<float>(hit.block->x)
                        - 0.5f
                        + 0.125f
                        + (slotIndex * 0.25f);

                    targetBrickPos.y =
                        static_cast<float>(hit.block->y)
                        + 0.5f
                        + 0.0625f;

                    targetBrickPos.z =
                        static_cast<float>(hit.block->z)
                        + 0.5f
                        - 0.0625f;

                    // Find a brick stored at that exact slot
                    for (int i = 0;
                        i < static_cast<int>(placedBricks.size());
                        ++i)
                    {
                        if (glm::distance(
                            placedBricks[i].position,
                            targetBrickPos
                        ) < 0.001f)
                        {
                            placedBricks.erase(
                                placedBricks.begin() + i
                            );

                            std::cout
                                << "Removed brick from slot "
                                << slotIndex
                                << std::endl;

                            break;
                        }
                    }
                }
            }


            //else if (m_buildMode == BuildMode::Brick)
            //{
            //    if (hit.hit)
            //    {
            //        float closestDistance = 0.20f;
            //        int closestBrick = -1;

            //        for (int i = 0;
            //            i < static_cast<int>(placedBricks.size());
            //            ++i)
            //        {
            //            float distance =
            //                glm::distance(
            //                    placedBricks[i].position,
            //                    hit.hitPosition
            //                );

            //            if (distance < closestDistance)
            //            {
            //                closestDistance = distance;
            //                closestBrick = i;
            //            }
            //        }

            //        if (closestBrick != -1)
            //        {
            //            placedBricks.erase(
            //                placedBricks.begin() +
            //                closestBrick
            //            );

            //            std::cout << "Removed brick" << std::endl;
            //        }
            //    }
            //}
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
                

                // Mouse hit position relative to centre of ground block
                float localX = hit.hitPosition.x - static_cast<float>(hit.block->x);

                // Convert block space:
                //
                // -0.5 ---------------- +0.5
                //
                // into:
                //
                //  0.0 ---------------- 1.0

                float edgeX = localX + 0.5f;

                // Four slots across one block
                int slotIndex =
                    static_cast<int>(
                        edgeX / 0.25f
                        );

                // Keep it safely between 0 and 3
                slotIndex = std::clamp(slotIndex,
                        0,
                        3
                    );

                glm::vec3 brickPos;

                brickPos.x =
                    static_cast<float>(hit.block->x)
                    - 0.5f
                    + 0.125f
                    + (slotIndex * 0.25f);

                brickPos.y =
                    static_cast<float>(hit.block->y)
                    + 0.5f
                    + 0.0625f;

                brickPos.z =
                    static_cast<float>(hit.block->z)
                    + 0.5f
                    - 0.0625f;

                PlacedBrick newBrick;

                newBrick.position = brickPos;
                newBrick.rotationY = 0.0f;
                newBrick.active = true;

                bool occupied = false;

                for (const PlacedBrick& brick : placedBricks)
                {
                    if (glm::distance(
                        brick.position,
                        brickPos) < 0.001f)
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
                    << "Hit X: "
                    << hit.hitPosition.x
                    << "  Block X: "
                    << hit.block->x
                    << "  Local X: "
                    << localX
                    << "  Slot: "
                    << slotIndex
                    << std::endl;

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