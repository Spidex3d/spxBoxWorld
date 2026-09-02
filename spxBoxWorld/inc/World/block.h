#pragma once

enum class BlockType
{
    Air = 0,
    Ground,
    Grass,
    Dirt,
    Stone
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
