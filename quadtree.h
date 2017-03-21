// quadratic tree node
struct q_node{
	struct q_node * child[4]; // clockwise
	double border[4]; // 0 -- l, 1 -- r, 2 -- b, 3 -- u
	double c_mass_x, c_mass_y; // center of mass
	double mass; // total
	int internal; // 1 -- contains multiple stars
};

typedef struct q_node q_node_t;

/* clockwise: 
0 -- upper right, 1 -- bottom right, 
2 -- bottom left, 3 -- upper left*/

void create_q_tree(int level, q_node_t ** curr_node, 
	const double * x_coords, const double * y_coords, 
	const double * masses, const int * indexes, int num_stars);

void delete_q_tree(int level, q_node_t ** curr_node);

int main();