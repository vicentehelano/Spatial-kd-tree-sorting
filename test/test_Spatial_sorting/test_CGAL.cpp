#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <CGAL/Delaunay_triangulation_3.h>
#include <CGAL/Delaunay_triangulation_cell_base_3.h>
#include <CGAL/point_generators_3.h>
#include <CGAL/Real_timer.h>
#include <CGAL/Triangulation_vertex_base_3.h>

#include <iostream>
#include <fstream>
#include <vector>

#include <cargs.h>

#include <xoroshiro256plusplus.h>

typedef CGAL::Exact_predicates_inexact_constructions_kernel K;
typedef CGAL::Delaunay_triangulation_3<K> Triangulation;
typedef K::Point_3          Point;

static uint64_t default_seed = 1234567890ULL;
const int NUM_INSERTED_POINTS = 1000000;

typedef enum point_distribution {
  AXES,
  CUBE,
  CYLINDER,
  DISK,
  LIU,
  PLANES,
  PARABOLOID,
  SPIRAL,
  SADDLE
} Point_distribution;

static struct cag_option options[] = {
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

  {.identifier = 'S',
    .access_letters = "S",
    .access_name = "saddle",
    .value_name = "NUMBER",
    .description = "generate points around saddle surface"},

  {.identifier = 'h',
    .access_letters = "h",
    .access_name = "help",
    .description = "shows the command help"}};

void points_within_axes(uint32_t npts, std::vector<Point> &out)
{
    double sdx = 1e-2; // standard deviation
    double sdy = 1e-2;
    double sdz = 1e-2;

    uint32_t third = npts / 3;

    xoroshiro256plusplus_seed(default_seed);

    // Points on the plane xy
    for (uint32_t i = 0; i < third; i++) {
        double x = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        double y = nxoroshiro256plusplus_d() * sdy;
        double z = nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }

    // Points on the plane yz
    for (uint32_t i = third; i < 2*third; i++) {
        double x = nxoroshiro256plusplus_d() * sdx;
        double y = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        double z = nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }

    // Points on the plane zx
    for (uint32_t i = 2*third; i < npts; i++) {
        double x = nxoroshiro256plusplus_d() * sdx;
        double y = nxoroshiro256plusplus_d() * sdy;
        double z = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }
}

void points_within_cube(uint32_t npts, std::vector<Point> &out)
{
    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i=0; i<npts; i++) {
        double x = xoroshiro256plusplus_d();
        double y = xoroshiro256plusplus_d();
        double z = xoroshiro256plusplus_d();
        out.push_back( Point(x,y,z) );
    }
}

void points_within_cylinder(uint32_t npts, std::vector<Point> &out, double h)
{
    double R = 1.0; // radius

    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i = 0; i < npts; i++) {
        double theta = 2 * M_PI * xoroshiro256plusplus_d();
        double r = R * sqrt(xoroshiro256plusplus_d());
        double x = r * sin(theta);
        double y = r * cos(theta);
        double z = h * (xoroshiro256plusplus_d() - 0.5);

        // add gaussian noise
        double sdx = 1e-2; // standard deviation in x
        double sdy = 1e-2; // standard deviation in y
        double sdz = 1e-2; // standard deviation in z
        x += sdx * nxoroshiro256plusplus_d();
        y += sdy * nxoroshiro256plusplus_d();
        z += sdz * nxoroshiro256plusplus_d();

        out.push_back( Point(x,y,z) );
    }
}

void points_within_planes(uint32_t npts, std::vector<Point> &out)
{
    double sdx = 1e-2; // standard deviation in x
    double sdy = 1e-2; // standard deviation in y
    double sdz = 1e-2; // standard deviation in z

    uint32_t third = npts / 3;

    xoroshiro256plusplus_seed(default_seed);

    // plane points xy
    for (uint32_t i = 0; i < third; i++) {
        double x = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        double y = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        double z = nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }

    // plane points yz
    for (uint32_t i = third; i < 2*third; i++) {
        double x = nxoroshiro256plusplus_d() * sdx;
        double y = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        double z = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }

    // plane points zx
    for (uint32_t i = 2*third; i < npts; i++) {
        double x = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        double y = nxoroshiro256plusplus_d() * sdy;
        double z = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
        out.push_back( Point(x,y,z) );
    }
}

void points_within_paraboloid(uint32_t npts, std::vector<Point> &out)
{
    double R = 1.0; // ray

    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i = 0; i < npts; i++) {
        double theta = 2 * M_PI * xoroshiro256plusplus_d();
        double r = R * sqrt(xoroshiro256plusplus_d());
        double x = r * sin(theta);
        double y = r * cos(theta);
        double z = pow(x, 2) + pow(y, 2);

        // add gaussian noise
        double sdx = 1e-2; // standard deviation in x
        double sdy = 1e-2; // standard deviation in y
        double sdz = 1e-2; // standard deviation in z
        x += sdx * nxoroshiro256plusplus_d();
        y += sdy * nxoroshiro256plusplus_d();
        z += sdz * nxoroshiro256plusplus_d();

        out.push_back( Point(x,y,z) );
    }
}

