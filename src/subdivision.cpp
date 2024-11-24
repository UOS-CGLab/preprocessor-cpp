#include "subdivision.h"
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <vector>
#include <string>
#include <to_json.h>

void compute_CC_vertex(MyMesh &mesh, MyMesh::VertexHandle v, OpenMesh::Vec3f &p)
{
    int k = mesh.valence(v);
    double alpha = 1.0f / (4.0f * k);
    double beta = 6.0f / (4.0f * k);
    p = (1 - alpha - beta) * mesh.point(v);

    for (auto vv_it = mesh.vv_iter(v); vv_it.is_valid(); ++vv_it)
    {
        p += (beta / k) * mesh.point(*vv_it);
    }

    for (auto voh_it = mesh.voh_iter(v); voh_it.is_valid(); ++voh_it)
    {
        auto next_vh = mesh.to_vertex_handle(mesh.next_halfedge_handle(*voh_it));
        p += (alpha / k) * mesh.point(next_vh);
    }
}

void compute_CC_face(MyMesh &mesh, MyMesh::FaceHandle f, OpenMesh::Vec3f &p)
{
    p = OpenMesh::Vec3f(0, 0, 0);
    int valence = 0;
    for (auto fv_it = mesh.fv_iter(f); fv_it.is_valid(); ++fv_it)
    {
        p += mesh.point(*fv_it);
        valence++;
    }
    p /= static_cast<float>(valence);
}

void compute_CC_edge(MyMesh &mesh, MyMesh::EdgeHandle e, OpenMesh::Vec3f &p)
{
    p = OpenMesh::Vec3f(0, 0, 0);

    OpenMesh::HalfedgeHandle h0 = mesh.halfedge_handle(e, 0);
    OpenMesh::HalfedgeHandle h1 = mesh.halfedge_handle(e, 1);

    // First halfedge
    p += 6.0f * mesh.point(mesh.to_vertex_handle(h0));
    OpenMesh::HalfedgeHandle h = mesh.next_halfedge_handle(h0);
    p += 1.0f * mesh.point(mesh.to_vertex_handle(h));
    h = mesh.next_halfedge_handle(h);
    p += 1.0f * mesh.point(mesh.to_vertex_handle(h));

    // Second halfedge
    p += 6.0f * mesh.point(mesh.to_vertex_handle(h1));
    h = mesh.next_halfedge_handle(h1);
    p += 1.0f * mesh.point(mesh.to_vertex_handle(h));
    h = mesh.next_halfedge_handle(h);
    p += 1.0f * mesh.point(mesh.to_vertex_handle(h));

    p /= 16.0f;
}

