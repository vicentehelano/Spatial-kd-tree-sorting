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

#ifndef _KDTREE_VERTICES_
#define _KDTREE_VERTICES_

#include "hxt_tetrahedra.h"

// Estrutura que representa uma árvore-KD.
typedef struct kd_node_t_struct {
	vertex_t* vertex;				        // Ponto associado ao nó da árvore.
	int axis;			                    // Campo que indica a dimensão pela qual a árvore KD divide o conjunto de pontos.
	struct kd_node_t_struct *esquerdo;		// Ponteiro para o filho esquerdo da Árvore KD.
	struct kd_node_t_struct *direito;		// Ponteiro para o filho direito da Árvore KD.
} kd_node_t;

// Estrutura para representar um nó da fila
typedef struct queue_node_t_struct {
	kd_node_t* node;
	struct queue_node_t_struct* next;
} queue_node_t;

// Estrutura para representar uma fila
typedef struct {
	queue_node_t* front;
	queue_node_t* rear;
} Queue;

/* biased randomized insertion order using a kd-tree */
status_t KDT_vertices_BRIO(bbox_t* bbox, vertex_t* vertices, uint32_t n);

#endif
