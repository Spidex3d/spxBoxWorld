#include "Core/input.h"
#include <GLFW/glfw3.h>
#include <Scene/camera.h>
//#include <Core\spxBoxWorld.h>


Camera* Input::s_camera = nullptr;

float Input::s_lastX = 0.0f;
float Input::s_lastY = 0.0f;

bool Input::s_firstMouse = true;

void Input::ProcessKeyboard(GLFWwindow* window, Camera& camera, float deltaTime)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(FORWARD, deltaTime);
       
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(BACKWARD, deltaTime);
        
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(LEFT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(RIGHT, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(DOWN, deltaTime);
    }

    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    {
        camera.ProcessKeyboard(UP, deltaTime);
    }
	


    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    {
        glfwSetWindowShouldClose(window, true);
    }
}

void Input::SetupMouse(GLFWwindow* window, Camera* camera)
{
    s_camera = camera;

    s_firstMouse = true;

    glfwSetCursorPosCallback(
        window,
        MouseCallback
    );

    // Capture and hide the mouse cursor
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

void Input::MouseCallback(GLFWwindow* window, double xpos, double ypos)
{

    if (!s_camera)
        return;

    // Only rotate while middle mouse button is held
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) != GLFW_PRESS)
    {
        s_firstMouse = true;
        return;
    }

    if (s_firstMouse)
    {
        s_lastX = static_cast<float>(xpos);
        s_lastY = static_cast<float>(ypos);

        s_firstMouse = false;
        return;
    }

    float xOffset =
        static_cast<float>(xpos) - s_lastX;

    float yOffset =
        s_lastY - static_cast<float>(ypos);

    s_lastX = static_cast<float>(xpos);
    s_lastY = static_cast<float>(ypos);

    s_camera->ProcessMouseMovement(
        xOffset,
        yOffset
    );

}
