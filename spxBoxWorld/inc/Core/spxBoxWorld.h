#pragma once
#include <memory>
//#include <World/block.h>

class Window;
class Render;
class Camera;

class spxBoxWorld
{
public:

    spxBoxWorld();
    ~spxBoxWorld();

    bool Initialize();

    void Run();

    void Shutdown();

private:

    std::unique_ptr<Window> m_window;
    std::unique_ptr<Render> m_renderer;
    std::unique_ptr<Camera> m_camera;

    //BuildMode m_buildMode = BuildMode::Block;
};