#pragma once
#include <glad/glad.h>

class MBXModel;

struct MBXMaterialRange
{
	int materialIndex = 0;
	unsigned int startIndex = 0;
	unsigned int indexCount = 0;
};

class Mesh
{
public:

	Mesh();
	~Mesh();

	
	void CreateCube();
	
	void RenderCube();
	
	bool CreateFromMBX(const MBXModel& model);
	void RenderMBX();

	void RenderMBXRange(unsigned int startIndex, unsigned int indexCount);

	void Destroy();

private:

	GLuint m_mbxVAO = 0;
	GLuint m_mbxVBO = 0;
	GLuint m_mbxEBO = 0;

	unsigned int m_mbxIndexCount = 0;

	GLuint m_cubeVAO = 0;
	GLuint m_cubeVBO = 0;

	GLuint m_planeVAO = 0;
	GLuint m_planeVBO = 0;

};
