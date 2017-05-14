#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <assert.h>
#include <sys/time.h>
#include "quadtree.h"

#define TREE_TEST	0

const int N = 5;

// indexes - star numbers from the array in the region
// stars pointer passed stays the same
void create_q_tree(const int level, q_node_t ** curr_node, 
	const star_t * stars, const int * indexes, const int num_stars) 
{	
	if (*curr_node == NULL)
	{
		if (level != 0) return;

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
		
	for (int i = 0; i < 4; i++) 
		(*curr_node)->child[i] = NULL;

	if (num_stars == 1) 
	{
		#if TREE_TEST
		printf("Star (%lf, %lf) to (%lf, %lf) x (%lf, %lf) on level %d\n", 
			stars[indexes[0]].x, stars[indexes[0]].y,
			(*curr_node)->border[0], (*curr_node)->border[1], 
			(*curr_node)->border[2], (*curr_node)->border[3], level);
		#endif
		
		(*curr_node)->mass = stars[indexes[0]].m;
		(*curr_node)->c_mass_x = stars[indexes[0]].x;
		(*curr_node)->c_mass_y = stars[indexes[0]].y;
		(*curr_node)->internal = 0;
		return;
	}
	
	// center of mass
	double total_mass = 0, c_mass_x = 0, c_mass_y = 0;
	for (int i = 0; i < num_stars; i++) 
	{
		int arr_idx = indexes[i];
		double mass = stars[arr_idx].m;
		total_mass += mass;
		c_mass_x += mass * stars[arr_idx].x;
		c_mass_y += mass * stars[arr_idx].y;
	}
	(*curr_node)->mass = total_mass;
	(*curr_node)->c_mass_x = c_mass_x / total_mass;
	(*curr_node)->c_mass_y = c_mass_y / total_mass;
	
	(*curr_node)->internal = 1;
	
	#if TREE_TEST
	printf("\nCenter (%lf, %lf) with mass %lf on level %d\n\n", 
		(*curr_node)->c_mass_x, (*curr_node)->c_mass_y, (*curr_node)->mass, level);
	#endif
	
	int child_indexes[4][num_stars]; 	
	int child_num_stars[4] = {0, 0, 0, 0};
	
	double d_border[2]; // 0 - x, 1 - y, decision border	
	d_border[0] = (*curr_node)->border[1] - 
		0.5 * ((*curr_node)->border[1] - (*curr_node)->border[0]);
	d_border[1] = (*curr_node)->border[3] - 
		0.5 * ((*curr_node)->border[3] - (*curr_node)->border[2]);
	
	for (int i = 0; i < num_stars; i++) 
	{	
		int arr_idx = indexes[i], idx_hit;		
		if (stars[arr_idx].x < d_border[0])
		{
			if (stars[arr_idx].y < d_border[1])
				idx_hit = 2;					
			else 
				idx_hit = 3;			
		}
		else 
		{
			if (stars[arr_idx].y < d_border[1])
				idx_hit = 1;			
			else 
				idx_hit = 0;		
		}
		child_indexes[idx_hit][child_num_stars[idx_hit]] = arr_idx;
		child_num_stars[idx_hit]++;	
	}
	
	double child_border[4][4]; // first index - child, second - as in border 
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
		// for empty squares (*curr_node)->child[i] == NULL
		if (child_num_stars[i] != 0) 
		{
			((*curr_node)->child)[i] = (q_node_t *)malloc(sizeof(q_node_t));
			assert((*curr_node)->child[i] != NULL);

			for (int j = 0; j < 4; j++) 
				(*curr_node)->child[i]->border[j] = child_border[i][j];		
		}
	}
	for (int i = 0; i < 4; i++) 
		create_q_tree(level + 1, &((*curr_node)->child[i]), 
			stars, &(child_indexes[i][0]), child_num_stars[i]);				
}

void delete_q_tree(q_node_t ** curr_node)
{
	if (*curr_node == NULL) return;

	for(int i = 0; i < 4; i++) 
		if (((*curr_node)->child)[i] != NULL)
			delete_q_tree(&(((*curr_node)->child)[i]));
		
	free(*curr_node);
	*curr_node = NULL;
}

#if TREE_TEST
int main()
{
	star_t stars[N];
	int indexes[N];

	for (int i = 0; i < N; i++)
	{
		stars[i].x = (float)rand()/(float)RAND_MAX;
		stars[i].y = (float)rand()/(float)RAND_MAX;
		stars[i].m = rand() % 10;
		indexes[i] = i; // shows which stars from the array belong to the region, init 0 level
		printf("%d: (%lf, %lf) with mass %lf\n", i, stars[i].x, stars[i].y, stars[i].m);
	}
	
	q_node_t * head_q = NULL;	
	create_q_tree(0, &head_q, stars, indexes, N);
	delete_q_tree(&head_q);

	return 0;
}
#endif
