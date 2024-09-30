#include "get_limit_point.h"
#include <nlohmann/json.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <unordered_set>

using json = nlohmann::json;

// Function to clear the "limit_point.json" file
void clear_json(const std::string &output_dir)
{
    std::ofstream file(output_dir + "/limit_point.json");
    if (file)
    {
        file << "[]";
        file.close();
    }
    else
    {
        std::cerr << "Error: Cannot open limit_point.json for writing" << std::endl;
    }
}

// Function to append data to "limit_point.json"
void append_to_json(const json &data, const std::string &output_dir)
{
    std::string file_path = output_dir + "/limit_point.json";
    std::ifstream in_file(file_path);
    json json_data;

    if (in_file)
    {
        in_file >> json_data;
        in_file.close();
    }
    else
    {
        json_data = json::array();
    }

    json_data.push_back(data);

    std::ofstream out_file(file_path);
    if (out_file)
    {
        out_file << json_data.dump(2);
        out_file.close();
    }
    else
    {
        std::cerr << "Error: Cannot open limit_point.json for writing" << std::endl;
    }
}

// Function to get the limit points and save them to JSON
void get_limit_point(MyMesh &mesh, const std::string &output_dir, int depth, int idx)
{
    std::vector<std::vector<int>> outputs;

    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.valence(*v_it) != 4)
        {
            if (mesh.is_boundary(*v_it))
                continue;

            for (auto voh_it = mesh.voh_iter(*v_it); voh_it.is_valid(); ++voh_it)
            {
                MyMesh::VertexHandle ve = mesh.to_vertex_handle(*voh_it);
                MyMesh::VertexHandle vv = mesh.to_vertex_handle(mesh.next_halfedge_handle(*voh_it));

                std::vector<int> output;
                output.push_back(ve.idx() + idx);

                for (auto veoh_it = mesh.voh_iter(ve); veoh_it.is_valid(); ++veoh_it)
                {
                    output.push_back(mesh.to_vertex_handle(*veoh_it).idx() + idx);
                    output.push_back(mesh.to_vertex_handle(mesh.next_halfedge_handle(*veoh_it)).idx() + idx);
                }
                outputs.push_back(output);

                output.clear();
                output.push_back(vv.idx() + idx);

                for (auto vvoh_it = mesh.voh_iter(vv); vvoh_it.is_valid(); ++vvoh_it)
                {
                    output.push_back(mesh.to_vertex_handle(*vvoh_it).idx() + idx);
                    output.push_back(mesh.to_vertex_handle(mesh.next_halfedge_handle(*vvoh_it)).idx() + idx);
                }
                outputs.push_back(output);
            }
        }
    }

    json data;
    data["depth"] = depth;
    data["data"] = outputs;

    if (depth <= 1)
    {
        clear_json(output_dir);
    }
    append_to_json(data, output_dir);
}
