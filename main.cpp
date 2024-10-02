#include <iostream>
#include <string>
#include <vector>

// Include OpenMesh headers
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// Include other necessary headers
#include "get_patch.h"
#include "get_extraordinary.h"
#include "get_limit_point.h"
#include "subdivision.h"
#include "MyTraits.h"
#include "utils.h"

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

int main()
{
    std::cout << "input file: ";
    std::string input_file, str_name, output_dir;

    input_file = "monsterfrog.obj";
    std::cout << input_file << std::endl;

    std::cout << "depth of subdivision: ";
    int depth = 6; //std::cin >> depth;
    std::cout << depth << std::endl;

    std::tie(str_name, output_dir) = make_output_dir(input_file);

    MyMesh mesh = initialize_mesh(input_file, output_dir);

    int idx = 0;
    for (int i = 0; i <= depth; ++i)
    {
        std::cout << "depth: " << i << std::endl;
        get_patch(mesh, idx, i, output_dir);
        get_extraordinary(mesh, output_dir, i);

        write_extraordinary_points(mesh, output_dir, idx, i);

        if (i == depth + 1)
        {
            break;
        }

        // Subdivide the mesh
        std::tie(mesh, idx) = subdivision(mesh, idx, i, output_dir);

//        // export the mesh to obj
//        std::string output_file = output_dir + "/subdivision" + std::to_string(i) + ".obj";
//        OpenMesh::IO::write_mesh(mesh, output_file);

        get_limit_point(mesh, output_dir, i, idx);

        add_dash(output_dir);
    }

    compress_output_dir(str_name);

    return 0;
}
