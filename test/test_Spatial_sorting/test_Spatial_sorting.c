/*  Copyright (C) 2018 Célestin Marot                                       *
                                                                            *
    This file is part of hxt_SeqDel, a sequential Delaunay triangulator.    *
                                                                            *
    hxt_SeqDel is free software: you can redistribute it and/or modify      *
    it under the terms of the GNU General Public License as published by    *
    the Free Software Foundation, either version 3 of the License, or       *
    (at your option) any later version.                                     *
                                                                            *
    hxt_SeqDel is distributed in the hope that it will be useful,           *
    but WITHOUT ANY WARRANTY; without even the implied warranty of          *
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the           *
    GNU General Public License for more details.                            *
                                                                            *
    You should have received a copy of the GNU General Public License       *
    along with hxt_SeqDel.  If not, see <http://www.gnu.org/licenses/>.     *
                                                                            *
    See the COPYING file for the GNU General Public License .               *
                                                                            *
Author: Célestin Marot (celestin.marot@uclouvain.be)                        */

#include <time.h>
#include <string.h>
#include <assert.h>

#include <math.h>

#include <cargs.h>

#include <hxt_vertices.h>
#include <kdt_vertices.h>
#include <kdt_point_generators.h>

typedef enum point_distribution {
  AXES,
  CUBE,
  CYLINDER,
  DISK,
  LIU,
  PLANES,
  PARABOLOID,
  SPIRAL
} Point_distribution;

typedef enum sorting_algorithm {
  UNDEFINED_ALGORITHM = -1,
  HXT,
  KDT,
} Sorting_algorithm;

static struct cag_option options[] = {
  {.identifier = 'T',
    .access_letters = "T",
    .access_name = "timing",
    .value_name = NULL,
    .description = "use to display timing"},

  {.identifier = 'H',
    .access_letters = "H",
    .access_name = "hxt",
    .value_name = NULL,
    .description = "use the HXT sorting function"},

  {.identifier = 'K',
    .access_letters = "K",
    .access_name = "kdt",
    .value_name = NULL,
    .description = "use the cut-longest-edge kd-tree sorting function"},

  {.identifier = 'a',
    .access_letters = "a",
    .access_name = "axes",
    .value_name = "NUMBER",
    .description = "generate points around the x, y and z axes"},

  {.identifier = 'c',
    .access_letters = "c",
    .access_name = "cube",
    .value_name = "NUMBER",
    .description = "generate points inside a unit cube"},

  {.identifier = 'C',
    .access_letters = "C",
    .access_name = "cylinder",
    .value_name = "NUMBER",
    .description = "generate points inside a unit cylinder"},

  {.identifier = 'd',
    .access_letters = "d",
    .access_name = "disk",
    .value_name = "NUMBER",
    .description = "generate points inside a unit disk (cylinder with height = 0.1)"},

  {.identifier = 'L',
    .access_letters = "L",
    .access_name = "liu",
    .value_name = "NUMBER",
    .description = "generate Liu's figure 5 point set"},

  {.identifier = 'p',
    .access_letters = "p",
    .access_name = "planes",
    .value_name = "NUMBER",
    .description = "generate points around canonical planes"},

  {.identifier = 'P',
    .access_letters = "P",
    .access_name = "paraboloid",
    .value_name = "NUMBER",
    .description = "generate points on the surface of a paraboloid"},

  {.identifier = 's',
    .access_letters = "s",
    .access_name = "spiral",
    .value_name = "NUMBER",
    .description = "generate points along a spiral"},

  {.identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .description = "shows the command help"}};

