#pragma once
#include <memory>
#include <string>
#include <glm/glm.hpp>
#include <vector>
#include <unordered_map>


class Texture;
class MBXModel;
class Mesh;
enum class BlockType;   

struct BlockMaterialRange
{
    unsigned int startIndex = 0;
    unsigned int indexCount = 0;
};

struct BlockMaterial
{
    int materialIndex = 0;

    std::unique_ptr<Texture> texture;

    glm::vec3 baseColor = glm::vec3(1.0f);

    bool useTexture = false;


    std::vector<BlockMaterialRange> ranges;
    /*unsigned int startIndex = 0;
    unsigned int indexCount = 0;*/
};



struct BlockAsset
{
    BlockType type;

    std::unique_ptr<MBXModel> model;
    std::unique_ptr<Mesh> mesh;

    std::vector<BlockMaterial> materials;
};

class BlockAssetManager
{
public:

    bool LoadBlockAsset(BlockType type, const std::string& mbxFile);

    BlockAsset* GetBlockAsset(BlockType type);

    


    void Shutdown();

private:

    std::unordered_map<BlockType, std::unique_ptr<BlockAsset>> m_assets;
    
};