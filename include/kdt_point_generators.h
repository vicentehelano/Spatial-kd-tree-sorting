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
/*
#include <time.h>
#include <string.h>
#include <assert.h>

#include <math.h>

#include <cargs.h>
*/

#ifndef _KDTREE_POINT_GENERATORS_
#define _KDTREE_POINT_GENERATORS_

#include <hxt_vertices.h>
/*#include <kdt_vertices.h>*/

void points_within_axes(vertex_t* vertices, uint32_t npts);

void points_within_cube(vertex_t* vertices, uint32_t npts);

void points_within_cylinder(vertex_t* vertices, uint32_t npts, double h);

void points_from_Liu(vertex_t* vertices);

void points_within_planes(vertex_t* vertices, uint32_t npts);

void points_within_paraboloid(vertex_t* vertices, uint32_t npts);

void points_within_spiral(vertex_t* vertices, uint32_t npts);

void points_around_saddle(vertex_t* vertices, uint32_t npts);

#endif // _KDTREE_POINT_GENERATORS_
