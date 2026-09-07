#pragma once
#include <memory>
#include <glm/glm.hpp>
#include <vector>
#include <World/placeBricks.h>	

class Shader;
class Mesh;
class Camera;
class World;
class Block;
class Texture;
class BlockAssetManager;
class MBXModel;

class Render
{
	public:
		Render();
		~Render();
		bool Initialize();
		//void RenderFrame(const Camera& camera, const Block* selectedBlock);
		void RenderFrame(Camera& camera, Block* selectedBlock, const std::vector<PlacedBrick>& placedBricks);

		void RenderGrassBlock(const glm::mat4& model);

		void Shutdown();


		World* GetWorld()
		{
			return m_world.get();
		}

private:
	
	// Add any private members or methods needed for rendering
	std::unique_ptr<Shader> m_shader;


	std::unique_ptr<Mesh> m_cubeMesh;

	std::unique_ptr<World> m_world;

	std::unique_ptr<BlockAssetManager> m_blockAssets;
};

