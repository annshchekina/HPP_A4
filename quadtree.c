#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "barneshut.h"

int N = 9;

// 0x5 child address appears out of nowhere, it passes != NULL test and when we access its fields as if it was a struct we get a segmentation fault
// never a problem on linux
// 0x5 never appears in addresses returned by malloc

void create_q_tree(int level, q_node_t ** curr_node, 
	double * x_arr, double * y_arr) 
{	
	if (*curr_node == NULL)
	{
		if (level != 0) return;
		// initialize
		*curr_node = (q_node_t *)malloc(sizeof(q_node_t));
		(*curr_node)->idx = (int *)malloc(N * sizeof(int));
		assert(*curr_node != NULL);
		assert((*curr_node)->idx != NULL);
		
		for (int i = 0; i < 4; i++)
		{
			if (i%2 == 0) 
				(*curr_node)->border[i] = 0;
			else
				(*curr_node)->border[i] = 1;
		}		
		(*curr_node)->num_stars = N;
		for (int i = 0; i < N; i++)
			(*curr_node)->idx[i] = i; 	
	}
	
	// not interesred in further detalisation 
	if ((*curr_node)->num_stars == 1) return;
	
	int curr_N = (*curr_node)->num_stars;	
	int * child_stars[4]; 	
	int child_num[4]; // number of stars there
	for (int i = 0; i < 4; i++) 
	{
		child_num[i] = 0;
		// !!!!!!!!!!!!!!!!!!
		(*curr_node)->child[i] = NULL;
		// !!!!!!!!!!!!!!!!!!
		child_stars[i] = malloc(curr_N * sizeof(int));
	}
	
	double d_border[2]; // 0 -- x, 1 -- y, decision border
	
	d_border[0] = (*curr_node)->border[1] - 
		0.5 * ((*curr_node)->border[1] - (*curr_node)->border[0]);
	d_border[1] = (*curr_node)->border[3] - 
		0.5 * ((*curr_node)->border[3] - (*curr_node)->border[2]);
	
	for (int i = 0; i < curr_N; i++) 
	{	
		int arr_idx = (*curr_node)->idx[i], idx_hit;
		
		if (x_arr[arr_idx] < d_border[0])
		{
			if (y_arr[arr_idx] < d_border[1])
				idx_hit = 2;					
			else 
				idx_hit = 3;			
		}
		else 
		{
			if (y_arr[arr_idx] < d_border[1])
				idx_hit = 1;			
			else 
				idx_hit = 0;		
		}
		child_stars[idx_hit][child_num[idx_hit]] = arr_idx;
		child_num[idx_hit]++;	
	}
	
	double child_border[4][4]; 
	// first index -- child, second -- as in border 
	child_border[0][0] =  d_border[0];
	child_border[0][1] =  (*curr_node)->border[1];
	child_border[0][2] =  d_border[1];
	child_border[0][3] =  (*curr_node)->border[3];
	
	child_border[1][0] =  d_border[0];
	child_border[1][1] =  (*curr_node)->border[1];
	child_border[1][2] =  (*curr_node)->border[2];
	child_border[1][3] =  d_border[1];
	
	child_border[2][0] =  (*curr_node)->border[0];
	child_border[2][1] =  d_border[0];
	child_border[2][2] =  (*curr_node)->border[2];
	child_border[2][3] =  d_border[1];
	
	child_border[3][0] =  (*curr_node)->border[0];
	child_border[3][1] =  d_border[0];
	child_border[3][2] =  d_border[1];
	child_border[3][3] =  (*curr_node)->border[3];
	
	for (int i = 0; i < 4; i++) 
	{
		if (child_num[i] != 0) // otherwise (*curr_node)->child[i] == NULL
		{
			// !!!!!!!!!!!!!!!!!!
			(*curr_node)->child[i] = malloc(sizeof(q_node_t));
			assert((*curr_node)->child[i] != NULL);
			// !!!!!!!!!!!!!!!!!!
			(*curr_node)->child[i]->idx = malloc(child_num[i] * sizeof(int));
			assert((*curr_node)->child[i]->idx != NULL);
			(*curr_node)->child[i]->num_stars = child_num[i];
			for (int j = 0; j < 4; j++) 
				(*curr_node)->child[i]->border[j] = child_border[i][j];		
			for (int j = 0; j < child_num[i]; j++) 
				(*curr_node)->child[i]->idx[j] = child_stars[i][j];	
		}
		printf("%p\n", (*curr_node)->child[i]);
	}
		
	
	for (int i = 0; i < 4; i++)
		free(child_stars[i]);	
		
	for (int i = 0; i < 4; i++)
		create_q_tree(level + 1, &((*curr_node)->child[i]), x_arr, y_arr);
}

void delete_q_tree(int level, q_node_t ** curr_node)
{
	printf("-----------\nLevel: %d\n%p\n", level, *curr_node);
	if (*curr_node == NULL) return;
	for(int i = 0; i < 4; i++) 
	{
		// when accessing -> error 11
		printf("%p\n", (*curr_node)->child[i]);
		if ((*curr_node)->child[i] != NULL)
		{
			printf("--\n");
			printf("%p, at %p\n", (*curr_node)->child[i], &((*curr_node)->child[i]));
			delete_q_tree(level + 1, &((*curr_node)->child[i]));
		}
	}
		
			
	free((*curr_node)->idx);
	free(*curr_node);
}

void print_q_tree(int level, q_node_t * curr_node, 
	double * x_arr, double * y_arr)
{
	if (curr_node == NULL) return;
	
	printf("--------------------\nLevel: %d\n", level);
	for(int i = 0; i < 4; i++)
	{
		if (curr_node->child[i] == NULL) continue;
		
		int j_n = curr_node->child[i]->num_stars;
		for(int j = 0; j < j_n; j++)
		{
			int arr_idx = curr_node->child[i]->idx[j];
			double * border = curr_node->child[i]->border;
			printf("Child %d: (%lf, %lf) indexed %d, Borders: (%lf, %lf) x (%lf, %lf)\n", 
				i, x_arr[arr_idx], y_arr[arr_idx], arr_idx,
				border[0], border[1], border[2], border[3]);
		}
			
	}	
	
	for(int i = 0; i < 4; i++)
		print_q_tree(level+1, curr_node->child[i], x_arr, y_arr);	
}

int main()
{
	double x_arr[N], y_arr[N];
	for (int i = 0; i < N; i++)
	{
		x_arr[i] = (float)rand()/(float)RAND_MAX;
		y_arr[i] = (float)rand()/(float)RAND_MAX;
		printf("%d: (%lf, %lf)\n", i, x_arr[i], y_arr[i]);
	}
	
	q_node_t * head_q = NULL;
	
	create_q_tree(0, &head_q, x_arr, y_arr);
	//print_q_tree(0, head_q, x_arr, y_arr);
	delete_q_tree(0, &head_q);

	return 0;
}