// simple visualisation with gmsh
status_t gmshTetDraw(mesh_t* mesh, const char* filename)
{
  FILE* file = fopen(filename,"w");
  if(file==NULL)
    HXT_ERROR_MSG(HXT_STATUS_FAILED, "Cannot open file %s",filename);

  /* format for gmsh */
  fprintf(file,"$MeshFormat\n"
               "2.2 0 %u\n"
               "$EndMeshFormat\n"
               "$Nodes\n"
               "%u\n",(unsigned) sizeof(double), mesh->num_vertices);

  { /* print the nodes */
    uint32_t i;
    for (i=0; i<mesh->num_vertices; i++)
      fprintf(file,"%u %.10E %.10E %.10E\n",i+1, mesh->vertices[i].coord[0],
                                                 mesh->vertices[i].coord[1],
                                                 mesh->vertices[i].coord[2]);
  }

  // count non-ghost vertex:
  uint64_t index = 0;
  uint64_t i;
  for (i=0; i<mesh->tetrahedra.num; i++)
  {
    if(mesh->tetrahedra.node[i*4 + 3]!=UINT32_MAX){
      ++index;
    }
  }

  fprintf(file,"$EndNodes\n"
                 "$Elements\n"
                 "%lu\n",index);

  { /* print the elements */

    index = 0;
    for (i=0; i<mesh->tetrahedra.num; i++){
      if(mesh->tetrahedra.node[i*4 + 3]!=UINT32_MAX){
      fprintf(file,"%lu 4 0 %u %u %u %u\n", ++index,mesh->tetrahedra.node[i*4]+1,
                                           mesh->tetrahedra.node[i*4 + 1]+1,
                                           mesh->tetrahedra.node[i*4 + 2]+1,
                                           mesh->tetrahedra.node[i*4 + 3]+1);
      }
    }
  }

  fputs("$EndElements\n",file);
  fclose(file);
  return HXT_STATUS_OK;
}

void __get_bounding_box(bbox_t* bbox, vertex_t** vertices_p, uint32_t npts)
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

status_t __create_vertices(bbox_t* bbox, vertex_t** vertices_p, uint32_t npts, Point_distribution d)
{
  #ifndef DEBUG
  HXT_INFO("creating %u vertices", npts);
  #endif
  HXT_CHECK( HXT_malloc(vertices_p, sizeof(vertex_t)*npts) );

  switch (d) {
      case AXES:
          points_within_axes(vertices_p, npts);
          break;
      case CUBE:
          points_within_cube(vertices_p, npts);
          break;
      case CYLINDER:
          points_within_cylinder(vertices_p, npts, 2.0);
          break;
      case DISK:
          points_within_cylinder(vertices_p, npts, 0.0625);
          break;
      case LIU:
          points_from_Liu(vertices_p);
          break;
      case PLANES:
          points_within_planes(vertices_p, npts);
          break;
      case PARABOLOID:
          points_within_paraboloid(vertices_p, npts);
          break;
      case SPIRAL:
          points_within_spiral(vertices_p, npts);
          break;
      default:
          return HXT_STATUS_FAILED;
  }

  // TODO:
  // CALCULAR O BOUNDING BOX MANUALMENTE,
  // USANDO UMA FUNÇÃO AUXILIAR: __get_bounding_box(...)
  // Compute bounding box
  __get_bounding_box(bbox, vertices_p, npts);

  return HXT_STATUS_OK;
}

uint32_t create_vertices(int argc, char *argv[], mesh_t *mesh)
{
  uint32_t npts = 0;
  const char *value = NULL;
  cag_option_context context;
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

  while (cag_option_fetch(&context)) {
    switch (cag_option_get_identifier(&context)) {
        case 'a':
          #ifndef DEBUG
          HXT_INFO("generating points around coordinate axes");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, AXES) == HXT_STATUS_OK);
          break;
        case 'c':
          #ifndef DEBUG
          HXT_INFO("generating points within cube");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, CUBE) == HXT_STATUS_OK);
          break;
        case 'C':
          #ifndef DEBUG
          HXT_INFO("generating points within cylinder");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, CYLINDER) == HXT_STATUS_OK);
          break;
        case 'd':
          #ifndef DEBUG
          HXT_INFO("generating points within disk");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, DISK) == HXT_STATUS_OK);
          break;
        case 'L':
          #ifndef DEBUG
          HXT_INFO("generating Liu's paper example");
          #endif
          value = cag_option_get_value(&context);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, 15, LIU) == HXT_STATUS_OK);
          break;
        case 'p':
          #ifndef DEBUG
          HXT_INFO("generating points around coordinate planes");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, PLANES) == HXT_STATUS_OK);
          break;
        case 'P':
          #ifndef DEBUG
          HXT_INFO("generating points around paraboloid");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, PARABOLOID) == HXT_STATUS_OK);
          break;
        case 's':
          #ifndef DEBUG
          HXT_INFO("generating points around logarithmic spiral");
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          assert(__create_vertices(&mesh->bbox, &mesh->vertices, npts, SPIRAL) == HXT_STATUS_OK);
          break;
    }
  }
  return npts;
}

