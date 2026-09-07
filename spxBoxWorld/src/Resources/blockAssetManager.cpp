#include <Resources\blockAssetManager.h>
#include <Graphics/texture.h>
#include <Graphics\mesh.h>
#include <Resources/mbxLoader.h>
#include <Helper/helpers.h>
#include <World/block.h>

#include <iostream>
#include <filesystem>

bool BlockAssetManager::LoadBlockAsset(BlockType type, const std::string& mbxFile)
{
    Helpers helpers;
    MBXLoader loader;


    // --------------------------------------------
    // Create new block asset
    // --------------------------------------------
    auto asset = std::make_unique<BlockAsset>();

    asset->type = type;

    asset->model = std::make_unique<MBXModel>();

    asset->mesh = std::make_unique<Mesh>();


    // --------------------------------------------
    // Load MBX file
    // --------------------------------------------
    std::string mbxPath =
        helpers.GetResourcesPath(
            mbxFile
        );

    if (!loader.Load(
        mbxPath,
        *asset->model))
    {
        std::cout
            << "Failed to load block asset: "
            << mbxFile
            << std::endl;

        return false;
    }


    // --------------------------------------------
    // Create OpenGL mesh
    // --------------------------------------------
    if (!asset->mesh->CreateFromMBX(
        *asset->model))
    {
        std::cout
            << "Failed to create block mesh: "
            << mbxFile
            << std::endl;

        return false;
    }


    // --------------------------------------------
    // Find folder containing the MBX file
    //
    // Example:
    // Models/Grass.mbx
    //
    // becomes:
    // Models
    // --------------------------------------------
    std::filesystem::path mbxRelativePath(mbxFile);

    std::string modelFolder = mbxRelativePath.parent_path().string();


    // --------------------------------------------
    // Build material draw ranges
    // --------------------------------------------
    for (size_t materialIndex = 0;
        materialIndex <
        asset->model->materials.size();
        ++materialIndex)
    {
        const MBXMaterial& mbxMaterial =
            asset->model->materials[
                materialIndex
            ];


        BlockMaterial blockMaterial;

        blockMaterial.materialIndex =
            static_cast<int>(
                materialIndex
                );

        blockMaterial.baseColor =
            glm::vec3(
                mbxMaterial.baseColor
            );


        // ----------------------------------------
        // Find triangles using this material
        // ----------------------------------------
        
        int rangeStart = -1;
        unsigned int rangeTriangleCount = 0;

        for (size_t triangleIndex = 0;
            triangleIndex < asset->model->triangles.size();
            ++triangleIndex)
        {
            const MBXTriangle& triangle =
                asset->model->triangles[triangleIndex];

            bool usesMaterial =
                triangle.materialIndex ==
                static_cast<int>(materialIndex);

            if (usesMaterial)
            {
                if (rangeStart == -1)
                {
                    rangeStart =
                        static_cast<int>(triangleIndex);

                    rangeTriangleCount = 0;
                }

                ++rangeTriangleCount;
            }

            // End of a continuous material range
            if ((!usesMaterial ||
                triangleIndex ==
                asset->model->triangles.size() - 1)
                && rangeStart != -1)
            {
                BlockMaterialRange range;

                range.startIndex =
                    static_cast<unsigned int>(
                        rangeStart
                        ) * 3;

                range.indexCount =
                    rangeTriangleCount * 3;

                blockMaterial.ranges.push_back(
                    range
                );

                rangeStart = -1;
                rangeTriangleCount = 0;
            }
        }

        if (blockMaterial.ranges.empty())
        {
            continue;
        }



        // ----------------------------------------
        // Load texture if material has one
        // ----------------------------------------
        if (!mbxMaterial.baseColorMap.empty())
        {
            blockMaterial.texture =
                std::make_unique<Texture>();


            std::filesystem::path textureRelativePath;

            if (!modelFolder.empty())
            {
                textureRelativePath =
                    std::filesystem::path(
                        modelFolder
                    ) /
                    mbxMaterial.baseColorMap;
            }
            else
            {
                textureRelativePath =
                    mbxMaterial.baseColorMap;
            }


            std::string texturePath =
                helpers.GetResourcesPath(
                    textureRelativePath.string()
                );


            if (!blockMaterial.texture
                ->LoadFromFile(
                    texturePath))
            {
                std::cout
                    << "Failed to load material texture: "
                    << texturePath
                    << std::endl;

                return false;
            }


            blockMaterial.useTexture = true;
        }
        else
        {
            blockMaterial.useTexture = false;
        }


        asset->materials.push_back(
            std::move(blockMaterial)
        );
    }


    // --------------------------------------------
    // Store finished asset
    // --------------------------------------------
    m_assets[type] =
        std::move(asset);


    std::cout
        << "Loaded BlockAsset: "
        << mbxFile
        << std::endl;

    return true;
}

BlockAsset* BlockAssetManager::GetBlockAsset(BlockType type)
{
    auto it = m_assets.find(type);

    if (it == m_assets.end())
    {
        return nullptr;
    }

    return it->second.get();
}

void BlockAssetManager::Shutdown()
{
	m_assets.clear();
}