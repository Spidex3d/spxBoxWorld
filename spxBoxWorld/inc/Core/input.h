#pragma once

struct GLFWwindow;

class Camera;

class Input
{
public:

    static void ProcessKeyboard(GLFWwindow* window, Camera& camera, float deltaTime);



    static void SetupMouse(GLFWwindow* window, Camera* camera);

private:
    static void MouseCallback(GLFWwindow* window, double xpos, double ypos);

    static Camera* s_camera;

    static float s_lastX;
    static float s_lastY;

    static bool s_firstMouse;

};
