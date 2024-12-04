# Preprocessor

This program is a Preprocessor of [Subdivision](https://github.com/UOS-CGLab/Subdivision).


## Contents
- [Overview](#overview)
- [prerequisites](#prerequisites)
- [flow of the code](#flow-of-the-code)
  - [find patches](#find-patches)
  - [find extraordinary points](#find-extraordinary-points)
  - [Subdivision](#subdivision)
  - [find limit points](#find-limit-points)




## Overview

This program performs mesh processing and subdivision using OpenMesh. 
It reads a 3D mesh from an .obj file, processes it iteratively to a specified depth of subdivision, 
and generates output files containing subdivision results, extraordinary points, and limit points. 
The program also organizes and compresses the results into a structured output directory.

## prerequisites

## flow of the code


### find patches

code of this step is in [get_patch.cpp](https://github.com/UOS-CGLab/preprocessor-cpp/blob/main/src/get_patch.cpp)

The code iterates through all the faces of the mesh, checking the following five conditions during each iteration:

1. The face has not been processed yet.
2. The face is not a boundary and has a valence of 4.
3. All neighboring faces also have a valence of 4.
4. If the depth is not 0, all neighboring faces must not be boundaries.
5. The face does not contain any extraordinary vertices.

If all the above conditions are satisfied, the face is at the center of a patch, 
and the code gathers data including the vertices indices and texture coordinates of the patch.

<img src="./etc/imgs/patch2.svg" alt="Description" width="400"> 

After gathering the data, the code write the data into a file named `patch.txt`

### find extraordinary points

code of this step is in [get_extraordinary.cpp](https://github.com/UOS-CGLab/preprocessor-cpp/blob/main/src/get_extraordinary.cpp)

The code iterates through all the vertices of the mesh, checking if the vertex has not been processed and the valence is not 4.
If the valence is not 4, the code marks the 3 layers of faces around the vertex to be subdivided.
<img src="./etc/imgs/extra.svg" width="400">

After checking all the vertices, the code iterates through the all faces of the mesh, 
checking if the face have a extraordinary vertex which is not included in the patch.
If the face has a extraordinary vertex, the code writhe the face vertices indices 
and texture coordinates into a file named `extraordinary.txt` **This only works with depth > 0**.

### Subdivision

code of this step is in [subdivision.cpp](https://github.com/UOS-CGLab/preprocessor-cpp/blob/main/src/subdivision.cpp)

We use the Catmull-Clark subdivision algorithm to subdivide the mesh.

For the face and edge texture coordinates, we use average value of the texture coordinates of the containing vertices.

<img src="./etc/imgs/texcoords.svg" width="400">

Vertex texture coordinates are the same as the original vertex texture coordinates.

For each subdivided face, texture coordinates are saved as follows:
```cpp
std::vector<OpenMesh::Vec2f> new_tex_coords = {mid_point, v1_texcoord, v2_texcoord, v3_texcoord};
```
<img src="./etc/imgs/texcoord_subdivided.svg" width="400">
 

### find limit points

code of this step is in [get_limit_point.cpp](https://github.com/UOS-CGLab/preprocessor-cpp/blob/main/src/get_limit_point.cpp)

The code iterates through all the vertices of the mesh, find the extraordinary vertices(i.e. valence is not 4).

For each extraordinary vertex, the code iterates through halfedges around the vertex,
```cpp
for (auto voh_it = mesh.voh_iter(*v_it); voh_it.is_valid(); ++voh_it)
```

find neighboring vertices of vv and ve include itself, and save all the vertices into a file named `limit_point.json`. 

<img src="./etc/imgs/limitpoints.svg" width="800">













