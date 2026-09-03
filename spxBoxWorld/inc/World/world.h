#pragma once
#include <memory>
#include <vector>
#include <World\block.h>

class Shader;
class Mesh;

class World
{
public:
    World();
    ~World();

    Block* GetBlock(int x, int y, int z);

    bool AddBlock(BlockType type, int x, int y, int z);

    bool RemoveBlock(int x, int y, int z);


    void GenerateWorld(Shader& shader, Mesh& cubeMesh);

    void Render(Shader& shader, Mesh& cubeMesh);

    void DestroyWorld();

    
private:
	bool IsBlockAt(int x, int y, int z) const;

    std::vector<Block> m_blocks;
    
};
