#include "quadtree.h"

double get_wall_seconds();

void calculation(star_t * __restrict stars, star_t * __restrict stars_buffer, 
	const q_node_t * __restrict quadtree, const int N_stars, const double delta_t);

void galsim(const int N, const char * filename, 
	const int nsteps, const double delta_t, const int graphics);