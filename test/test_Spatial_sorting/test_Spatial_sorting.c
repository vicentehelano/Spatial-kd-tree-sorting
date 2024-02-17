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

#include <kdt_vertices.h>
#include <time.h>
#include <string.h>

typedef enum distribution {
  AXES,
  CUBE,
  CYLINDER,
  DISK,
  PLANES,
  PARABOLOID,
  SPIRAL
} Distribution;

typedef enum algorithm {
  KDT,
  HXT
} Algorithm;

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

void __points_in_cube(vertex_t** vertices_p, uint32_t npts)
{
    for (uint32_t i=0; i<npts; i++) {
    (*vertices_p)[i].coord[0] = (double) rand()/RAND_MAX;
    (*vertices_p)[i].coord[1] = (double) rand()/RAND_MAX;
    (*vertices_p)[i].coord[2] = (double) rand()/RAND_MAX;
  }
}

status_t create_nodes(bbox_t* bbox, vertex_t** vertices_p, uint32_t npts, Distribution d)
{
  HXT_CHECK( HXT_malloc(vertices_p, sizeof(vertex_t)*npts) );

  switch (d) {
      case  AXES:
          break;
      case CUBE:
          __points_in_cube(vertices_p, npts);
          break;
      case CYLINDER:
          break;
      case DISK:
          break;
      case PLANES:
          break;
      case PARABOLOID:
          break;
      case SPIRAL:
          break;
      default:
          break;
  }

  // Compute bounding box
  bbox->min[0] = 0.0;
  bbox->min[1] = 0.0;
  bbox->min[2] = 0.0;
  bbox->max[0] = 1.0;
  bbox->max[1] = 1.0;
  bbox->max[2] = 1.0;

  return HXT_STATUS_OK;
}

int main(int argc, char **argv)
{
    printf("%d\n", argc);

  if(argc != 4){
    printf("usage: %s -[hk] -[acCdpPs] NUMBER\n"
                "\nSORTING METHOD\n"
                "\t-h\t--hxt       \tuse HXT function.\n"
                "\t-k\t--kdt       \tuse KDT function.\n"
                "\nPOINT DISTRIBUTION\n"
                "\t-a\t--axes      \tgenerate points around the x, y and z axes.\n"
                "\t-c\t--cube      \tgenerate points inside a unit cube.\n"
                "\t-C\t--cylinder  \tgenerate points inside a unit cylinder.\n"
                "\t-d\t--disk      \tgenerate points inside a unit disk (cylinder with height = 0.1).\n"
                "\t-p\t--planes    \tgenerate points around canonical planes.\n"
                "\t-P\t--paraboloid\tgenerate points on the surface of a paraboloid.\n"
                "\t-s\t--spiral    \tgenerate points along a spiral.\n", argv[0], argv[1]);
    return 0;
  }

  // Check whether to use KDT or HXT
  Algorithm alg;
  if ((strcmp(argv[1], "-k") == 0 || strcmp(argv[1], "--kdt") == 0) && argc == 4)
    alg = KDT;
  else if ((strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--hxt") == 0 && argc == 4))
    alg = HXT;
  else {
    printf("Invalid option for algorithm.\n");
    return 0;
  }

  mesh_t* mesh;
  HXT_CHECK( HXT_mesh_create(&mesh) );
  /*
  // Obter o número de vértices
  uint32_t num_vertices;
  if(sscanf(argv[2], " %u ", &num_vertices) == 0) {
      printf("%s is not a valid number\n", argv[2]);
      return 0;
  }
  */

  //if(sscanf(argv[1]+1, " %u ", &mesh->num_vertices)==0)
  //return HXT_ERROR_MSG(HXT_STATUS_ERROR ,"%s is not a valid number", argv[1]+1);

  HXT_INFO("creating %u vertices", mesh->num_vertices);
  HXT_CHECK( create_nodes(&mesh->bbox, &mesh->vertices, mesh->num_vertices, CUBE) );

  clock_t time0 = clock();
  // Run the corresponding algorithm
  if (alg == KDT)
    HXT_CHECK( KDT_vertices_BRIO(&mesh->bbox, mesh->vertices, mesh->num_vertices) );
  else if (alg == HXT)
    HXT_CHECK( HXT_vertices_BRIO(&mesh->bbox, mesh->vertices, mesh->num_vertices) );

  clock_t time1 = clock();

  printf("BRIO : %f s\n", (double) (time1-time0) / CLOCKS_PER_SEC);

  // this is were we are really doing the delaunay...
  HXT_CHECK( HXT_tetrahedra_compute(mesh) );

  clock_t time2 = clock();
  printf("Delaunay insertion: %f s\n", (double) (time2-time1) / CLOCKS_PER_SEC);

  uint64_t numGhosts = 0;
  for(uint64_t i=0; i<mesh->tetrahedra.num; i++) {
    if(mesh->tetrahedra.node[4*i + 3]==HXT_GHOST_VERTEX)
      numGhosts++;
  }
  printf("%u vertices, %lu Delaunay tetrahedra, %lu ghosts, %f s\n", mesh->num_vertices, mesh->tetrahedra.num - numGhosts, numGhosts, (double) (time2-time0)/CLOCKS_PER_SEC);

  if(argv[2][0]!='-' || argv[2][1])
    HXT_CHECK( gmshTetDraw(mesh, argv[2]) );

  HXT_CHECK( HXT_mesh_delete(&mesh) );

  return HXT_STATUS_OK;
}
