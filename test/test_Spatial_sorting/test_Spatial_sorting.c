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

#include <hxt_vertices.h>
#include <time.h>

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


status_t read_nodes(const char* filename, bbox_t* bbox, vertex_t** vertices_p, uint32_t* npts)
{
  FILE* file = fopen(filename,"r");
  if(file==NULL)
    return HXT_ERROR_MSG(HXT_STATUS_FAILED, "cannot open file");

  if(fscanf(file," %d %*d %*d ", npts)==0)
    return HXT_ERROR_MSG(HXT_STATUS_FAILED, "error reading 1st line of %s", filename);

  printf("file contain %d vertices\n", *npts);

  HXT_CHECK( HXT_malloc(vertices_p, sizeof(vertex_t)*(*npts)) );

  for (uint32_t i=0; i<(*npts); i++){
    if(fscanf(file, " %*d %lf %lf %lf ", &(*vertices_p)[i].coord[0], &(*vertices_p)[i].coord[1], &(*vertices_p)[i].coord[2])==0)
      return HXT_ERROR_MSG(HXT_STATUS_FAILED, "error reading %s", filename);

    for (int j=0; j<3; j++) {
      if((*vertices_p)[i].coord[j]<bbox->min[j])
        bbox->min[j]=(*vertices_p)[i].coord[j];
      if((*vertices_p)[i].coord[j]>bbox->max[j])
        bbox->max[j]=(*vertices_p)[i].coord[j];
    }
  }

  fclose(file);
  puts("finished reading");
  return HXT_STATUS_OK;
}


status_t create_nodes(bbox_t* bbox, vertex_t** vertices_p, uint32_t npts)
{
  HXT_CHECK( HXT_malloc(vertices_p, sizeof(vertex_t)*npts) );

  for (uint32_t i=0; i<npts; i++) {
    (*vertices_p)[i].coord[0] = (double) rand()/RAND_MAX;
    (*vertices_p)[i].coord[1] = (double) rand()/RAND_MAX;
    (*vertices_p)[i].coord[2] = (double) rand()/RAND_MAX;
  }

  bbox->min[0] = 0.0;
  bbox->min[1] = 0.0;
  bbox->min[2] = 0.0;
  bbox->max[0] = 1.0;
  bbox->max[1] = 1.0;
  bbox->max[2] = 1.0;

  // // worst case scenario
  // for(uint32_t i=0; i<npts/2; i++) {
  //   (*vertices_p)[i].coord[0] = 0.0;
  //   (*vertices_p)[i].coord[1] = 2.0*i/npts;
  //   (*vertices_p)[i].coord[2] = 0.5;
  // }

  // for(uint32_t i=npts/2; i<npts; i++) {
  //   (*vertices_p)[i].coord[0] = 1.0;
  //   (*vertices_p)[i].coord[1] = 0.5;
  //   (*vertices_p)[i].coord[2] = 2.0*(i - npts/2)/npts;
  // }

  // bbox->min[0] = 0.0;
  // bbox->min[1] = 0.0;
  // bbox->min[2] = 0.0;
  // bbox->max[0] = 1.0;
  // bbox->max[1] = 2.0*(npts-1)/npts;
  // bbox->max[2] = 2.0*(npts - 1 - npts/2)/npts;

  return HXT_STATUS_OK;
}



int main(int argc, char **argv)
{
    printf("%d\n", argc);

  if(argc<2){
    printf("usage: %s INPUT OUTPUT\n"
                "INPUT is a .node file:\n"
                "   N+1 3 0\n"
                "   1 x0 y0 z0\n"
                "   2 x1 y1 z1\n"
                "   3 x2 y2 z2\n"
                "     ...\n"
                "   N+1 xN yN zN\n\n"
                "  or \"-NUM\" to generate NUM random point inside the unit cube\n\n"
                "OUTPUT is the tetrahedral mesh in GMSH format\n"
                "  or \"-\" if you do not want any output file", argv[0]);
    return 0;
  }

  mesh_t* mesh;
  HXT_CHECK( HXT_mesh_create(&mesh) );

  if(argv[1][0]!='-' || argv[1][1]<'0' || argv[1][1]>'9'){
    HXT_CHECK( read_nodes(argv[1], &mesh->bbox, &mesh->vertices, &mesh->num_vertices) );
    mesh->size_vertices = mesh->num_vertices;
  }
  else {
    if(sscanf(argv[1]+1, " %u ", &mesh->num_vertices)==0)
      return HXT_ERROR_MSG(HXT_STATUS_ERROR ,"%s is not a valid number", argv[1]+1);

    HXT_INFO("creating %u vertices", mesh->num_vertices);
    HXT_CHECK( create_nodes(&mesh->bbox, &mesh->vertices, mesh->num_vertices) );
  }

  clock_t time0 = clock();

  // TODO: substituir por nossa ordenação
  HXT_CHECK( KDT_vertices_BRIO(&mesh->bbox, mesh->vertices, mesh->num_vertices) );
  // END TODO

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
