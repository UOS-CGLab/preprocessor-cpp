#include "get_patch.h"
#include <fstream>
#include <iostream>
#include <vector>
#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// Function to write data into "patch.txt"
void write_into_file(
        int v0, int v1, int v2, int v3,
        int v4, int v5, int v6, int v7,
        int v8, int v9, int v10, int v11,
        int v12, int v13, int v14, int v15,
        int idx,
        const std::vector<OpenMesh::Vec2f>& v5_texcoords,
        const std::vector<OpenMesh::Vec2f>& v6_texcoords,
        const std::vector<OpenMesh::Vec2f>& v9_texcoords,
        const std::vector<OpenMesh::Vec2f>& v10_texcoords,
        const std::string& output_dir)
{
    std::ofstream file(output_dir + "/patch.txt", std::ios_base::app);
    if (file)
    {
        file << v0 + idx << ", " << v1 + idx << ", " << v2 + idx << ", " << v3 + idx << ", "
             << v4 + idx << ", " << v5 + idx << ", " << v6 + idx << ", " << v7 + idx << ", "
             << v8 + idx << ", " << v9 + idx << ", " << v10 + idx << ", " << v11 + idx << ", "
             << v12 + idx << ", " << v13 + idx << ", " << v14 + idx << ", " << v15 + idx << ", ";

        // Write texture coordinates
        for (const auto& tex : v5_texcoords)
            file << tex[0] << ", " << tex[1] << ", ";
        for (const auto& tex : v6_texcoords)
            file << tex[0] << ", " << tex[1] << ", ";
        for (const auto& tex : v9_texcoords)
            file << tex[0] << ", " << tex[1] << ", ";
        for (const auto& tex : v10_texcoords)
            file << tex[0] << ", " << tex[1] << ", ";

        file << "\n";
    }
    else
    {
        std::cerr << "Error: Cannot open patch.txt for appending" << std::endl;
    }
}

// Main function to process the mesh and generate patches
int get_patch(MyMesh& mesh, int idx, int depth, const std::string& output_dir)
{
    int count = 0;

    for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        if (mesh.data(*f_it).patched){
            continue;
        }

        if (mesh.valence(*f_it) == 4 && !mesh.is_boundary(*f_it))
        {
            std::vector<MyMesh::FaceHandle> neighbors;
            for (auto fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
            {
                for (auto vf_it = mesh.vf_iter(*fv_it); vf_it.is_valid(); ++vf_it)
                {
                    neighbors.push_back(*vf_it);
                }
            }

            bool all_valence_4 = true;
            for (auto n : neighbors)
            {
                if (mesh.valence(n) != 4)
                {
                    all_valence_4 = false;
                    break;
                }
            }

            if (depth != 0)
            {
                bool is_boundary = false;
                for (auto n : neighbors)
                {
                    if (mesh.is_boundary(n))
                    {
                        is_boundary = true;
                        break;
                    }
                }

                if (is_boundary)
                {
                    continue;
                }
            }

            if (all_valence_4)
            {
                count++;

                std::vector<int> v_indices;
                for (auto fv_it = mesh.fv_iter(*f_it); fv_it.is_valid(); ++fv_it)
                {
                    v_indices.push_back(fv_it->idx());
                }

                bool not_extraordinary = true;
                for (auto v : v_indices)
                {
                    if (mesh.valence(mesh.vertex_handle(v)) != 4)
                    {
                        not_extraordinary = false;
                        break;
                    }
                }

                if (!not_extraordinary)
                {
                    continue;
                }

                int f0 = v_indices[0];
                int f1 = v_indices[1];
                int f2 = v_indices[2];
                int f3 = v_indices[3];

                // Compute halfedges
                auto h_top = mesh.next_halfedge_handle(mesh.next_halfedge_handle(mesh.find_halfedge(mesh.vertex_handle(f0), mesh.vertex_handle(f3))));
                auto h_left = mesh.next_halfedge_handle(mesh.next_halfedge_handle(mesh.find_halfedge(mesh.vertex_handle(f1), mesh.vertex_handle(f0))));
                auto h_bottom = mesh.next_halfedge_handle(mesh.next_halfedge_handle(mesh.find_halfedge(mesh.vertex_handle(f2), mesh.vertex_handle(f1))));
                auto h_right = mesh.next_halfedge_handle(mesh.next_halfedge_handle(mesh.find_halfedge(mesh.vertex_handle(f3), mesh.vertex_handle(f2))));

                if (depth != 0)
                {
                    if (mesh.is_boundary(mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(h_top)))) ||
                        mesh.is_boundary(mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(h_right)))) ||
                        mesh.is_boundary(mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(h_left)))) ||
                        mesh.is_boundary(mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(h_bottom)))))
                    {
                        continue;
                    }
                }

                // Compute vertices v0 to v15
                int v0 = mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(h_top)))).idx();
                int v1 = mesh.to_vertex_handle(h_top).idx();
                int v2 = mesh.from_vertex_handle(h_top).idx();
                int v3 = mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(h_right)))).idx();
                int v4 = mesh.from_vertex_handle(h_left).idx();
                int v5 = f0;
                int v6 = f3;
                int v7 = mesh.to_vertex_handle(h_right).idx();
                int v8 = mesh.to_vertex_handle(h_left).idx();
                int v9 = f1;
                int v10 = f2;
                int v11 = mesh.from_vertex_handle(h_right).idx();
                int v12 = mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(h_left)))).idx();
                int v13 = mesh.from_vertex_handle(h_bottom).idx();
                int v14 = mesh.to_vertex_handle(h_bottom).idx();
                int v15 = mesh.to_vertex_handle(mesh.next_halfedge_handle(mesh.opposite_halfedge_handle(mesh.next_halfedge_handle(h_bottom)))).idx();

                mesh.data(*f_it).patched = true;

                // Collect texture coordinates for vertices
                std::vector<OpenMesh::Vec2f> v5_texcoords, v6_texcoords, v9_texcoords, v10_texcoords;

                for (auto voh : mesh.voh_range(mesh.vertex_handle(f0)))
                {
//                    v5_texcoords.push_back(mesh.texcoord2D(mesh.opposite_halfedge_handle(voh)));
                    v5_texcoords.push_back(mesh.data(mesh.opposite_halfedge_handle(voh)).texcoord2D);
                }


                for (auto voh : mesh.voh_range(mesh.vertex_handle(f3)))
                {
                    v6_texcoords.push_back(mesh.data(mesh.opposite_halfedge_handle(voh)).texcoord2D);
                }
                for (auto voh : mesh.voh_range(mesh.vertex_handle(f1)))
                {
                    v9_texcoords.push_back(mesh.data(mesh.opposite_halfedge_handle(voh)).texcoord2D);
                }
                for (auto voh : mesh.voh_range(mesh.vertex_handle(f2)))
                {
                    v10_texcoords.push_back(mesh.data(mesh.opposite_halfedge_handle(voh)).texcoord2D);
                }

                write_into_file(
                        v0, v1, v2, v3, v4, v5, v6, v7, v8, v9, v10, v11, v12, v13, v14, v15,
                        idx, v5_texcoords, v6_texcoords, v9_texcoords, v10_texcoords, output_dir);

            }
        }
    }

    return count;
}
