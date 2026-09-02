#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <Core\window.h>
#include "Core/spxBoxWorld.h"
#include  <Core\input.h>  
#include <Graphics/render.h>
#include <Scene/camera.h>
#include <iostream>
#include <boxLog.h>






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

    while (!m_window->ShouldClose())
    {
        float currentFrame = static_cast<float>(glfwGetTime());

        float deltaTime = currentFrame - lastFrame;

        lastFrame = currentFrame;

        m_window->PollEvents();

        Input::ProcessKeyboard(m_window->GetNativeWindow(), *m_camera, deltaTime);

        m_renderer->RenderFrame(*m_camera);




        // Update game
        
        
       

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