Sorting_algorithm get_sorting_algorithm(int argc, char *argv[])
{
  cag_option_context context;
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  // get the target sorting algorithm
  while (cag_option_fetch(&context)) {
    switch (cag_option_get_identifier(&context)) {
        case 'H':
          return HXT;
        case 'K':
          return KDT;
    }
  }
  return UNDEFINED_ALGORITHM;
}

void usage(char *argv[])
{
  printf("Usage: %s [OPTION]...\n\n", argv[0]);
  cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
}

int main(int argc, char **argv)
{
  mesh_t* mesh;
  Sorting_algorithm alg = -1;
  cag_option_context context;

  // Grab help menu
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);
  while (cag_option_fetch(&context)) {
    switch (cag_option_get_identifier(&context)) {
        case 'h':
          usage(argv);
          return EXIT_SUCCESS;
    }
  }

  // Get sorting algorithm
  alg = get_sorting_algorithm(argc, argv);
  if (alg == UNDEFINED_ALGORITHM) {
    fprintf(stderr, "%s: undefined sorting algorithm.\n", argv[0]);
    usage(argv);
    return EXIT_FAILURE;
  }
  #ifndef DEBUG
  HXT_INFO("sorting algorithm: %s", ((alg == HXT)?("HXT native"):("cut-longest-edge kd-tree")));
  #endif // DEBUG
  // Create nodes
  HXT_CHECK( HXT_mesh_create(&mesh) );
  clock_t time0 = clock();
  mesh->num_vertices = create_vertices(argc, argv, mesh);
  clock_t time1 = clock();
  #ifndef DEBUG
  printf("Point generation: %f s\n", (double) (time1-time0) / CLOCKS_PER_SEC);
  #endif // DEBUG
  // Run the spatial sorting algorithm
  switch (alg) {
      case HXT:
          HXT_CHECK( HXT_vertices_BRIO(&mesh->bbox, mesh->vertices, mesh->num_vertices) );
          break;
      case KDT:
          HXT_CHECK( KDT_vertices_BRIO(mesh->bbox, mesh->vertices, mesh->num_vertices) );
          break;
      default:
          break;
  }
  clock_t time2 = clock();
  #ifndef DEBUG
  printf("BRIO: %f s\n", (double) (time2-time1) / CLOCKS_PER_SEC);
  #endif // DEBUG
  // this is were we are really doing the delaunay...
  HXT_CHECK( HXT_tetrahedra_compute(mesh) );

  clock_t time3 = clock();
  // O TEMPO AQUI
  printf("%f s\n", (double) (time3-time2) / CLOCKS_PER_SEC);

  #ifndef DEBUG
  printf("Delaunay insertion: %f s\n", (double) (time3-time2) / CLOCKS_PER_SEC);

  uint64_t numGhosts = 0;
  for(uint64_t i=0; i<mesh->tetrahedra.num; i++) {
    if(mesh->tetrahedra.node[4*i + 3]==HXT_GHOST_VERTEX)
      numGhosts++;
  }
  printf("%u vertices, %lu Delaunay tetrahedra, %lu ghosts, %f s\n", mesh->num_vertices, mesh->tetrahedra.num - numGhosts, numGhosts, (double) (time2-time0)/CLOCKS_PER_SEC);

  gmshTetDraw(mesh, "output.msh");

  HXT_CHECK( HXT_mesh_delete(&mesh) );

  #endif // DEBUG
  return HXT_STATUS_OK;
}
