#pragma once
#include <memory>

class Shader;
class Mesh;
class Camera;
class World;

class Render
{
	public:
		Render();
		~Render();
		bool Initialize();
		void RenderFrame(const Camera& camera);
		void Shutdown();


private:
	//std::unique_ptr<Camera> m_camera;
	// Add any private members or methods needed for rendering
	std::unique_ptr<Shader> m_shader;

	std::unique_ptr<Mesh> m_cubeMesh;

	std::unique_ptr<Mesh> m_planeMesh;

	std::unique_ptr<World> m_world;

	//std::unique_ptr<Mesh> m_testMesh;
};
