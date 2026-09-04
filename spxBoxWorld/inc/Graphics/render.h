#pragma once
#include <memory>

class Shader;
class Mesh;
class Camera;
class World;
class Block;
class Texture;

class MBXModel;

class Render
{
	public:
		Render();
		~Render();
		bool Initialize();
		void RenderFrame(const Camera& camera, const Block* selectedBlock);
		void Shutdown();


		World* GetWorld()
		{
			return m_world.get();
		}

private:
	
	// Add any private members or methods needed for rendering
	std::unique_ptr<Shader> m_shader;

	std::unique_ptr<Mesh> m_mbxMesh;

	std::unique_ptr<Mesh> m_cubeMesh;

	std::unique_ptr<Mesh> m_planeMesh;

	std::unique_ptr<World> m_world;

	// Keep loaded MBX data
	std::unique_ptr<MBXModel> m_grassModel; 
	// temporary test texture
	std::unique_ptr<Texture> m_testTexture;
};
