//
// Created by Seobeen Jang on 2024/10/01.
//

#ifndef PREPROCESSOR_UTILS_H
#define PREPROCESSOR_UTILS_H

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <vector>
#include "MyTraits.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

void add_dash(const std::string &output_dir);
std::pair<std::string, std::string> make_output_dir(const std::string &input_file);
void move_prev_files(const std::string &output_dir);
void compress_output_dir(const std::string &str_name);
MyMesh initialize_mesh(const std::string &input_file, const std::string &output_dir);

#endif //PREPROCESSOR_UTILS_H
