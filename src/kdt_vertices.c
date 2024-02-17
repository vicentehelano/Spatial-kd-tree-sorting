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

// Função para trocar dois vértices
void __swapVertices( vertex_t *a, vertex_t *b )
{
	vertex_t temp = *a;
	*a = *b;
	*b = temp;
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
void __quick_select(vertex_t* vertices, int n, int k, int axis)
{
	int left = 0;
	int right = n - 1;
	while (left <= right)
	{
		int pivot_Index = __partition(vertices, left, right, axis);

		if (pivot_Index == k) // A mediana foi encontrada
			return;

		else if (pivot_Index < k)
			left = pivot_Index + 1;

		else
			right = pivot_Index - 1;
	}
}

kd_node_t *__KDT_vertices_build_kdtree(int axis, vertex_t* vertices, const uint32_t n)
{
	if ( n == 0 )
		return NULL;

	// Calcula a mediana usando o algoritmo de seleção de mediana
	int median = n / 2;

	__quick_select(vertices, n, median, axis);

	// Cria o nó atual
	kd_node_t *no = (kd_node_t*) malloc(sizeof(kd_node_t));
	if (no == NULL)
	{
		fprintf(stderr, "Falha na alocação de memória\n");
		exit(1);
	}

	no->vertex = &vertices[median];
	no->axis = axis;

	// Calcula a nova dimensão para a próxima divisão
	int next_axis = (axis + 1) % 3;

	// Constrói de forma recursiva a subárvore esquerda
	no->esquerdo = __KDT_vertices_build_kdtree(next_axis, vertices, median);

	// Constrói de forma recursiva a subárvore direita
	no->direito = __KDT_vertices_build_kdtree(next_axis, vertices + median + 1, n - median - 1);

	return no;
}




































kd_node_t *KDT_vertices_build_kdtree( bbox_t* bbox, vertex_t* vertices, const uint32_t n )
{
	return __KDT_vertices_build_kdtree(0, vertices, n);
}

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
static status_t __KDT_vertices_breadth_first_sort( vertex_t* const __restrict__ array, const uint32_t n, kd_node_t* raiz )
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

// Função PRINCIPAL para ordenar o array de vertices usando a árvore KD
static status_t KDT_vertices_sort( vertex_t* const __restrict__ array, const uint32_t n )
{
	bbox_t bbox;
    kd_node_t* raiz = KDT_vertices_build_kdtree(&bbox, array, n); // Construa a árvore KD

    // Verifica se a árvore KD foi construída correntamente
	if ( raiz == NULL )
		return HXT_STATUS_ERROR;

    vertex_t* buffer = NULL;
    HXT_CHECK(
            HXT_malloc( &buffer, n*sizeof( vertex_t )));

    __KDT_vertices_breadth_first_sort(buffer, n, raiz);

    memcpy(array, buffer, n*sizeof(vertex_t));

    HXT_free( &buffer );
    return HXT_STATUS_OK;
}

status_t KDT_vertices_BRIO( bbox_t* bbox, vertex_t* vertices, const uint32_t n )
{
	status_t sortStatus = KDT_vertices_sort( vertices, n );
	if ( sortStatus != HXT_STATUS_OK )
		return sortStatus;

	return HXT_STATUS_OK;
}
