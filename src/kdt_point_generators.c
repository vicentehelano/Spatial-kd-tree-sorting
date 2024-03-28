#include <xoroshiro256plusplus.h>

#include <hxt_vertices.h>

static uint64_t default_seed = 1234567890ULL;

void points_within_axes(vertex_t** vertices_p, uint32_t npts)
{
    double sdx = 1e-2; // standard deviation
    double sdy = 1e-2;
    double sdz = 1e-2;

    uint32_t third = npts / 3;

    xoroshiro256plusplus_seed(default_seed);

    // Points on the plane xy
    for (uint32_t i = 0; i < third; i++) {
        (*vertices_p)[i].coord[0] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = nxoroshiro256plusplus_d() * sdz;
    }

    // Points on the plane yz
    for (uint32_t i = third; i < 2*third; i++) {
        (*vertices_p)[i].coord[0] = nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = nxoroshiro256plusplus_d() * sdz;
    }

    // Points on the plane zx
    for (uint32_t i = 2*third; i < npts; i++) {
        (*vertices_p)[i].coord[0] = nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
    }
}

void points_within_cube(vertex_t** vertices_p, uint32_t npts)
{
    xoroshiro256plusplus_seed(default_seed);

    for (uint32_t i=0; i<npts; i++) {
    (*vertices_p)[i].coord[0] = xoroshiro256plusplus_d();
    (*vertices_p)[i].coord[1] = xoroshiro256plusplus_d();
    (*vertices_p)[i].coord[2] = xoroshiro256plusplus_d();
  }
}

void points_within_cylinder(vertex_t** vertices_p, uint32_t npts, double h)
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

        (*vertices_p)[i].coord[0] = x;
        (*vertices_p)[i].coord[1] = y;
        (*vertices_p)[i].coord[2] = z;
    }
}

void points_from_Liu(vertex_t** vertices_p)
{
    xoroshiro256plusplus_seed(default_seed);

    (*vertices_p)[0].coord[0] = 2.880;
    (*vertices_p)[0].coord[1] = 64.490;
    (*vertices_p)[0].coord[2] = 0.0;
    (*vertices_p)[0].dist = 1;

    (*vertices_p)[1].coord[0] = 22.320;
    (*vertices_p)[1].coord[1] = 56.810;
    (*vertices_p)[1].coord[2] = 0.0;
    (*vertices_p)[1].dist = 2;

    (*vertices_p)[2].coord[0] = 38.640;
    (*vertices_p)[2].coord[1] = 64.730;
    (*vertices_p)[2].coord[2] = 0.0;
    (*vertices_p)[2].dist = 3;

    (*vertices_p)[3].coord[0] = 47.520;
    (*vertices_p)[3].coord[1] = 50.090;
    (*vertices_p)[3].coord[2] = 0.0;
    (*vertices_p)[3].dist = 4;

    (*vertices_p)[4].coord[0] = 64.920;
    (*vertices_p)[4].coord[1] = 40.490;
    (*vertices_p)[4].coord[2] = 0.0;
    (*vertices_p)[4].dist = 5;

    (*vertices_p)[5].coord[0] = 66.480;
    (*vertices_p)[5].coord[1] = 19.730;
    (*vertices_p)[5].coord[2] = 0.0;
    (*vertices_p)[5].dist = 6;

    (*vertices_p)[6].coord[0] = 90.840;
    (*vertices_p)[6].coord[1] = 4.010;
    (*vertices_p)[6].coord[2] = 0.0;
    (*vertices_p)[6].dist = 7;

    (*vertices_p)[7].coord[0] = 98.280;
    (*vertices_p)[7].coord[1] = 43.730;
    (*vertices_p)[7].coord[2] = 0.0;
    (*vertices_p)[7].dist = 8;

    (*vertices_p)[8].coord[0] = 102.840;
    (*vertices_p)[8].coord[1] = 70.970;
    (*vertices_p)[8].coord[2] = 0.0;
    (*vertices_p)[8].dist = 9;

    (*vertices_p)[9].coord[0] = 119.760;
    (*vertices_p)[9].coord[1] = 59.810;
    (*vertices_p)[9].coord[2] = 0.0;
    (*vertices_p)[9].dist = 10;

    (*vertices_p)[10].coord[0] = 125.400;
    (*vertices_p)[10].coord[1] = 17.330;
    (*vertices_p)[10].coord[2] = 0.0;
    (*vertices_p)[10].dist = 11;

    (*vertices_p)[11].coord[0] = 142.680;
    (*vertices_p)[11].coord[1] = 44.330;
    (*vertices_p)[11].coord[2] = 0.0;
    (*vertices_p)[11].dist = 12;

    (*vertices_p)[12].coord[0] = 162.480;
    (*vertices_p)[12].coord[1] = 22.130;
    (*vertices_p)[12].coord[2] = 0.0;
    (*vertices_p)[12].dist = 13;

    (*vertices_p)[13].coord[0] = 182.400;
    (*vertices_p)[13].coord[1] = 11.450;
    (*vertices_p)[13].coord[2] = 0.0;
    (*vertices_p)[13].dist = 14;

    (*vertices_p)[14].coord[0] = 199.680;
    (*vertices_p)[14].coord[1] = 18.770;
    (*vertices_p)[14].coord[2] = 0.0;
    (*vertices_p)[14].dist = 15;
}

void points_within_planes(vertex_t** vertices_p, uint32_t npts)
{
    double sdx = 1e-2; // standard deviation in x
    double sdy = 1e-2; // standard deviation in y
    double sdz = 1e-2; // standard deviation in z

    uint32_t third = npts / 3;

    xoroshiro256plusplus_seed(default_seed);

    // plane points xy
    for (uint32_t i = 0; i < third; i++) {
        (*vertices_p)[i].coord[0] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = nxoroshiro256plusplus_d() * sdz;
    }

    // plane points yz
    for (uint32_t i = third; i < 2*third; i++) {
        (*vertices_p)[i].coord[0] = nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
    }

    // plane points zx
    for (uint32_t i = 2*third; i < npts; i++) {
        (*vertices_p)[i].coord[0] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdx;
        (*vertices_p)[i].coord[1] = nxoroshiro256plusplus_d() * sdy;
        (*vertices_p)[i].coord[2] = xoroshiro256plusplus_d() + nxoroshiro256plusplus_d() * sdz;
    }
}

void points_within_paraboloid(vertex_t** vertices_p, uint32_t npts)
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

        (*vertices_p)[i].coord[0] = x;
        (*vertices_p)[i].coord[1] = y;
        (*vertices_p)[i].coord[2] = z;
    }
}

void points_within_spiral(vertex_t** vertices_p, uint32_t npts)
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

        (*vertices_p)[i].coord[0] = x;
        (*vertices_p)[i].coord[1] = y;
        (*vertices_p)[i].coord[2] = z;
    }
}

void points_around_saddle(vertex_t** vertices_p, uint32_t npts)
{
    double R = 1.0; // ray

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

        (*vertices_p)[i].coord[0] = x;
        (*vertices_p)[i].coord[1] = y;
        (*vertices_p)[i].coord[2] = z;
    }
}
