#include <glad/glad.h>
#include <Graphics\render.h>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>
#include <Scene\camera.h>
#include <Helper\helpers.h>
#include <World\world.h>



Render::Render()
{
}

Render::~Render()
{
}

bool Render::Initialize()
{
    Helpers helpers;

    glEnable(GL_DEPTH_TEST);

    std::string vertexShaderPath =
        helpers.GetAssetPath("Shader/basic.vert");

    std::string fragmentShaderPath =
        helpers.GetAssetPath("Shader/basic.frag");

    m_shader = std::make_unique<Shader>(
        vertexShaderPath,
        fragmentShaderPath
    );

    if (!m_shader || m_shader->ID() == 0)
    {
        return false;
    }

    // --------------------------------------------
    // Cube Mesh
    // --------------------------------------------
    m_cubeMesh = std::make_unique<Mesh>();
    m_cubeMesh->CreateCube();
    // --------------------------------------------
    // Plane Mesh
    // --------------------------------------------
     m_planeMesh = std::make_unique<Mesh>();
     m_planeMesh->CreatePlane();

    // --------------------------------------------
    // World
    // --------------------------------------------
    m_world = std::make_unique<World>();
    m_world->GenerateWorld(*m_shader, *m_cubeMesh, *m_planeMesh);
   

    return true;
}

void Render::RenderFrame(const Camera& camera)
{
    glClearColor(0.12f, 0.15f, 0.18f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->Use();

	// camera matrices
    float aspect = 1280.0f / 720.0f;

    glm::mat4 view = camera.GetViewMatrix();

    glm::mat4 projection = camera.GetProjectionMatrix(aspect);


    m_shader->setMat4("view", view);

    m_shader->setMat4("projection", projection);

    // --------------------------------------------
    // World
    // --------------------------------------------
   
    m_world->Render(*m_shader, *m_cubeMesh, *m_planeMesh);

    
}

void Render::Shutdown()
{
    if (m_world)
    {
        m_world->DestroyWorld();
        m_world.reset();
    }

    if (m_cubeMesh)
    {
        m_cubeMesh->Destroy();
        m_cubeMesh.reset();
    }
    
    m_shader.reset();
}