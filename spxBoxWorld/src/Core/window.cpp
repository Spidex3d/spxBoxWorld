#include <glad/glad.h>  
#include <GLFW/glfw3.h>
#include "Core/window.h"
#include <iostream>

bool Window::Create(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cout << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    m_window = glfwCreateWindow(width, height, title, nullptr, nullptr);

    if (!m_window)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    glfwMakeContextCurrent(m_window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;

        glfwDestroyWindow(m_window);
        m_window = nullptr;
        glfwTerminate();

        return false;
    }

    return true;
}
void Window::PollEvents()
{
    glfwPollEvents();
}

void Window::SwapBuffers()
{
    glfwSwapBuffers(m_window);
}

bool Window::ShouldClose() const
{
    return glfwWindowShouldClose(m_window);
}

void Window::Destroy()
{
    if (m_window)
    {
        glfwDestroyWindow(m_window);
        m_window = nullptr;
    }

    glfwTerminate();
}