#include <glad/glad.h>
#include <Graphics\render.h>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>
#include <Scene\camera.h>
#include <Helper\helpers.h>
#include <World\world.h>
#include <Resources/mbxLoader.h>
#include <Graphics\texture.h>
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
    // --------------------------------------------
    // Load Grass MBX
    // --------------------------------------------

    MBXLoader loader;

    m_grassModel =
        std::make_unique<MBXModel>();

    std::string grassPath = helpers.GetResourcesPath("Models/Grass.mbx");

    if (!loader.Load(grassPath, *m_grassModel))
    {
        std::cout
            << "Failed to load Grass.mbx"
            << std::endl;

        return false;
    }

    // --------------------------------------------
// Debug: show triangle material assignments
// --------------------------------------------
    std::cout << std::endl;
    std::cout << "Triangle material assignments:" << std::endl;

    for (size_t i = 0;
        i < m_grassModel->triangles.size();
        ++i)
    {
        const MBXTriangle& triangle =
            m_grassModel->triangles[i];

        std::cout
            << "Triangle "
            << i
            << " -> Material "
            << triangle.materialIndex
            << std::endl;
    }

    std::cout << std::endl;

    // --------------------------------------------
    // Create MBX mesh
    // --------------------------------------------
    m_mbxMesh = std::make_unique<Mesh>();

    if (!m_mbxMesh->CreateFromMBX(
        *m_grassModel))
    {
        std::cout << "Failed to create MBX mesh" << std::endl;

        return false;
    }
	

	// top and side textures for blocks
    m_topTexture = std::make_unique<Texture>();
    m_sideTexture = std::make_unique<Texture>();

	
    // --------------------------------------------
    // Cube Mesh
    // --------------------------------------------
    m_cubeMesh = std::make_unique<Mesh>();
    m_cubeMesh->CreateCube();
    
    // --------------------------------------------
    // World
    // --------------------------------------------
    m_world = std::make_unique<World>();
    m_world->GenerateWorld(*m_shader, *m_cubeMesh);

    // --------------------------------------------
    // Texture from MBX material
    // --------------------------------------------
    if (m_grassModel->materials.size() <= 1)
    {
        std::cout
            << "Grass.mbx does not contain material 1"
            << std::endl;

        return false;
    }

    std::string topTexturePath =
        helpers.GetResourcesPath(
            "Models/" +
            m_grassModel->materials[1].baseColorMap
        );

    if (!m_topTexture->LoadFromFile(topTexturePath))
    {
        return false;
    }

    std::string sideTexturePath =
        helpers.GetResourcesPath(
            "Models/" +
            m_grassModel->materials[2].baseColorMap
        );

    if (!m_sideTexture->LoadFromFile(sideTexturePath))
    {
        return false;
    }

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
    
    glm::mat4 model = glm::mat4(1.0f);

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

   
    // --------------------------------------------
    // Sides - Material 2
    // --------------------------------------------
    m_shader->SetUniformInt("useTexture", 1);

    m_sideTexture->Bind(0);

    m_shader->SetUniformInt("baseTexture", 0);

    m_mbxMesh->RenderMBXRange(0, 24);


    // --------------------------------------------
    // Top - Material 1
    // --------------------------------------------
    m_topTexture->Bind(0);

    m_mbxMesh->RenderMBXRange(24, 6);


    // --------------------------------------------
    // Bottom - Material 0
    // --------------------------------------------
    m_shader->SetUniformInt("useTexture", 0);

    m_shader->setVec3("blockColor", glm::vec3(m_grassModel->materials[0].baseColor));

    m_mbxMesh->RenderMBXRange(30, 6);

    
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
    

    if (m_mbxMesh)
    {
        m_mbxMesh->Destroy();
        m_mbxMesh.reset();
    }

    m_grassModel.reset();
   
    m_shader.reset();
}