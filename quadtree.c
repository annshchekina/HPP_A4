#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "quadtree.h"

int N = 5;

// 0x5 child address appears out of nowhere, it passes != NULL test and when we access its fields as if it was a struct we get a segmentation fault
// never a problem on linux
// 0x5 never appears in addresses returned by malloc

// indexes show which stars from the array belong to the region
void create_q_tree(int level, q_node_t ** curr_node, 
	const double * x_coords, const double * y_coords, 
	const double * masses, const int * indexes, int num_stars) 
{	
	if (*curr_node == NULL)
	{
		if (level != 0) return;
		// initialize
		*curr_node = (q_node_t *)malloc(sizeof(q_node_t));
		assert(*curr_node != NULL);	
		for (int i = 0; i < 4; i++)
		{
			if (i%2 == 0) 
				(*curr_node)->border[i] = 0;
			else
				(*curr_node)->border[i] = 1;
		}			
	}
	
	if (num_stars == 1) 
	{
		printf("(%lf, %lf) with mass %lf to (%lf, %lf) x (%lf, %lf) on level %d\n", 
			x_coords[indexes[0]], y_coords[indexes[0]], masses[indexes[0]],
			(*curr_node)->border[0], (*curr_node)->border[1], 
			(*curr_node)->border[2], (*curr_node)->border[3], level);
					
		(*curr_node)->mass = masses[0];
		(*curr_node)->c_mass_x = x_coords[0];
		(*curr_node)->c_mass_y = y_coords[0];
		return;
	}
	
	// calculate the center of mass
	double total_mass = 0, c_mass_x = 0, c_mass_y = 0;
	for (int i = 0; i < num_stars; i++) 
	{
		int arr_idx = indexes[i];
		double mass = masses[arr_idx];
		total_mass += mass;
		c_mass_x += mass * x_coords[arr_idx];
		c_mass_y += mass * y_coords[arr_idx];
	}
	(*curr_node)->mass = total_mass;
	(*curr_node)->c_mass_x = c_mass_x / total_mass;
	(*curr_node)->c_mass_y = c_mass_y / total_mass;
	
	printf("Center (%lf, %lf) on level %d\n", 
		(*curr_node)->c_mass_x, (*curr_node)->c_mass_y, level);
	
	int * child_indexes[4]; 	
	int child_num_stars[4];
	for (int i = 0; i < 4; i++) 
	{
		// !!!!!!!!!!!!!!!!!!
		(*curr_node)->child[i] = NULL;
		// !!!!!!!!!!!!!!!!!!
		child_num_stars[i] = 0;
		child_indexes[i] = malloc(num_stars * sizeof(int));
	}
	
	double d_border[2]; // 0 -- x, 1 -- y, decision border	
	d_border[0] = (*curr_node)->border[1] - 
		0.5 * ((*curr_node)->border[1] - (*curr_node)->border[0]);
	d_border[1] = (*curr_node)->border[3] - 
		0.5 * ((*curr_node)->border[3] - (*curr_node)->border[2]);
	
	for (int i = 0; i < num_stars; i++) 
	{	
		int arr_idx = indexes[i], idx_hit;		
		if (x_coords[arr_idx] < d_border[0])
		{
			if (y_coords[arr_idx] < d_border[1])
				idx_hit = 2;					
			else 
				idx_hit = 3;			
		}
		else 
		{
			if (y_coords[arr_idx] < d_border[1])
				idx_hit = 1;			
			else 
				idx_hit = 0;		
		}
		child_indexes[idx_hit][child_num_stars[idx_hit]] = arr_idx;
		child_num_stars[idx_hit]++;	
	}
	
	double child_border[4][4]; // first index -- child, second -- as in border 
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
		if (child_num_stars[i] != 0) // otherwise (*curr_node)->child[i] == NULL
		{
			// !!!!!!!!!!!!!!!!!!
			(*curr_node)->child[i] = malloc(sizeof(q_node_t));
			assert((*curr_node)->child[i] != NULL);
			// !!!!!!!!!!!!!!!!!!
			for (int j = 0; j < 4; j++) 
				(*curr_node)->child[i]->border[j] = child_border[i][j];		
		}
		// printf("%p\n", (*curr_node)->child[i]);
	}
	// after loop fusion error 11 in delete
	for (int i = 0; i < 4; i++) 
	{
		create_q_tree(level + 1, &((*curr_node)->child[i]), 
			x_coords, y_coords, masses, child_indexes[i], child_num_stars[i]);
			
		free(child_indexes[i]);	
	}
}

void delete_q_tree(int level, q_node_t ** curr_node)
{
	// printf("-----------\nLevel: %d\n%p\n", level, *curr_node);
	if (*curr_node == NULL) return;
	for(int i = 0; i < 4; i++) 
	{
		// when accessing -> error 11
		//printf("%p\n", (*curr_node)->child[i]);
		if ((*curr_node)->child[i] != NULL)
		{
			// printf("--\n");
			// printf("%p, at %p\n", (*curr_node)->child[i], &((*curr_node)->child[i]));
			delete_q_tree(level + 1, &((*curr_node)->child[i]));
		}
	}
		
	free(*curr_node);
}

int main()
{
	double x_arr[N], y_arr[N], mass[N];
	int indexes[N];
	for (int i = 0; i < N; i++)
	{
		x_arr[i] = (float)rand()/(float)RAND_MAX;
		y_arr[i] = (float)rand()/(float)RAND_MAX;
		mass[i] = rand() % 10;
		indexes[i] = i;
		printf("%d: (%lf, %lf) with mass %lf\n", i, x_arr[i], y_arr[i], mass[i]);
	}
	
	q_node_t * head_q = NULL;
	
	create_q_tree(0, &head_q, x_arr, y_arr, mass, indexes, N);
	delete_q_tree(0, &head_q);

	return 0;
}