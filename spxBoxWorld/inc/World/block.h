#pragma once
#include <glm\glm.hpp>

enum class BlockType
{
    Air = 0,
    Ground,
    Grass,
    Dirt,
    Stone,
    Brick,
	BrickHalf
};

struct BlockDefinition
{
    BlockType type;
    glm::vec3 color;
    bool canBuildOn;
};

struct Block
{
    BlockType type;

    int x;
    int y;
    int z;

    bool isActive;
    bool canBuildOn;

	Block(BlockType type, int x, int y, int z, bool isActive = true, bool canBuildOn = true)  
        : type(type), x(x), y(y), z(z), isActive(isActive), canBuildOn(canBuildOn) { }
    
};

inline BlockDefinition GetBlockDefinition(BlockType type)
{
    switch (type)
    {
    case BlockType::Ground:
        return {
            BlockType::Ground,
            glm::vec3(0.35f, 0.55f, 0.25f),
            false
        };

    case BlockType::Grass:
        return {
            BlockType::Grass,
            glm::vec3(0.20f, 0.70f, 0.20f),
            true
        };

    case BlockType::Dirt:
        return {
            BlockType::Dirt,
            glm::vec3(0.45f, 0.25f, 0.10f),
            true
        };

    case BlockType::Stone:
        return {
            BlockType::Stone,
            glm::vec3(0.50f, 0.50f, 0.50f),
            true
        };

    case BlockType::Air:
    default:
        return {
            BlockType::Air,
            glm::vec3(0.0f),
            false
        };
    }
}
