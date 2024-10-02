import openmesh as om
import numpy as np
import sys


def main(obj_file, output_dir):
    output_file = output_dir + "/halfedge.txt"
    mesh = om.read_polymesh("./mesh_files/" + obj_file, halfedge_tex_coord=True, vertex_tex_coord=True)

    with open(output_file, "w") as f:
        for heh in mesh.halfedges():
            # numpy 배열을 리스트로 변환한 후, 문자열로 변환하여 파일에 작성
            texcoord = mesh.texcoord2D(heh).tolist()
            f.write(f"{texcoord[0]}, {texcoord[1]}")
            f.write("\n")


if __name__ == "__main__":
    main(sys.argv[1], sys.argv[2])
