#pragma once
#include <glad/glad.h>

class Mesh
{
public:

	Mesh();
	~Mesh();

	
	void CreateCube();

	
	void RenderCube();

	void Destroy();

private:

	GLuint m_cubeVAO = 0;
	GLuint m_cubeVBO = 0;

	

};
