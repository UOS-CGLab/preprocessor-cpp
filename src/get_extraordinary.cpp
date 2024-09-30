#include "get_extraordinary.h"
#include <vector>
#include <string>

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
void get_extraordinary(MyMesh &mesh, const std::string &output_dir, int depth)
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
