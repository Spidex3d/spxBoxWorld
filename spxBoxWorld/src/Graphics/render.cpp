#include <glad/glad.h>
#include <Graphics\render.h>
#include <Graphics\shader.h>
#include <Graphics\mesh.h>
#include <Scene\camera.h>
#include <Helper\helpers.h>
#include <World\world.h>
#include <Resources/mbxLoader.h>
#include <Graphics\texture.h>
#include <Resources\blockAssetManager.h>
#include <iostream>


Render::Render(){}

Render::~Render(){}

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
    // Block Asset Manager
    // --------------------------------------------

    m_blockAssets =
        std::make_unique<BlockAssetManager>();

    if (!m_blockAssets->LoadBlockAsset(
        BlockType::Grass,
        "Models/Grass.mbx"))
    {
        std::cout
            << "Failed to load Grass BlockAsset"
            << std::endl;

        return false;
    }
    // --------------------------------------------
    // Full Brick Asset
    // --------------------------------------------

    if (!m_blockAssets->LoadBlockAsset(
        BlockType::Brick,
        "Models/Brick_full.mbx"))
    {
        std::cout
            << "Failed to load Brick BlockAsset"
            << std::endl;

        return false;
    }
    // --------------------------------------------
	// Half Brick Asset
    // --------------------------------------------

    if (!m_blockAssets->LoadBlockAsset(
        BlockType::BrickHalf,
        "Models/Brick_half.mbx"))
    {
        std::cout
            << "Failed to load BrickHalf BlockAsset"
            << std::endl;

        return false;
    }

    // --------------------------------------------
    // Test GetBlockAsset
    // --------------------------------------------

    BlockAsset* grassAsset =
        m_blockAssets->GetBlockAsset(
            BlockType::Grass
        );

    if (!grassAsset)
    {
        std::cout
            << "Grass BlockAsset not found"
            << std::endl;

        return false;
    }

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

    return true;
}

void Render::RenderFrame(Camera& camera, Block* selectedBlock, const std::vector<PlacedBrick>& placedBricks, PlacedBrick* selectedBrick)
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
   
    const auto& blocks = m_world->GetBlocks();

    for (const Block& block : blocks)
    {

        if (!block.isActive)
            continue;

        glm::mat4 blockModel =
            glm::mat4(1.0f);

        blockModel = glm::translate(
            blockModel,
            glm::vec3(
                static_cast<float>(block.x),
                static_cast<float>(block.y),
                static_cast<float>(block.z)
            )
        );


        if (block.type == BlockType::Grass)
        {
            RenderGrassBlock(blockModel);
            continue;
        }


        BlockDefinition definition =
            GetBlockDefinition(block.type);

        m_shader->setMat4(
            "model",
            blockModel
        );

        m_shader->SetUniformInt(
            "useTexture",
            0
        );

        m_shader->setVec3(
            "blockColor",
            definition.color
        );

        m_cubeMesh->RenderCube();

    }
    // -------------------------------------------
	//  render a  brick 
    // -------------------------------------------
  
        for (const PlacedBrick& brick : placedBricks)
        {
            for (const PlacedBrick& brick : placedBricks)
            {
                if (!brick.active)
                    continue;

                BlockAsset* brickAsset = nullptr;

                if (brick.type == BrickType::Half)
                {
                    brickAsset =
                        m_blockAssets->GetBlockAsset(
                            BlockType::BrickHalf
                        );
                }
                else
                {
                    brickAsset =
                        m_blockAssets->GetBlockAsset(
                            BlockType::Brick
                        );
                }

                if (!brickAsset)
                    continue;

                glm::mat4 model =
                    glm::mat4(1.0f);

                model = glm::translate(
                    model,
                    brick.position
                );

                model = glm::rotate(
                    model,
                    glm::radians(brick.rotationY),
                    glm::vec3(0.0f, 1.0f, 0.0f)
                );

                model = glm::scale(
                    model,
                    brickAsset->model->scale
                );

                m_shader->setMat4(
                    "model",
                    model
                );

                for (const BlockMaterial& material :
                    brickAsset->materials)
                {
                    if (material.useTexture &&
                        material.texture)
                    {
                        m_shader->SetUniformInt(
                            "useTexture",
                            1
                        );

                        material.texture->Bind(0);

                        m_shader->SetUniformInt(
                            "baseTexture",
                            0
                        );
                    }
                    else
                    {
                        m_shader->SetUniformInt(
                            "useTexture",
                            0
                        );

                        m_shader->setVec3(
                            "blockColor",
                            material.baseColor
                        );
                    }

                    for (const BlockMaterialRange& range :
                        material.ranges)
                    {
                        brickAsset->mesh->RenderMBXRange(
                            range.startIndex,
                            range.indexCount
                        );
                    }
                }
            }


        }
   

    // --------------------------------------------
    // Highlight selected brick
    // --------------------------------------------

    if (selectedBrick)
    {
        glm::mat4 model =
            glm::mat4(1.0f);

        model = glm::translate(
            model,
            selectedBrick->position
        );

        model = glm::rotate(
            model,
            glm::radians(
                selectedBrick->rotationY
            ),
            glm::vec3(0.0f, 1.0f, 0.0f)
        );

        // Slightly larger than the brick
        model = glm::scale(
            model,
            glm::vec3(
                0.255f,
                0.130f,
                0.130f
            )
        );

        m_shader->setMat4(
            "model",
            model
        );

        m_shader->SetUniformInt(
            "useTexture",
            0
        );

        m_shader->setVec3(
            "blockColor",
            glm::vec3(
                1.0f,
                1.0f,
                0.0f
            )
        );

        glPolygonMode(
            GL_FRONT_AND_BACK,
            GL_LINE
        );

        m_cubeMesh->RenderCube();

        glPolygonMode(
            GL_FRONT_AND_BACK,
            GL_FILL
        );
    }



	// --------------------------------------------
	// Highlight selected block
	// --------------------------------------------

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

void Render::RenderGrassBlock(const glm::mat4& model)
{

    BlockAsset* asset =
        m_blockAssets->GetBlockAsset(
            BlockType::Grass
        );

    if (!asset)
        return;

    m_shader->setMat4(
        "model",
        model
    );


    for (const BlockMaterial& material :
        asset->materials)
    {
        if (material.useTexture &&
            material.texture)
        {
            m_shader->SetUniformInt(
                "useTexture",
                1
            );

            material.texture->Bind(0);

            m_shader->SetUniformInt(
                "baseTexture",
                0
            );
        }
        else
        {
            m_shader->SetUniformInt(
                "useTexture",
                0
            );

            m_shader->setVec3(
                "blockColor",
                material.baseColor
            );
        }

        for (const BlockMaterialRange& range :
            material.ranges)
        {
            asset->mesh->RenderMBXRange(
                range.startIndex,
                range.indexCount
            );
        }

       
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
    
    if (m_blockAssets)
    {
        m_blockAssets->Shutdown();
        m_blockAssets.reset();
    }
        
    m_shader.reset();
}