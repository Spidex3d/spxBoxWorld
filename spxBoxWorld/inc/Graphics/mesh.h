#pragma once
#include <glad/glad.h>

class Mesh
{
public:

	Mesh();
	~Mesh();

	
	void CreateCube();
	//void CreatePlane();

	
	void RenderCube();
	//void RenderPlane();

	void Destroy();

private:

	GLuint m_cubeVAO = 0;
	GLuint m_cubeVBO = 0;

	GLuint m_planeVAO = 0;
	GLuint m_planeVBO = 0;

};
