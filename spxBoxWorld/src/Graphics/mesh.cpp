#include "Graphics/mesh.h"
#include <glad/glad.h>


Mesh::Mesh(){}
Mesh::~Mesh(){ Destroy(); }


void Mesh::CreateCube()
{
    float vertices[] =
    {
        // back face
        -0.5f, -0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,
         0.5f,  0.5f, -0.5f,

         0.5f,  0.5f, -0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        // front face
        -0.5f, -0.5f,  0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,
        -0.5f, -0.5f,  0.5f,

        // left face
        -0.5f,  0.5f,  0.5f,
        -0.5f,  0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f,  0.5f,
        -0.5f,  0.5f,  0.5f,

        // right face
         0.5f,  0.5f,  0.5f,
         0.5f,  0.5f, -0.5f,
         0.5f, -0.5f, -0.5f,

         0.5f, -0.5f, -0.5f,
         0.5f, -0.5f,  0.5f,
         0.5f,  0.5f,  0.5f,

         // bottom face
         -0.5f, -0.5f, -0.5f,
          0.5f, -0.5f, -0.5f,
          0.5f, -0.5f,  0.5f,

          0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f,  0.5f,
         -0.5f, -0.5f, -0.5f,

         // top face
         -0.5f,  0.5f, -0.5f,
          0.5f,  0.5f, -0.5f,
          0.5f,  0.5f,  0.5f,

          0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f,  0.5f,
         -0.5f,  0.5f, -0.5f
    };

    glGenVertexArrays(1, &m_cubeVAO);
    glGenBuffers(1, &m_cubeVBO);

    glBindVertexArray(m_cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, m_cubeVBO);

    glBufferData(
        GL_ARRAY_BUFFER,
        sizeof(vertices),
        vertices,
        GL_STATIC_DRAW
    );

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        3 * sizeof(float),
        (void*)0
    );

    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}



void Mesh::RenderCube()
{
    glBindVertexArray(m_cubeVAO);

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

//void Mesh::CreatePlane()
//{
//    float vertices[] =
//    {
//		// ground plane
//        -0.5f,  0.5f, -0.5f,
//         0.5f,  0.5f, -0.5f,
//         0.5f,  0.5f,  0.5f,
//
//         0.5f,  0.5f,  0.5f,
//        -0.5f,  0.5f,  0.5f,
//        -0.5f,  0.5f, -0.5f
//
//    };
//    glGenVertexArrays(1, &m_planeVAO);
//    glGenBuffers(1, &m_planeVBO);
//
//    glBindVertexArray(m_planeVAO);
//
//    glBindBuffer(GL_ARRAY_BUFFER, m_planeVBO);
//
//    glBufferData(
//        GL_ARRAY_BUFFER,
//        sizeof(vertices),
//        vertices,
//        GL_STATIC_DRAW
//    );
//
//    glVertexAttribPointer(
//        0,
//        3,
//        GL_FLOAT,
//        GL_FALSE,
//        3 * sizeof(float),
//        (void*)0
//    );
//
//    glEnableVertexAttribArray(0);
//
//    glBindBuffer(GL_ARRAY_BUFFER, 0);
//    glBindVertexArray(0);
//}

//void Mesh::RenderPlane()
//{
//    glBindVertexArray(m_planeVAO);
//
//    glDrawArrays(GL_TRIANGLES, 0, 6);
//
//    glBindVertexArray(0);
//}




void Mesh::Destroy()
{
    if (m_planeVBO)
    {
        glDeleteBuffers(1, &m_planeVBO);
        m_planeVBO = 0;
    }

    if (m_cubeVBO)
    {
        glDeleteBuffers(1, &m_cubeVBO);
        m_cubeVBO = 0;
    }

    if (m_cubeVAO)
    {
        glDeleteVertexArrays(1, &m_cubeVAO);
        m_cubeVAO = 0;
    }
}