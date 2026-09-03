#include <glad/glad.h>
#include <Graphics\render.h>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>
#include <Scene\camera.h>
#include <Helper\helpers.h>
#include <World\world.h>
#include <Resources/mbxLoader.h>
#include <iostream>



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
    // ------------------------------------
    MBXLoader loader;
    MBXModel grassModel;

    std::string grassPath =
        helpers.GetResourcesPath("Models/Grass.mbx");

    if (!loader.Load(
        grassPath,
        grassModel))
    {
        std::cout << "Failed to load Grass.mbx" << std::endl;

        return false;
    }

    m_mbxMesh = std::make_unique<Mesh>();

    if (!m_mbxMesh->CreateFromMBX(
        grassModel))
    {
        std::cout
            << "Failed to create MBX mesh"
            << std::endl;

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
     //m_planeMesh = std::make_unique<Mesh>();
    // m_planeMesh->CreatePlane();

    // --------------------------------------------
    // World
    // --------------------------------------------
    m_world = std::make_unique<World>();
    m_world->GenerateWorld(*m_shader, *m_cubeMesh);
   

    return true;
}

void Render::RenderFrame(const Camera& camera, const Block* selectedBlock)
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
  // Test MBX model
  // --------------------------------------------
    glm::mat4 model =
        glm::mat4(1.0f);

    model = glm::translate(
        model,
        glm::vec3(
            0.0f,
            4.0f,
            0.0f
        )
    );

    m_shader->setMat4(
        "model",
        model
    );

    m_shader->setVec3(
        "blockColor",
        glm::vec3(
            0.2f,
            0.7f,
            0.2f
        )
    );

    m_mbxMesh->RenderMBX();






    // --------------------------------------------
    // World
    // --------------------------------------------
   
    m_world->Render(*m_shader, *m_cubeMesh);

    if (selectedBlock)
    {
        glm::mat4 model = glm::mat4(1.0f);

        model = glm::translate(
            model,
            glm::vec3(
                static_cast<float>(selectedBlock->x),
                static_cast<float>(selectedBlock->y),
                static_cast<float>(selectedBlock->z)
            )
        );

        // Slightly larger so it does not z-fight with the block
        model = glm::scale(
            model,
            glm::vec3(1.02f)
        );

        m_shader->setMat4("model", model);

        m_shader->setVec3(
            "blockColor",
            glm::vec3(1.0f, 1.0f, 1.0f)
        );

        glLineWidth(2.0f);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

        m_cubeMesh->RenderCube();

        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glLineWidth(1.0f);
    }
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