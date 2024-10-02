#ifndef GET_EXTRAORDINARY_H
#define GET_EXTRAORDINARY_H

#include <string>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "MyTraits.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

int add_vertex(MyMesh &mesh, MyMesh::VertexHandle v, int idx);
int add_face(MyMesh &mesh, MyMesh::FaceHandle f, int idx);

void get_extraordinary(MyMesh &mesh, const std::string &output_dir, int depth);
void write_extraordinary_points(MyMesh &mesh, const std::string &output_dir, int idx, int depth);

#endif // GET_EXTRAORDINARY_H
