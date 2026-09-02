#pragma once

struct GLFWwindow;


class Window
{
public:

    bool Create(int width, int height, const char* title);

    void PollEvents();
    void SwapBuffers();

    bool ShouldClose() const;

    void Destroy();

    GLFWwindow* GetNativeWindow() const
    {
        return m_window;
    }

private:

    GLFWwindow* m_window = nullptr;

};