void points_within_spiral(uint32_t npts, std::vector<Point> &out)
{
    double a = 0.25 / M_PI;
    double b = 300.0;
    double h = (b-a)/(npts-1);

    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i = 0; i < npts; i++) {
        double u0 = i*h;
        double theta = 2 * M_PI * sqrt(u0);
        double alpha = 0.5;
        double beta = 0.01;
        double gamma = 1.0;
        double x = alpha * theta * exp(beta * theta) * sin(theta);
        double y = alpha * theta * exp(beta * theta) * cos(theta);
        double z = gamma * theta;

        // add gaussian noise
        double sdx = 5e-1; // standard deviation in x
        double sdy = 5e-1; // standard deviation in y
        double sdz = 1e0;  // standard deviation in z
        x += sdx * nxoroshiro256plusplus_d();
        y += sdy * nxoroshiro256plusplus_d();
        z += sdz * nxoroshiro256plusplus_d();

        out.push_back( Point(x,y,z) );
    }
}

void points_around_saddle(uint32_t npts, std::vector<Point> &out)
{
    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i = 0; i < npts; i++) {
        double x = 2*xoroshiro256plusplus_d() - 1.0;
        double y = 2*xoroshiro256plusplus_d() - 1.0;
        double z = x*x - y*y;

        // add gaussian noise
        double sdx = 1e-2; // standard deviation in x
        double sdy = 1e-2; // standard deviation in y
        double sdz = 1e-2; // standard deviation in z
        x += sdx * nxoroshiro256plusplus_d();
        y += sdy * nxoroshiro256plusplus_d();
        z += sdz * nxoroshiro256plusplus_d();

        out.push_back( Point(x,y,z) );
    }
}

int __create_vertices(uint32_t npts, Point_distribution d, std::vector<Point> &vertices)
{
  vertices.reserve(npts);

  switch (d) {
      case AXES:
          points_within_axes(npts, vertices);
          break;
      case CUBE:
          points_within_cube(npts, vertices);
          break;
      case CYLINDER:
          points_within_cylinder(npts, vertices, 2.0);
          break;
      case DISK:
          points_within_cylinder(npts, vertices, 0.0625);
          break;
      case PLANES:
          points_within_planes(npts, vertices);
          break;
      case PARABOLOID:
          points_within_paraboloid(npts, vertices);
          break;
      case SPIRAL:
          points_within_spiral(npts, vertices);
          break;
      case SADDLE:
          points_around_saddle(npts, vertices);
          break;
      default:
          return EXIT_FAILURE;
  }

  return EXIT_SUCCESS;
}

int create_vertices(int argc, char *argv[], std::vector<Point> &vertices)
{
  uint32_t npts = 0;
  const char *value = NULL;
  cag_option_context context;
  cag_option_init(&context, options, CAG_ARRAY_SIZE(options), argc, argv);

  while (cag_option_fetch(&context)) {
    switch (cag_option_get_identifier(&context)) {
        case 'a':
          #ifndef NDEBUG
          std::cout << "generating points around coordinate axes\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, AXES, vertices);
          break;
        case 'c':
          #ifndef NDEBUG
          std::cout << "generating points within cube\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, CUBE, vertices);
          break;
        case 'C':
          #ifndef NDEBUG
          std::cout << "generating points within cylinder\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, CYLINDER, vertices);
          break;
        case 'd':
          #ifndef NDEBUG
          std::cout << "generating points within disk\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, DISK, vertices);
          break;
        case 'p':
          #ifndef NDEBUG
          std::cout << "generating points around coordinate planes\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, PLANES, vertices);
          break;
        case 'P':
          #ifndef NDEBUG
          std::cout << "generating points around paraboloid\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, PARABOLOID, vertices);
          break;
        case 's':
          #ifndef NDEBUG
          std::cout << "generating points around logarithmic spiral\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, SPIRAL, vertices);
          break;
        case 'S':
          #ifndef NDEBUG
          std::cout << "generating points around logarithmic spiral\n";
          #endif
          value = cag_option_get_value(&context);
          npts = atoi(value);
          __create_vertices(npts, SADDLE, vertices);
          break;
    }
  }

  return EXIT_SUCCESS;
}

void usage(char *argv[])
{
  printf("Usage: %s [OPTION]...\n\n", argv[0]);
  cag_option_print(options, CAG_ARRAY_SIZE(options), stdout);
}

int main(int argc, char **argv)
{
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

  // Generate points
  std::vector<Point> vertices;
  create_vertices(argc, argv, vertices);

  clock_t time0 = clock();
  Triangulation T(vertices.begin(), vertices.end());
  clock_t time1 = clock();
  std::cout << ((double) (time1-time0) / CLOCKS_PER_SEC) << std::endl;

  return 0;
}
