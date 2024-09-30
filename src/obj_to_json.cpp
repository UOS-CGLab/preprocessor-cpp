#include "obj_to_json.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <nlohmann/json.hpp>

void obj_to_json(const std::string &obj_file, const std::string &json_file)
{
    // Open the OBJ file
    std::ifstream file(obj_file);
    if (!file)
    {
        std::cerr << "Error: Cannot open OBJ file " << obj_file << std::endl;
        return;
    }

    std::vector<float> vertices;

    std::string line;
    while (std::getline(file, line))
    {
        // Trim leading and trailing whitespaces
        line.erase(0, line.find_first_not_of(" \t"));
        line.erase(line.find_last_not_of(" \t") + 1);

        // Check if line starts with 'v '
        if (line.rfind("v ", 0) == 0) // line starts with "v "
        {
            // Split the line into parts
            std::istringstream iss(line);
            std::string prefix;
            float x, y, z;

            iss >> prefix >> x >> y >> z;

            // Append the vertex coordinates and a 0.0 to the vertices vector
            vertices.push_back(x);
            vertices.push_back(y);
            vertices.push_back(z);
            vertices.push_back(0.0f);
        }
    }

    file.close();

    // Create JSON object
    nlohmann::json data;
    data["Base_Vertex"] = vertices;

    // Write to JSON file
    std::ofstream json_out(json_file);
    if (!json_out)
    {
        std::cerr << "Error: Cannot open JSON file " << json_file << " for writing" << std::endl;
        return;
    }

    json_out << data.dump(4); // indent = 4 spaces

    json_out.close();
}
