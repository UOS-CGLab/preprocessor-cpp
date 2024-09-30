#ifndef GET_PATCH_H
#define GET_PATCH_H

#include <string>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include "MyTraits.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

int get_patch(MyMesh &mesh, int idx, int depth, const std::string &output_dir);

#endif
