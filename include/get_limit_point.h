#ifndef GET_LIMIT_POINT_H
#define GET_LIMIT_POINT_H

#include <string>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>
#include <nlohmann/json.hpp>
#include "MyTraits.h"


typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

void clear_json(const std::string &output_dir);
void append_to_json(const nlohmann::json &data, const std::string &output_dir);
void get_limit_point(MyMesh &mesh, const std::string &output_dir, int depth, int idx);

#endif
