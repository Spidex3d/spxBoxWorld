#include "World/world.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>

World::World() {}


World::~World()
{
}



void World::GenerateWorld(
    Shader& shader, Mesh& cubeMesh, Mesh& planeMesh)
{

    m_blocks.clear();

    for (int z = 0; z < 10; ++z)
    {
        for (int x = 0; x < 10; ++x)
        {
            m_blocks.emplace_back(
                BlockType::Ground,
                x - 5,
                0,
                z - 5,
                true,
                false
            );
        }
    }

    // A few test cube blocks
    m_blocks.emplace_back(
        BlockType::Grass,
        0,
        0,
        0,
        true,
        true
    );

    m_blocks.emplace_back(
        BlockType::Stone,
        1,
        0,
        0,
        true,
        true
    );

   /* m_blocks.clear();

    for (int z = 0; z < 10; ++z)
    {
        for (int x = 0; x < 10; ++x)
        {
            m_blocks.emplace_back(
                BlockType::Grass,
                x - 5,
                -1,
                z - 5,
                true
            );
        }
    }*/

}

void World::Render(
    Shader& shader,
    Mesh& cubeMesh,
    Mesh& planeMesh)
{
    for (const Block& block : m_blocks)
    {
        if (!block.isActive)
            continue;

        glm::mat4 model = glm::mat4(1.0f);

        if (block.type == BlockType::Ground)
        {
            // Ground plane sits directly at the block position
            model = glm::translate(
                model,
                glm::vec3(
                    static_cast<float>(block.x),
                    static_cast<float>(block.y),
                    static_cast<float>(block.z)
                )
            );

            shader.setMat4("model", model);

            planeMesh.RenderPlane();
        }
        else
        {
            // Cube is centred on its origin,
            // so raise it half a block above the floor
            model = glm::translate(
                model,
                glm::vec3(
                    static_cast<float>(block.x),
                    static_cast<float>(block.y) + 1.0f,
                    static_cast<float>(block.z)
                )
            );

            shader.setMat4("model", model);

            cubeMesh.RenderCube();
        }
    }
}

void World::DestroyWorld()
{
    
}