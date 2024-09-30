#ifndef SUBDIVISION_H
#define SUBDIVISION_H

#include <string>
#include "MyTraits.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

void compute_CC_vertex(MyMesh &mesh, MyMesh::VertexHandle v, OpenMesh::Vec3f &p);
void compute_CC_face(MyMesh &mesh, MyMesh::FaceHandle f, OpenMesh::Vec3f &p);
void compute_CC_edge(MyMesh &mesh, MyMesh::EdgeHandle e, OpenMesh::Vec3f &p);

std::tuple<MyMesh, int> subdivision(MyMesh &mesh, int &prev_idx, int depth, const std::string &output_dir);

#endif // SUBDIVISION_H
