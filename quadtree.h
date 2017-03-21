// quadratic tree node
struct q_node{
	int num_stars;
	int * idx; // indexes in the star array, # = num_stars
	double border[4]; // 0 -- l, 1 -- r, 2 -- b, 3 -- u
	struct q_node * child[4]; // clockwise
};

typedef struct q_node q_node_t;

/* clockwise: 
0 -- upper right, 1 -- bottom right, 
2 -- bottom left, 3 -- upper left*/

void create_q_tree(int level, q_node_t ** curr_node, 
	double * x_arr, double * y_arr);

void delete_q_tree(int level, q_node_t ** curr_node);

void print_q_tree(int level, q_node_t * curr_node,
	double * x_arr, double * y_arr);

int main();