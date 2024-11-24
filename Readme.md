# Preprocessor

This program is a Preprocessor of [Subdivifion](https://github.com/UOS-CGLab/Subdivision)


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

code of this step is in [get_patch.cpp]()

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

code of this step is in [get_extraordinary.cpp]()

The code iterates through all the vertices of the mesh, checking if the vertex has not been processed and the valence is not 4.
If the valence is not 4, the code marks the 3 layers of faces around the vertex to be subdivided.
<img src="./etc/imgs/extra.svg" width="400">

After checking all the vertices, the code iterates through the all faces of the mesh, 
checking if the face have a extraordinary vertex which is not included in the patch.
If the face has a extraordinary vertex, the code writhe the face vertices indices 
and texture coordinates into a file named `extraordinary.txt` **This only works with depth > 0**

### Subdivision

We use the Catmull-Clark subdivision algorithm to subdivide the mesh.


### find limit points

code of this step is in [get_limit_point.cpp]()

The code iterates through all the vertices of the mesh, checking if the vertex is not a boundary and the valence is not 4.



