#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core\window.h>
#include "Core/spxBoxWorld.h"
#include  <Core\input.h>  
#include <Graphics/render.h>
#include <Scene/camera.h>
#include <iostream>
#include <boxLog.h>

#include <World/blockRaycast.h>
#include <World/world.h>






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

    while (!m_window->ShouldClose())
    {
        float currentFrame =
            static_cast<float>(glfwGetTime());

        float deltaTime =
            currentFrame - lastFrame;

        lastFrame = currentFrame;

        m_window->PollEvents();

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

        Block* selectedBlock =
            hit.hit ? hit.block : nullptr;


        // ------------------------------------------------
        // Remove block - Left Mouse
        // ------------------------------------------------
        bool leftMouse =
            glfwGetMouseButton(
                m_window->GetNativeWindow(),
                GLFW_MOUSE_BUTTON_LEFT
            ) == GLFW_PRESS;

        if (leftMouse &&
            !lastLeftMouse &&
            hit.hit &&
            hit.block)
        {
            m_renderer->GetWorld()->RemoveBlock(
                hit.block->x,
                hit.block->y,
                hit.block->z
            );

            // Selected pointer now refers to a block
            // that has just been made inactive.
            selectedBlock = nullptr;
        }

        lastLeftMouse = leftMouse;


        // ------------------------------------------------
        // Add block - Right Mouse
        // ------------------------------------------------
        
        
        
        bool rightMouse =
            glfwGetMouseButton(
                m_window->GetNativeWindow(),
                GLFW_MOUSE_BUTTON_RIGHT
            ) == GLFW_PRESS;

        if (rightMouse &&
            !lastRightMouse &&
            hit.hit &&
            hit.block)
        {
            int newX = hit.placePosition.x;
            int newY = hit.placePosition.y;
            int newZ = hit.placePosition.z;

            bool added =
                m_renderer->GetWorld()->AddBlock(
                    BlockType::Stone,
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

       /* if (rightMouse &&
            !lastRightMouse &&
            hit.hit &&
            hit.block)
        {
            int newX =
                hit.block->x + hit.normal.x;

            int newY =
                hit.block->y + hit.normal.y;

            int newZ =
                hit.block->z + hit.normal.z;

            m_renderer->GetWorld()->AddBlock(
                BlockType::Stone,
                newX,
                newY,
                newZ
            );
        }*/

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
            selectedBlock
        );

        m_window->SwapBuffers();
    }
}


//void spxBoxWorld::Run()
//{
//    float lastFrame = 0.0f;
//    bool lastLeftMouse = false;
//    bool lastRightMouse = false;
//
//    while (!m_window->ShouldClose())
//    {
//        float currentFrame = static_cast<float>(glfwGetTime());
//
//        float deltaTime = currentFrame - lastFrame;
//
//        lastFrame = currentFrame;
//
//        m_window->PollEvents();
//
//        Input::ProcessKeyboard(m_window->GetNativeWindow(), *m_camera, deltaTime);
//
//        double mouseX;
//        double mouseY;
//
//        glfwGetCursorPos(m_window->GetNativeWindow(), &mouseX,&mouseY);
//
//
//        RaycastHit hit =
//            BlockRaycast::CastFromMouse(
//                *m_renderer->GetWorld(),
//                *m_camera,
//                mouseX,
//                mouseY,
//                1280,
//                720
//            );
//
//        Block* selectedBlock = hit.hit ? hit.block : nullptr;
//
//        static Block* lastSelected = nullptr;
//       
//
//        m_renderer->RenderFrame(*m_camera, selectedBlock);
//
//        // #####
//        bool leftMouse =
//            glfwGetMouseButton(
//                m_window->GetNativeWindow(),
//                GLFW_MOUSE_BUTTON_LEFT
//            ) == GLFW_PRESS;
//
//        if (leftMouse &&
//            !lastLeftMouse &&
//            hit.hit &&
//            hit.block)
//        {
//            m_renderer->GetWorld()->RemoveBlock(hit.block->x, hit.block->y, hit.block->z);
//        }
//
//        lastLeftMouse = leftMouse;
//
//        bool rightMouse =
//            glfwGetMouseButton(
//                m_window->GetNativeWindow(),
//                GLFW_MOUSE_BUTTON_RIGHT
//            ) == GLFW_PRESS;
//
//        if (rightMouse &&
//            !lastRightMouse &&
//            hit.hit &&
//            hit.block)
//        {
//            int newX =
//                hit.block->x + hit.normal.x;
//
//            int newY =
//                hit.block->y + hit.normal.y;
//
//            int newZ =
//                hit.block->z + hit.normal.z;
//
//            m_renderer->GetWorld()->AddBlock(BlockType::Stone, newX, newY, newZ);
//        }
//
//        lastRightMouse = rightMouse;
//
//
//       // #####
//
//        if (selectedBlock != lastSelected)
//        {
//            lastSelected = selectedBlock;
//
//            if (selectedBlock)
//            {
//                std::cout
//                    << "Selected block: "
//                    << selectedBlock->x << ", "
//                    << selectedBlock->y << ", "
//                    << selectedBlock->z
//                    << std::endl;
//            }
//        }
//        // Update game
//                      
//
//        m_window->SwapBuffers();
//    }
//
//	
//}

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
