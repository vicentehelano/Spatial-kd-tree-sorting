#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Real_timer.h>
#include <CGAL/Triangulation_vertex_base_3.h>

#include <iostream>
#include <fstream>
#include <vector>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef K::Point_3          Point;

const int NUM_INSERTED_POINTS = 1000000;

int main()
{
  CGAL::Random_points_in_cube_3<Point> rnd(1.);

  std::cerr << "Construction of a 3D Delaunay triangulation from a vector of "
            << NUM_INSERTED_POINTS << " random points in a cube" << std::endl;
  std::vector<Point> V;
  V.reserve(NUM_INSERTED_POINTS);
  for (int i = 0; i != NUM_INSERTED_POINTS; ++i)
    V.push_back(*rnd++);

  // Sequential Delaunay T3
  typedef CGAL::Delaunay_triangulation_3<K> SequentialTriangulation;

  CGAL::Real_timer t;
  t.start();
  SequentialTriangulation S(V.begin(), V.end());
  t.stop();
  std::cerr << "Sequential construction takes " << t.time() << " sec." << std::endl;

  return 0;
}
