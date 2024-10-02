#ifndef MYTRAITS_H
#define MYTRAITS_H

#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

// Define mesh traits
struct MyTraits : public OpenMesh::DefaultTraits {
    FaceTraits {
        OpenMesh::Vec3f position;
        bool visited;
        bool interior;
        bool patched;
        int id;
    };
    EdgeTraits {
        OpenMesh::Vec3f position;
        bool valid;
        int id;
    };
    VertexTraits {
        OpenMesh::Vec3f position;
        bool valid;
        int id;
        bool todo;
        bool visited;
        bool visited2;
        bool visited3;
        bool interior;
    };
    HalfedgeTraits {
        OpenMesh::Vec2f texcoord2D;
    };
    HalfedgeAttributes(OpenMesh::Attributes::TexCoord2D);
};

typedef OpenMesh::PolyMesh_ArrayKernelT<MyTraits> MyMesh;

#endif