std::tuple<MyMesh, int> subdivision(MyMesh &mesh, int &prev_idx, int depth, const std::string &output_dir)
{
    // Data structures for collecting output
    std::vector<int> f_offsets;
    std::vector<int> f_valances;
    std::vector<int> f_indices;
    std::vector<int> f_data;
    std::vector<int> e_data;
    std::vector<int> e_indices;
    std::vector<int> v_offsets;
    std::vector<int> v_data;
    std::vector<int> v_index;
    std::vector<int> v_valances;
    std::vector<int> v_indices;

    MyMesh mesh_next;

    int fidx = prev_idx + mesh.n_vertices();
    int offset = 0;
    int idx = 0;

    // Face points
    for ( auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it) {

        OpenMesh::FaceHandle f = *f_it;
//        if (!mesh.data(f).visited) {
//            continue;
//        }

        // --- Compute the new face point using compute_CC_face ---
        OpenMesh::Vec3f p;
        compute_CC_face(mesh, f, p);
        // p = OpenMesh::Vec3f(0, 0, 0);
        mesh.data(f).id = idx;
        mesh.data(f).position = p;
        mesh_next.add_vertex(p);  // Add the new vertex to the next mesh

        // --- Add face indices for JSON output ---
        int val = 0;
        for (auto fv_it = mesh.fv_iter(f); fv_it.is_valid(); ++fv_it) {
            val++;
            f_data.push_back(fv_it->idx() + prev_idx);  // Store vertex indices
        }

        f_offsets.push_back(offset);
        f_valances.push_back(val);
        offset += val;
        f_indices.push_back(idx + fidx);
        idx++;

        // Skip non-interior faces
        if (!mesh.data(f).interior) {
            continue;
        }

        // Mark vertices and edges of the face as valid
        for (auto fv_it = mesh.fv_iter(f); fv_it.is_valid(); ++fv_it) {
            mesh.data(*fv_it).valid = true;
        }
        for (auto fe_it = mesh.fe_iter(f); fe_it.is_valid(); ++fe_it) {
            mesh.data(*fe_it).valid = true;
        }
    }

    // **디버깅용 출력문 추가**
//    std::cout << "Face Data Size: " << f_data.size() << "\n";
//    std::cout << "Face Indices Size: " << f_indices.size() << "\n";

    // Edge points
    for (auto e_it = mesh.edges_begin(); e_it != mesh.edges_end(); ++e_it) {
        OpenMesh::EdgeHandle e = *e_it;
        if (!mesh.data(e).valid) {
            continue;
        }

        // --- Compute the new edge point using compute_CC_edge ---
        OpenMesh::Vec3f p;
        compute_CC_edge(mesh, e, p);
        // p = OpenMesh::Vec3f(0, 0, 0);
        mesh.data(e).id = idx;
        mesh.data(e).position = p;
        mesh_next.add_vertex(p);  // Add the new vertex to the next mesh

        // Add edge data for JSON output
        OpenMesh::HalfedgeHandle h0 = mesh.halfedge_handle(e, 0);
        OpenMesh::HalfedgeHandle h1 = mesh.halfedge_handle(e, 1);
        OpenMesh::VertexHandle v0 = mesh.to_vertex_handle(h0);
        OpenMesh::VertexHandle v1 = mesh.to_vertex_handle(h1);
        OpenMesh::FaceHandle f0 = mesh.face_handle(h0);
        OpenMesh::FaceHandle f1 = mesh.face_handle(h1);

        e_data.push_back(v0.idx() + prev_idx);
        e_data.push_back(v1.idx() + prev_idx);
        e_data.push_back(f0.idx() + fidx);
        e_data.push_back(f1.idx() + fidx);

        e_indices.push_back(idx + fidx);
        idx++;
    }

    // **디버깅용 출력문 추가**
//    std::cout << "Edge Data Size: " << e_data.size() << "\n";
//    std::cout << "Edge Indices Size: " << e_indices.size() << "\n";

    // Vertex points
    offset = 0;
    for (auto v_it = mesh.vertices_begin(); v_it != mesh.vertices_end(); ++v_it) {
        OpenMesh::VertexHandle v = *v_it;
        if (!mesh.data(v).valid) {
            continue;
        }

        // --- Compute the new vertex point using compute_CC_vertex ---
        OpenMesh::Vec3f p;
        compute_CC_vertex(mesh, v, p);
        // p = OpenMesh::Vec3f(0, 0, 0);
        mesh.data(v).id = idx;
        mesh.data(v).position = p;
        mesh_next.add_vertex(p);  // Add the new vertex to the next mesh

        // Add vertex data for JSON output
        int val = 0;
        for (auto vf_it = mesh.vf_iter(v); vf_it.is_valid(); ++vf_it) {
            val++;
            v_data.push_back(vf_it->idx() + fidx);  // Store face indices
        }

        for (auto ve_it = mesh.ve_iter(v); ve_it.is_valid(); ++ve_it) {
            OpenMesh::EdgeHandle edge = *ve_it;
            OpenMesh::HalfedgeHandle h0 = mesh.halfedge_handle(edge, 0);
            OpenMesh::HalfedgeHandle h1 = mesh.halfedge_handle(edge, 1);
            OpenMesh::VertexHandle v0 = mesh.to_vertex_handle(h0);
            OpenMesh::VertexHandle v1 = mesh.to_vertex_handle(h1);
            if (v0 != v) {
                v_data.push_back(v0.idx() + prev_idx);
            }
            if (v1 != v) {
                v_data.push_back(v1.idx() + prev_idx);
            }
            val++;
        }

        v_offsets.push_back(offset);
        v_valances.push_back(val);
        offset += val;
        v_index.push_back(v.idx() + prev_idx);
        v_indices.push_back(idx + fidx);
        idx++;
    }

    // **디버깅용 출력문 추가**
//    std::cout << "Vertex Data Size: " << v_data.size() << "\n";
//    std::cout << "Vertex Indices Size: " << v_indices.size() << "\n";

    // Create new faces in mesh_next
    for (auto f_it = mesh.faces_begin(); f_it != mesh.faces_end(); ++f_it)
    {
        OpenMesh::FaceHandle f = *f_it;
        if (!mesh.data(f).interior){
            continue;
        }

        auto v0 = mesh.data(f).id;
        // Compute mid-point of texture coordinates
        OpenMesh::Vec2f mid_point(0, 0);
        int valence = 0;
        for (auto fh_it = mesh.fh_iter(f); fh_it.is_valid(); ++fh_it)
        {
            mid_point += mesh.data(*fh_it).texcoord2D;
            valence++;
        }
        mid_point /= static_cast<float>(valence);

        for (auto fh_it = mesh.fh_iter(f); fh_it.is_valid(); ++fh_it)
        {
            auto v1 = mesh.data(mesh.edge_handle(*fh_it)).id;
            auto v2 = mesh.data(mesh.to_vertex_handle(*fh_it)).id;
            auto v3 = mesh.data(mesh.edge_handle(mesh.next_halfedge_handle(*fh_it))).id;

            std::vector<MyMesh::VertexHandle> face_vhandles;
            face_vhandles.push_back(mesh_next.vertex_handle(v0));
            face_vhandles.push_back(mesh_next.vertex_handle(v1));
            face_vhandles.push_back(mesh_next.vertex_handle(v2));
            face_vhandles.push_back(mesh_next.vertex_handle(v3));

            mesh_next.add_face(face_vhandles);

            OpenMesh::Vec2f v1_texcoord = (mesh.data(*fh_it).texcoord2D + mesh.data(mesh.prev_halfedge_handle(*fh_it)).texcoord2D) / 2;
            OpenMesh::Vec2f v2_texcoord = mesh.data(*fh_it).texcoord2D;
            OpenMesh::Vec2f v3_texcoord = (mesh.data(*fh_it).texcoord2D + mesh.data(mesh.next_halfedge_handle(*fh_it)).texcoord2D) / 2;

            std::vector<OpenMesh::Vec2f> new_tex_coords = {mid_point, v1_texcoord, v2_texcoord, v3_texcoord};

            // std::cout << new_tex_coords[0] << " " << new_tex_coords[1] << " " << new_tex_coords[2] << " " << new_tex_coords[3] << std::endl;

            OpenMesh::FaceHandle last_face_handle = mesh_next.face_handle(mesh_next.n_faces() - 1);

            int tmp = 0;
            for (auto fh_it2 = mesh_next.fh_iter(last_face_handle); fh_it2.is_valid(); ++fh_it2)
            {
                mesh_next.data(*fh_it2).texcoord2D = new_tex_coords[tmp++];
            }

            if (mesh.data(f).patched) {
                mesh_next.data(last_face_handle).patched = true;
            } else {
                mesh_next.data(last_face_handle).patched = false;
            }
        }
    }

    // Prepare data for JSON output
    to_json(v_indices, v_offsets, v_valances, v_index, v_data,
            e_indices, e_data,
            f_indices, f_offsets, f_valances, f_data,
            depth, output_dir);

    // Update prev_idx
    prev_idx = fidx;

    // Replace the original mesh with the subdivided mesh
    mesh = mesh_next;

    return {mesh_next, fidx};
}
