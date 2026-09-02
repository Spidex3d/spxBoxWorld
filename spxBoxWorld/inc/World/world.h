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

    void GenerateWorld(Shader& shader, Mesh& cubeMesh, Mesh& planeMesh);

    void Render(Shader& shader, Mesh& cubeMesh, Mesh& planeMesh);

    void DestroyWorld();
private:

    std::vector<Block> m_blocks;
};
