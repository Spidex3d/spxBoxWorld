#include "World/world.h"
#include <glm/gtc/matrix_transform.hpp>
#include <Graphics/shader.h>
#include <Graphics/mesh.h>
#include <Graphics/texture.h>
#include <Resources/mbxLoader.h>

World::World() {}


World::~World()
{
}

Block* World::GetBlock(int x, int y, int z)
{
    
        for (Block& block : m_blocks)
        {
            if (!block.isActive)
                continue;

            if (block.x == x &&
                block.y == y &&
                block.z == z)
            {
                return &block;
            }
        }

        return nullptr;
    
}

bool World::AddBlock(BlockType type, int x, int y, int z)
{
    Block* existing =
        GetBlock(x, y, z);

    if (existing)
    {
        return false;
    }

    m_blocks.emplace_back(
        type,
        x,
        y,
        z,
        true,
        true
    );

    return true;
}

bool World::RemoveBlock(int x, int y, int z)
{
    Block* block =
        GetBlock(x, y, z);

    if (!block)
        return false;

    block->isActive = false;

    return true;
}



//void World::GenerateWorld(Shader& shader, Mesh& cubeMesh, Mesh& planeMesh)
void World::GenerateWorld(Shader& shader, Mesh& cubeMesh)
{

    m_blocks.clear();
	// Ground plane blocks
    for (int z = 0; z < 10; ++z)
    {
        for (int x = 0; x < 10; ++x)
        {
            m_blocks.emplace_back(
                BlockType::Grass,
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
        -3,
        1,
        0,
        true,
        true
    );

    m_blocks.emplace_back(
        BlockType::Stone,
        1,
        1,
        0,
        true,
        true
    );

    m_blocks.emplace_back(
        BlockType::Stone,
        0, 1, 0,
        true
    );

    m_blocks.emplace_back(
        BlockType::Dirt,
        0, 2, 0,
        true
    );

    m_blocks.emplace_back(
        BlockType::Stone,
        2, 1, 0,
        true
    );

}

void World::Render(Shader& shader, Mesh& cubeMesh)
{
    
    for (const Block& block : m_blocks)
    {
        if (!block.isActive)
            continue;

        BlockDefinition definition =
            GetBlockDefinition(block.type);

        shader.setVec3(
            "blockColor",
            definition.color
        );


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

            cubeMesh.RenderCube();
        }
        else
        {
            // Cube is centred on its origin,
            // so raise it half a block above the floor
            model = glm::translate(
                model,
                glm::vec3(
                    static_cast<float>(block.x),
					static_cast<float>(block.y), // + 0.5f
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