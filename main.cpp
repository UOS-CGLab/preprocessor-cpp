#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>

// Include OpenMesh headers
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// Include other necessary headers
#include "get_patch.h"
#include "get_extraordinary.h"
#include "get_limit_point.h"
#include "subdivision.h"
#include "obj_to_json.h"
#include "MyTraits.h"

// Namespace for filesystem (since C++17)
namespace fs = std::filesystem;

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

// Function to add a dash in the "patch.txt" file
void add_dash(const std::string &output_dir)
{
    std::ofstream file(output_dir + "/patch.txt", std::ios_base::app);
    if (file)
    {
        file << "-\n";
    }
    else
    {
        std::cerr << "Error: Cannot open patch.txt for appending" << std::endl;
    }
}

int main()
{
    std::cout << "input file: ";
    std::string input_file;

    input_file = "mesh_files/monsterfrog.obj";
    // input_file = "mesh_files/monsterfrog_subdiv1.obj";
    std::cout << input_file << std::endl;

    // Make base.json
    std::string str_name = input_file.substr(input_file.find_last_of("/\\") + 1);
    str_name = str_name.substr(0, str_name.find_last_of('.'));

    std::string output_dir = "output/" + str_name;
    if (!fs::exists(output_dir))
    {
        fs::create_directories(output_dir);
    }
    else
    {
        fs::remove_all(output_dir);
        fs::create_directories(output_dir);
    }

    obj_to_json(input_file, output_dir + "/base.json");

    // Read the mesh
    MyMesh mesh;
    OpenMesh::IO::Options opt;
    opt += OpenMesh::IO::Options::VertexTexCoord;
    opt += OpenMesh::IO::Options::FaceTexCoord;
    mesh.request_halfedge_texcoords2D();

    if (!OpenMesh::IO::read_mesh(mesh, input_file, opt))
    {
        std::cerr << "Error: Cannot read mesh from " << input_file << std::endl;
        return 1;
    }

    int origin_vertices = mesh.n_vertices();

    std::cout << "depth of subdivision: ";
    int depth; std::cout << "4" << std::endl;
    //std::cin >> depth;
    depth = 8;

    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        mesh.data(*v_it).todo = false;
        mesh.data(*v_it).visited = false;
        mesh.data(*v_it).visited2 = false;
        mesh.data(*v_it).visited3 = false;
        mesh.data(*v_it).interior = false;
    }

    for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        mesh.data(*f_it).visited = false;
        mesh.data(*f_it).interior = false;
    }

    for (auto h_it = mesh.halfedges_begin(); h_it != mesh.halfedges_end(); ++h_it)
    {
        mesh.data(*h_it).texcoord2D = mesh.texcoord2D(*h_it);
    }

    int idx = 0;
    for (int i = 0; i <= depth; ++i)
    {
        std::cout << "depth: " << i << std::endl;
        get_patch(mesh, idx, i, output_dir);
        get_extraordinary(mesh, output_dir, i);

        if (i > 0)
        {
            // Write the extraordinary points to a file
            std::ofstream file(output_dir + "/extra_ordinary" + std::to_string(i) + ".txt");
            if (file)
            {
                for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
                {
                    if (mesh.data(*f_it).patched)
                    {
                        continue;
                    }

                    std::vector<int> verts;
                    std::vector<std::vector<OpenMesh::Vec2f>> texcoords;

                    for (auto fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
                    {
                        verts.push_back((*fv_it).idx() + idx);
                        std::vector<OpenMesh::Vec2f> texcoord;
                        for (auto voh_it = mesh.voh_iter(*fv_it); voh_it.is_valid(); ++voh_it)
                        {
                            texcoord.push_back(mesh.data(mesh.opposite_halfedge_handle(*voh_it)).texcoord2D);
                        }
                        texcoords.push_back(texcoord);
                    }

                    std::vector<int> var;
                    for (auto fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
                    {
                        var.push_back(mesh.valence(*fv_it));
                    }

                    // Format and write the texture coordinates to the file
                    std::string v0_texcoords_data = "";
                    for (const auto &texcoord : texcoords[0])
                    {
                        v0_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
                    }
                    v0_texcoords_data = v0_texcoords_data.substr(0, v0_texcoords_data.size() - 2);

                    std::string v1_texcoords_data = "";
                    for (const auto &texcoord : texcoords[1])
                    {
                        v1_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
                    }
                    v1_texcoords_data = v1_texcoords_data.substr(0, v1_texcoords_data.size() - 2);

                    std::string v2_texcoords_data = "";
                    for (const auto &texcoord : texcoords[2])
                    {
                        v2_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
                    }
                    v2_texcoords_data = v2_texcoords_data.substr(0, v2_texcoords_data.size() - 2);

                    std::string v3_texcoords_data = "";
                    for (const auto &texcoord : texcoords[3])
                    {
                        v3_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
                    }
                    v3_texcoords_data = v3_texcoords_data.substr(0, v3_texcoords_data.size() - 2);

                    std::string v0_len = std::to_string(texcoords[2].size());

                    // Write vertex indices and texture coordinates
                    file << verts[0] << ", " << verts[1] << ", " << verts[3] << ", "
                        << verts[3] << ", " << verts[1] << ", " << verts[2] << ", "
                        << v0_len << ", "
                        << v0_texcoords_data << ", " << v1_texcoords_data << ", " << v3_texcoords_data << ", "
                        << v3_texcoords_data << ", " << v1_texcoords_data << ", " << v2_texcoords_data << "\n";
                }
            }
        }


        if (i == depth + 1)
        {
            break;
        }

        // Subdivide the mesh
        std::tie(mesh, idx) = subdivision(mesh, idx, i, output_dir);

//        // export the mesh to obj
//        std::string output_file = output_dir + "/subdivision" + std::to_string(i) + ".obj";
//        OpenMesh::IO::write_mesh(mesh, output_file);

        get_limit_point(mesh, output_dir, i, idx);

        add_dash(output_dir);
    }

    return 0;
}
