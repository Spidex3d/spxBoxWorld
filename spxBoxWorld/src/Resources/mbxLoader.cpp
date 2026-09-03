#include <Resources/mbxLoader.h>

#include <fstream>
#include <sstream>
#include <iostream>


bool MBXLoader::Load(
    const std::string& filePath,
    MBXModel& outModel)
{
    std::ifstream file(filePath);

    if (!file.is_open())
    {
        std::cout
            << "Failed to open MBX file: "
            << filePath
            << std::endl;

        return false;
    }


    std::vector<glm::vec3> positions;
    std::vector<glm::vec3> normals;
    std::vector<glm::vec2> uvs;


    std::string line;


    while (std::getline(file, line))
    {
        if (line.empty())
            continue;


        std::stringstream ss(line);

        std::string command;

        ss >> command;


        // ------------------------------------------------
        // Object name
        // ------------------------------------------------
        if (command == "o")
        {
            ss >> outModel.name;
        }


        // ------------------------------------------------
        // Position
        // ------------------------------------------------
        else if (command == "v")
        {
            glm::vec3 position;

            ss
                >> position.x
                >> position.y
                >> position.z;

            positions.push_back(position);
        }


        // ------------------------------------------------
        // Normal
        // ------------------------------------------------
        else if (command == "vn")
        {
            glm::vec3 normal;

            ss
                >> normal.x
                >> normal.y
                >> normal.z;

            normals.push_back(normal);
        }


        // ------------------------------------------------
        // UV
        // ------------------------------------------------
        else if (command == "vuv")
        {
            glm::vec2 uv;

            ss
                >> uv.x
                >> uv.y;

            uvs.push_back(uv);
        }


        // ------------------------------------------------
        // Face
        // ------------------------------------------------
        else if (command == "f")
        {
            std::string vertexData[3];

            ss
                >> vertexData[0]
                >> vertexData[1]
                >> vertexData[2];


            MBXTriangle triangle;


            for (int i = 0; i < 3; ++i)
            {
                std::stringstream vertexStream(
                    vertexData[i]
                );

                std::string positionIndex;
                std::string uvIndex;
                std::string normalIndex;


                std::getline(
                    vertexStream,
                    positionIndex,
                    '/'
                );

                std::getline(
                    vertexStream,
                    uvIndex,
                    '/'
                );

                std::getline(
                    vertexStream,
                    normalIndex,
                    '/'
                );


                // MBX indices start at 1.
                // C++ vectors start at 0.
                triangle.indices[i] =
                    static_cast<unsigned int>(
                        std::stoi(positionIndex) - 1
                        );
            }


            std::string materialMarker;

            ss >> materialMarker;

            if (materialMarker == "m")
            {
                ss >> triangle.materialIndex;
            }


            outModel.triangles.push_back(
                triangle
            );
        }
    }


    // ------------------------------------------------
    // Build vertex array
    // ------------------------------------------------

    outModel.vertices.resize(
        positions.size()
    );


    for (size_t i = 0;
        i < positions.size();
        ++i)
    {
        outModel.vertices[i].position =
            positions[i];


        if (i < normals.size())
        {
            outModel.vertices[i].normal =
                normals[i];
        }


        if (i < uvs.size())
        {
            outModel.vertices[i].uv =
                uvs[i];
        }
    }


    std::cout
        << "Loaded MBX: "
        << outModel.name
        << std::endl;

    std::cout
        << "Vertices: "
        << outModel.vertices.size()
        << std::endl;

    std::cout
        << "Triangles: "
        << outModel.triangles.size()
        << std::endl;


    return true;
}