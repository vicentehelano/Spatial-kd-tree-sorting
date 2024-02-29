/*  Copyright (C) 2023 Rafael Vanali                                        *
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
Author: Rafael Vanali (email@user.com)                                      */

#include <kdt_vertices.h>

#define MAX3_IDX(a,b,c) (((a) > (b))?(((a) > (c))?0:2):(((b) > (c))?1:2))
#define MIN3_IDX(a,b,c) (((a) < (b))?(((a) < (c))?0:2):(((b) < (c))?1:2))

// Função para criar uma fila vazia
Queue* createQueue()
{
	Queue* queue = (Queue*)malloc(sizeof(Queue));

	// Tratar erro de alocação
	if (queue == NULL)
		exit(1);

	queue->front = NULL;
	queue->rear = NULL;
	return queue;
}

// Função para enfileirar um nó na fila
void __enqueue(Queue* queue, kd_node_t* node)
{
	queue_node_t* newNode = (queue_node_t*)malloc(sizeof(queue_node_t));

	// Tratar erro de alocação
	if ( newNode == NULL )
		exit(1);

	newNode->node = node;
	newNode->next = NULL;

	if ( queue->rear == NULL )
	{
		queue->front = newNode;
		queue->rear = newNode;
	}

	else
	{
		queue->rear->next = newNode;
		queue->rear = newNode;
	}
}

// Função para desenfileirar um nó da fila
kd_node_t* __dequeue(Queue* queue)
{
	if ( queue->front == NULL )
		return NULL; // Fila vazia

	queue_node_t* frontNode = queue->front;
	kd_node_t* node = frontNode->node;

	queue->front = frontNode->next;
	if ( queue->front == NULL )
		queue->rear = NULL;

	free( frontNode );

	return node;
}

// Função para destruir a fila
void __destroyQueue(Queue* queue)
{
	while (queue->front != NULL)
	{
		__dequeue(queue);
	}

	free(queue);
}

// Função para ordenar em largura a partir da árvore KD diretamente no array
static status_t __KDT_vertices_breadth_first_sort( vertex_t* const __restrict__ array, kd_node_t* raiz )
{
	if ( raiz == NULL )
		return HXT_STATUS_ERROR;

	Queue* queue = createQueue();
	__enqueue(queue, raiz);

	uint32_t index = 0;

	while ( queue->front != NULL )
	{
		kd_node_t* currentNode = __dequeue(queue);

		// Enfileira os filhos do nó atual se existirem
		if ( currentNode->esquerdo != NULL )
			__enqueue(queue, currentNode->esquerdo);

		if ( currentNode->direito != NULL )
			__enqueue(queue, currentNode->direito);

		// Copia o elemento do nó atual para o array
		array[index] = *(currentNode->vertex);
		index++;
	}

	__destroyQueue(queue);
	return HXT_STATUS_OK;
}

// Função para trocar dois vértices
void __swapVertices( vertex_t *a, vertex_t *b )
{
	double tmp[3];
    memcpy(tmp,  a->coord, sizeof(tmp));
    memcpy(a->coord, b->coord, sizeof(tmp));
    memcpy(b->coord, tmp,  sizeof(tmp));
}

// Função para selecionar um pivô aleatório e particionar os pontos
int __partition(vertex_t* vertices, int left, int right, int axis)
{
	// Seleciona um pivô aleatório
	int pivotIndex = left + rand() % (right - left + 1);
	vertex_t pivot = vertices[pivotIndex];

	// Move o pivô para o final do array temporariamente
	__swapVertices(&vertices[pivotIndex], &vertices[right]);

	// Particiona os pontos em torno do pivô
	int i = left - 1;
	for (int j = left; j < right; j++)
	{
		if (vertices[j].coord[axis] <= pivot.coord[axis])
		{
			i++;
			__swapVertices(&vertices[i], &vertices[j]);
		}
	}

	__swapVertices(&vertices[i + 1], &vertices[right]); // Move o pivô de volta para a posição correta

	return i + 1;
}

