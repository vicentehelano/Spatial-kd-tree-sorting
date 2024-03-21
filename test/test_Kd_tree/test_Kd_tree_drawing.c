#include <assert.h>

#include <kdt_vertices.h>
#include <kdt_point_generators.h>

void get_bounding_box(bbox_t* bbox, vertex_t** vertices_p, uint32_t npts)
{
    bbox->min[0] = (*vertices_p)[0].coord[0];
    bbox->min[1] = (*vertices_p)[0].coord[1];
    bbox->min[2] = (*vertices_p)[0].coord[2];
    bbox->max[0] = (*vertices_p)[0].coord[0];
    bbox->max[1] = (*vertices_p)[0].coord[1];
    bbox->max[2] = (*vertices_p)[0].coord[2];

    for (uint32_t i = 1; i < npts; i++) {
        // Update bbbx to X
        if ((*vertices_p)[i].coord[0] < bbox->min[0])
            bbox->min[0] = (*vertices_p)[i].coord[0];

        if ((*vertices_p)[i].coord[0] > bbox->max[0])
            bbox->max[0] = (*vertices_p)[i].coord[0];

        // Update bbbx to Y
        if ((*vertices_p)[i].coord[1] < bbox->min[1])
            bbox->min[1] = (*vertices_p)[i].coord[1];

        if ((*vertices_p)[i].coord[1] > bbox->max[1])
            bbox->max[1] = (*vertices_p)[i].coord[1];

        // Update bbbx to Z
        if ((*vertices_p)[i].coord[2] < bbox->min[2])
            bbox->min[2] = (*vertices_p)[i].coord[2];

        if ((*vertices_p)[i].coord[2] > bbox->max[2])
            bbox->max[2] = (*vertices_p)[i].coord[2];
    }
}

int main(int argc, char **argv)
{
    int npts = 15;
    vertex_t *vertices = NULL;
    bbox_t bbox;

    vertices = (vertex_t *) malloc(sizeof(vertex_t)*npts);
    assert(vertices != NULL);

    points_from_Liu(&vertices);
    get_bounding_box(&bbox, &vertices, npts);

    kd_node_t *root = KDT_vertices_build_kdtree(bbox, vertices, npts);

    desenha_arvore(root, "arvore.tex");

    return HXT_STATUS_OK;
}
