#include "Graphics/mesh.h"
#include <glad/glad.h>
#include <vector>
#include <cstddef>
#include <Resources/mbxLoader.h>


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

bool Mesh::CreateFromMBX(const MBXModel& model)
{
    
        if (model.vertices.empty() ||
            model.triangles.empty())
        {
            return false;
        }

        std::vector<unsigned int> indices;

        for (const MBXTriangle& triangle :
            model.triangles)
        {
            indices.push_back(
                triangle.indices[0]
            );

            indices.push_back(
                triangle.indices[1]
            );

            indices.push_back(
                triangle.indices[2]
            );
        }

        m_mbxIndexCount =
            static_cast<unsigned int>(
                indices.size()
                );

        glGenVertexArrays(
            1,
            &m_mbxVAO
        );

        glGenBuffers(
            1,
            &m_mbxVBO
        );

        glGenBuffers(
            1,
            &m_mbxEBO
        );

        glBindVertexArray(
            m_mbxVAO
        );

        glBindBuffer(
            GL_ARRAY_BUFFER,
            m_mbxVBO
        );

        glBufferData(
            GL_ARRAY_BUFFER,
            model.vertices.size() *
            sizeof(MBXVertex),
            model.vertices.data(),
            GL_STATIC_DRAW
        );

        glBindBuffer(
            GL_ELEMENT_ARRAY_BUFFER,
            m_mbxEBO
        );

        glBufferData(
            GL_ELEMENT_ARRAY_BUFFER,
            indices.size() *
            sizeof(unsigned int),
            indices.data(),
            GL_STATIC_DRAW
        );

        // position
        glVertexAttribPointer(
            0,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(MBXVertex),
            (void*)offsetof(
                MBXVertex,
                position
            )
        );

        glEnableVertexAttribArray(0);

        // normal
        glVertexAttribPointer(
            1,
            3,
            GL_FLOAT,
            GL_FALSE,
            sizeof(MBXVertex),
            (void*)offsetof(
                MBXVertex,
                normal
            )
        );

        glEnableVertexAttribArray(1);

        // UV
        glVertexAttribPointer(
            2,
            2,
            GL_FLOAT,
            GL_FALSE,
            sizeof(MBXVertex),
            (void*)offsetof(
                MBXVertex,
                uv
            )
        );

        glEnableVertexAttribArray(2);

        glBindVertexArray(0);

        return true;
}

void Mesh::RenderMBX()
{
    glBindVertexArray(
        m_mbxVAO
    );

    glDrawElements(
        GL_TRIANGLES,
        m_mbxIndexCount,
        GL_UNSIGNED_INT,
        nullptr
    );

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
    if (m_mbxEBO)
    {
        glDeleteBuffers(
            1,
            &m_mbxEBO
        );

        m_mbxEBO = 0;
    }

    if (m_mbxVBO)
    {
        glDeleteBuffers(
            1,
            &m_mbxVBO
        );

        m_mbxVBO = 0;
    }

    if (m_mbxVAO)
    {
        glDeleteVertexArrays(
            1,
            &m_mbxVAO
        );

        m_mbxVAO = 0;
    }

    m_mbxIndexCount = 0;

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