// Função para encontrar a mediana dos pontos
uint64_t __KDT_cut_along_axis(vertex_t* vertices, uint64_t n, int axis)
{
	uint64_t left  = 0;
	uint64_t right = n - 1;
	uint64_t k  = n/2;
	uint64_t pivot_Index;

	while (left < right) {
		pivot_Index = __partition(vertices, left, right, axis);

		uint64_t length = pivot_Index - left + 1;
		if (length == k) {// A mediana foi encontrada
			return pivot_Index;
		} else if (k < length) // median is on the pivot's left
			right = pivot_Index - 1;
		else {
			left = pivot_Index + 1;
			k -= length;
		}
	}

	if (left == right) {
        return left;
    }

    fprintf(stderr, "cut-longest-edge failed.\n");
	return -1;
}

int __KDT_get_longest_axis(bbox_t bbox)
{
    double dx = bbox.max[0] - bbox.min[0];
    double dy = bbox.max[1] - bbox.min[1];
    double dz = bbox.max[2] - bbox.min[2];
    return MAX3_IDX(dx,dy,dz);
}

kd_node_t *__KDT_vertices_build_kdtree(bbox_t bbox, vertex_t* vertices, const uint32_t n)
{
	if ( n == 0 )
		return NULL;

	// Determina a direção com maior variação
	int axis = __KDT_get_longest_axis(bbox);

    // Calcula a mediana usando o algoritmo de seleção de mediana
	int median = __KDT_cut_along_axis(vertices, n, axis);

	// Cria o nó atual
	kd_node_t *no = (kd_node_t*) malloc(sizeof(kd_node_t));
	if (no == NULL)
	{
		fprintf(stderr, "Falha na alocação de memória\n");
		exit(1);
	}

	no->vertex = &vertices[median];
	//no->axis = axis;

	// Calcula os bounding boxes dos retangulos esquerdo e direito
	bbox_t left_bbox  = bbox;
	bbox_t right_bbox = bbox;

    left_bbox.max[axis]  = no->vertex->coord[axis];
    right_bbox.min[axis] = no->vertex->coord[axis];

	// Constrói de forma recursiva a subárvore esquerda
	no->esquerdo = __KDT_vertices_build_kdtree(left_bbox, vertices, median);

	// Constrói de forma recursiva a subárvore direita
	no->direito = __KDT_vertices_build_kdtree(right_bbox, vertices + median + 1, n - median - 1);

	return no;
}

kd_node_t *KDT_vertices_build_kdtree( bbox_t bbox, vertex_t* vertices, const uint32_t n )
{
	return __KDT_vertices_build_kdtree(bbox, vertices, n);
}

// Função PRINCIPAL para ordenar o array de vertices usando a árvore KD
static status_t KDT_vertices_sort( bbox_t bbox, vertex_t* const __restrict__ array, const uint32_t n )
{
    kd_node_t* raiz = KDT_vertices_build_kdtree(bbox, array, n); // Construa a árvore KD

    // Verifica se a árvore KD foi construída correntamente
	if ( raiz == NULL )
		return HXT_STATUS_ERROR;

    vertex_t* buffer = NULL;
    HXT_CHECK(
            HXT_malloc( &buffer, n*sizeof( vertex_t )));

    __KDT_vertices_breadth_first_sort(buffer, raiz);

    memcpy(array, buffer, n*sizeof(vertex_t));

    HXT_free( &buffer );
    return HXT_STATUS_OK;
}

status_t KDT_vertices_BRIO( bbox_t bbox, vertex_t* vertices, const uint32_t n )
{
	status_t sortStatus = KDT_vertices_sort( bbox, vertices, n );
	if ( sortStatus != HXT_STATUS_OK )
		return sortStatus;

	return HXT_STATUS_OK;
}
