// quadratic tree node
struct q_node{
	struct q_node * child[4]; // clockwisen
	double border[4]; // 0 -- l, 1 -- r, 2 -- b, 3 -- u
	double c_mass_x, c_mass_y; // center of mass
	double mass; // total
	int internal; // 1 -- contains multiple stars 
};

// do you really need internal field? if internal, move forward down
// if not, all child pointers are 0, checking internal is faster? 

typedef struct q_node q_node_t;

// way to move it to barneshut.h?
// star element
struct star{	
	double x;
	double y;
	double m;
	double vx;
	double vy;
};

typedef struct star star_t;

/* clockwise: 
0 -- upper right, 1 -- bottom right, 
2 -- bottom left, 3 -- upper left */

void create_q_tree(const int level, q_node_t ** curr_node, 
	const star_t * stars, const int * indexes, const int num_stars);

void delete_q_tree(q_node_t ** curr_node);
