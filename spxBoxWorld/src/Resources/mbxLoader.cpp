#include <Resources/mbxLoader.h>

#include <fstream>
#include <sstream>
#include <iostream>


static std::string Trim(const std::string& str)
{
    const size_t first = str.find_first_not_of(" \t\r\n");

    if (first == std::string::npos)
        return "";

    const size_t last = str.find_last_not_of(" \t\r\n");

    return str.substr(first, last - first + 1);
}

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

			// go through the faces and check if there is a material index specified I think

            if (materialMarker == "m")
            {
                ss >> triangle.materialIndex;
            }


            outModel.triangles.push_back(triangle);
        }

		// ------------------------------------------------
		// Material
		// ------------------------------------------------

        else if (command == "material")
        {
            if (outModel.materials.size() >= 8)
            {
                std::cout
                    << "Warning: Maximum number of materials reached."
                    << std::endl;

                continue;
            }

            MBXMaterial material;

            // Example:
            // material 1
            ss >> material.id;


            // --------------------------------------------
            // Read material block until "endmaterial"
            // --------------------------------------------
            while (std::getline(file, line))
            {
                line = Trim(line);

                if (line.empty())
                    continue;

                std::stringstream materialStream(line);

                std::string materialCommand;

                materialStream >> materialCommand;


                // ----------------------------------------
                // End of material
                // ----------------------------------------
                if (materialCommand == "endmaterial")
                {
                    break;
                }


                // ----------------------------------------
                // Name
                // ----------------------------------------
                if (materialCommand == "name")
                {
                    std::string value;

                    std::getline(
                        materialStream,
                        value
                    );

                    material.name =
                        Trim(value);
                }


                // ----------------------------------------
                // Base colour
                // ----------------------------------------
                else if (materialCommand == "base_color")
                {
                    materialStream
                        >> material.baseColor.r
                        >> material.baseColor.g
                        >> material.baseColor.b
                        >> material.baseColor.a;
                }


                // ----------------------------------------
                // Metallic
                // ----------------------------------------
                else if (materialCommand == "metallic")
                {
                    materialStream
                        >> material.metallic;
                }


                // ----------------------------------------
                // Roughness
                // ----------------------------------------
                else if (materialCommand == "roughness")
                {
                    materialStream
                        >> material.roughness;
                }


                // ----------------------------------------
                // Alpha
                // ----------------------------------------
                else if (materialCommand == "alpha")
                {
                    materialStream
                        >> material.alpha;
                }


                // ----------------------------------------
                // Emission colour
                // ----------------------------------------
                else if (materialCommand == "emission_color")
                {
                    materialStream
                        >> material.emissionColor.r
                        >> material.emissionColor.g
                        >> material.emissionColor.b;
                }


                // ----------------------------------------
                // Emission strength
                // ----------------------------------------
                else if (materialCommand == "emission_strength")
                {
                    materialStream
                        >> material.emissionStrength;
                }


                // ----------------------------------------
                // Base colour texture
                // ----------------------------------------
                else if (materialCommand == "base_color_map")
                {
                    std::string value;

                    std::getline(
                        materialStream,
                        value
                    );

                    material.baseColorMap =
                        Trim(value);
                }
            }


            outModel.materials.push_back(
                material
            );


            std::cout
                << "Material "
                << material.id
                << ": "
                << material.name
                << std::endl;

            if (!material.baseColorMap.empty())
            {
                std::cout
                    << "  Texture: "
                    << material.baseColorMap
                    << std::endl;
            }
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


    std::cout
        << "Materials: "
        << outModel.materials.size()
        << std::endl;

    for (const MBXMaterial& material :
        outModel.materials)
    {
        std::cout
            << "Material "
            << material.id
            << ": "
            << material.name
            << std::endl;

        std::cout
            << "  Base Color: "
            << material.baseColor.r << ", "
            << material.baseColor.g << ", "
            << material.baseColor.b << ", "
            << material.baseColor.a
            << std::endl;

        if (!material.baseColorMap.empty())
        {
            std::cout
                << "  Texture: "
                << material.baseColorMap
                << std::endl;
        }
    }

    return true;
}