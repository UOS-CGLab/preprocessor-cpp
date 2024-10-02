#include "utils.h"
#include <fstream>
#include <iostream>
#include <filesystem>
#include <string>

#include <OpenMesh/Core/IO/MeshIO.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

#include "obj_to_json.h"
#include "MyTraits.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

namespace fs = std::filesystem;
namespace {
    std::vector<OpenMesh::Vec2f> fetch_halfedge_texcoords(MyMesh &mesh, const std::string &input_file, const std::string &output_dir)
    {
        std::string python_file = "./etc/halfedge.py";
        std::string cmd = "python3.11 " + python_file + " " + input_file + " " + output_dir;

        int result = system(cmd.c_str());
        if (result != 0) {
            std::cerr << "Failed to fetch halfedge texcoords" << std::endl;
            exit(1);
        }

        std::ifstream file(output_dir + "/halfedge.txt");

        std::vector<OpenMesh::Vec2f> texcoords;
        if (file)
        {
            std::string line;
            while (std::getline(file, line))
            {
                // 쉼표를 제거하고 x, y를 파싱
                std::replace(line.begin(), line.end(), ',', ' ');  // 쉼표를 공백으로 대체
                std::istringstream iss(line);
                float x, y;
                if (iss >> x >> y)
                {
                    texcoords.emplace_back(x, y);
                }
                else
                {
                    std::cerr << "Error: Unable to parse line: " << line << std::endl;
                }
            }
        }
        else
        {
            std::cerr << "Error: Cannot open halfedge.txt" << std::endl;
            exit(1);
        }

        return texcoords;
    }
}

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

std::pair<std::string, std::string> make_output_dir(const std::string &input_file)
{
    /*
     * Make output directory
     * Output: output/input_file_name
     */
    std::string obj_file = "./etc/mesh_files/" + input_file;
    std::string str_name = input_file.substr(0, input_file.find_last_of('.'));
    std::string output_dir = "./output/" + str_name;
    // std::cout << "Output directory: " << output_dir << std::endl;
    // exit(0);

    if (!fs::exists(output_dir)){
        fs::create_directories(output_dir);
    } else {
        fs::remove_all(output_dir);
        fs::create_directories(output_dir);
    }

    if (str_name == "monsterfrog"){
        move_prev_files(output_dir);
    }

    obj_to_json(obj_file, output_dir + "/base.json");

    return {str_name, output_dir};
}

void move_prev_files(const std::string &output_dir)
{
    /*
     * Copy files from ./past_files to output_dir
     * files to Copy: d512.bmp, animation directory
     */

    std::string past_files_dir = "./etc/monsterfrog_files";
    if (!fs::exists(past_files_dir)){
        std::cerr << "Error: past_files directory does not exist" << std::endl;
    }

    // Move d512.bmp
    std::string d512_src = past_files_dir + "/d512.bmp";
    std::string d512_dst = output_dir + "/d512.bmp";
    if (fs::exists(d512_src)){
        fs::copy_file(d512_src, d512_dst, fs::copy_options::overwrite_existing);
    } else {
        std::cerr << "Error: d512.bmp does not exist" << std::endl;
    }

    // Move animation directory
    std::string animation_src = past_files_dir + "/animation";
    std::string animation_dst = output_dir + "/animation";
    if (fs::exists(animation_src)){
        fs::copy(animation_src, animation_dst, fs::copy_options::recursive | fs::copy_options::overwrite_existing);
    } else {
        std::cerr << "Error: animation directory does not exist" << std::endl;
    }
}

void compress_output_dir(const std::string &str_name)
{
    /*
     * Compress the ./output/$(str_name) directory
     * Output: output_dir.zip
     */

    std::string output_dir = "./output/" + str_name;
    std::string zip_file = output_dir + ".zip";
    std::string cmd = "zip -r " + zip_file + " " + output_dir + " > /dev/null 2>&1";

    int result = system(cmd.c_str());
    if (result != 0) {
        std::cerr << "Failed to compress the directory: " << output_dir << std::endl;
    }
}

MyMesh initialize_mesh(const std::string &input_file, const std::string &output_dir)
{
    MyMesh mesh;
    OpenMesh::IO::Options opt;
    opt += OpenMesh::IO::Options::VertexTexCoord;
    opt += OpenMesh::IO::Options::FaceTexCoord;
    mesh.request_vertex_texcoords2D();
    mesh.request_face_texture_index();
    mesh.request_halfedge_texcoords2D();

    if (!OpenMesh::IO::read_mesh(mesh, "./etc/mesh_files/" + input_file, opt))
    {
        std::cerr << "initialize_mesh error: Cannot read mesh from " << input_file << std::endl;
        exit(1);
    }

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

//    std::cout << mesh.texcoord2D(*mesh.halfedges_begin()) << std::endl;
//    exit(0);
    if (mesh.texcoord2D(*mesh.halfedges_begin()) == OpenMesh::Vec2f(0, 0))
    {
        std::vector<OpenMesh::Vec2f> texcoords = fetch_halfedge_texcoords(mesh, input_file, output_dir);
        auto texcoord_it = texcoords.begin();
        for (auto he_it = mesh.halfedges_begin(); he_it != mesh.halfedges_end(); ++he_it)
        {
            mesh.data(*he_it).texcoord2D = *texcoord_it;
            // std::cout << *texcoord_it << std::endl;
            ++texcoord_it;
        }
    }
    return mesh;
}

