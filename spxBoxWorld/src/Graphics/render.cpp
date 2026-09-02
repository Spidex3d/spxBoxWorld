#include <glad/glad.h>
#include <Graphics\render.h>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>
#include <Scene\camera.h>
#include <Helper\helpers.h>
#include <glm/gtc/matrix_transform.hpp>



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
	    
    m_testMesh = std::make_unique<Mesh>();
    m_testMesh->CreateCube();

	return true;
}
void Render::RenderFrame(const Camera& camera)
{
    glClearColor(0.12f, 0.15f, 0.18f, 1.0f);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    m_shader->Use();

	// camera matrices
    float aspect =
        1280.0f / 720.0f;

    glm::mat4 model = glm::mat4(1.0f);

    glm::mat4 view = camera.GetViewMatrix();

    glm::mat4 projection = camera.GetProjectionMatrix(aspect);

    m_shader->setMat4("model", model);
    m_shader->setMat4("view", view);
    m_shader->setMat4("projection", projection);

    // ------------------------------------------------
    // Temporary 10 x 10 block floor
    // ------------------------------------------------
    for (int z = 0; z < 10; ++z)
    {
        for (int x = 0; x < 10; ++x)
        {
            glm::mat4 model =
                glm::mat4(1.0f);

            model = glm::translate(
                model,
                glm::vec3(
                    static_cast<float>(x) - 5.0f,
                    -1.0f,
                    static_cast<float>(z) - 5.0f
                )
            );

            model = glm::scale(model, glm::vec3(0.98f));

            m_shader->setMat4("model", model);

            m_testMesh->RenderCube();
        }
    }


	// Render the cube
	//m_testMesh->RenderCube();
}
void Render::Shutdown()
{
    
    m_shader.reset();
}