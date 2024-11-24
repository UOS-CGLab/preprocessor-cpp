#include "get_extraordinary.h"
#include <vector>
#include <string>
#include <fstream>
#include "MyTraits.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;


// Function to add a vertex to the visited list
int add_vertex(MyMesh &mesh, MyMesh::VertexHandle v, int idx)
{
    if (!mesh.data(v).visited)
    {
        mesh.data(v).visited = true;
        // Optionally store vertex id and position
        idx++;
    }
    return idx;
}

// Function to add a face to the visited list and its vertices
int add_face(MyMesh &mesh, MyMesh::FaceHandle f, int idx)
{
    if (!mesh.data(f).visited)
    {
        mesh.data(f).visited = true;
        for (auto fv_it = mesh.fv_iter(f); fv_it.is_valid(); ++fv_it)
        {
            idx = add_vertex(mesh, *fv_it, idx);
        }
    }
    return idx;
}


// Main function to process extraordinary vertices (equivalent to get_extraordinary in Python)
void get_extraordinary(MyMesh &mesh)
{
    int idx = 0;

    for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        if (mesh.valence(*f_it) != 4)
        {
            for (auto fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
            {
                if (!mesh.data(*fv_it).todo)
                {
                    mesh.data(*fv_it).todo = true;
                }
            }
        }
    }

    // Second pass: Add faces around extraordinary vertices
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.is_boundary(*v_it))
            continue;

        if (mesh.valence(*v_it) != 4 || mesh.data(*v_it).todo)
        {
            for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
            {
                idx = add_face(mesh, *vf_it, idx);
            }
        }
    }

    // Third pass: Mark adjacent vertices
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.data(*v_it).visited)
        {
            mesh.data(*v_it).visited2 = true;
        }
    }

    // Fourth pass: Add faces around newly visited vertices
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.data(*v_it).visited2)
        {
            for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
            {
                idx = add_face(mesh, *vf_it, idx);
            }
        }
    }

    // Mark interior vertices and faces
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.data(*v_it).visited)
        {
            mesh.data(*v_it).visited3 = true;
            mesh.data(*v_it).interior = true;
        }
    }

    for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        if (mesh.data(*f_it).visited)
        {
            mesh.data(*f_it).interior = true;
        }
    }

    // Final pass: Add faces around visited3 vertices
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it)
    {
        if (mesh.data(*v_it).visited3)
        {
            for (auto vf_it = mesh.vf_iter(*v_it); vf_it.is_valid(); ++vf_it)
            {
                idx = add_face(mesh, *vf_it, idx);
            }
        }
    }
}

void write_extraordinary_points(MyMesh &mesh, const std::string &output_dir, int idx, int depth) {
    std::ofstream file(output_dir + "/extra_ordinary" + std::to_string(depth) + ".txt");
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
            std::string v0_texcoords_data;
            for (const auto &texcoord : texcoords[0])
            {
                v0_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
            }
            v0_texcoords_data = v0_texcoords_data.substr(0, v0_texcoords_data.size() - 2);

            std::string v1_texcoords_data;
            for (const auto &texcoord : texcoords[1])
            {
                v1_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
            }
            v1_texcoords_data = v1_texcoords_data.substr(0, v1_texcoords_data.size() - 2);

            std::string v2_texcoords_data;
            for (const auto &texcoord : texcoords[2])
            {
                v2_texcoords_data += std::to_string(texcoord[0]) + ", " + std::to_string(texcoord[1]) + ", ";
            }
            v2_texcoords_data = v2_texcoords_data.substr(0, v2_texcoords_data.size() - 2);

            std::string v3_texcoords_data;
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
        file.close();
    }
    else
    {
        std::cerr << "Error: Cannot open file " << output_dir << "/extra_ordinary" << std::to_string(depth) << ".txt for writing" << std::endl;
    }
}