#include <stdio.h>
#include <stdlib.h>

int ticks = 0;

/* dfs method fills out the information in visited[][] and postorder[][] */
void dfs(int * adj_matrix, int node, int * visited, int * postorder, int num_nodes) {
	// printf("about to dfs on node: %d\n", node);
	visited[node] = 1;
	for (int i = 0; i < num_nodes; i++) {
		if (visited[i] == 0 && adj_matrix[node * num_nodes + i] == 1) {
			dfs(adj_matrix, i, visited, postorder, num_nodes);
		}
	}
	postorder[node] = ticks;
	// printf("node: %d, ticks: %d\n", node, ticks);
	ticks += 1;
}

void topological_sort(int * permutation, int * visited, int * postorder, int * adj_matrix, int num_nodes) {
	for (int i = 0; i < num_nodes; i++) {
		if (visited[i] == 0) {
			dfs(adj_matrix, i, visited, postorder, num_nodes);
		}
	}
	/* reverse postorder of the dfs of the nodes */
	for (int i = 0; i < num_nodes; i++) { 
		permutation[num_nodes - postorder[i] - 1] = i;
	}
	for (int i = 0; i < num_nodes; i++) {
		// printf("permutation: %d\n", permutation[i]);
	}
}

void delete_edges(int * adj_matrix, int * nodes, int start, int end, int num_nodes) {
	int subset_size = end - start;
	if (subset_size == 1) {
		int indexA = nodes[start];
		int indexB = nodes[end];
		if (adj_matrix[indexA * num_nodes + indexB] == 1 && adj_matrix[indexB * num_nodes + indexA] == 1) {
			adj_matrix[indexA * num_nodes + indexB] = 0;
		}
	} else if (subset_size > 1) {
		/* find middle index of subset to split into more subsets */
		int middle = start + subset_size/ 2; 
		delete_edges(adj_matrix, nodes, start, middle, num_nodes); // setA
		delete_edges(adj_matrix, nodes, middle + 1, end, num_nodes); //setB
		int setA = 0;
		int setB = 0;
		/* check which direction edges to delete */
		for (int i = start; i <= middle; i++) {
			for (int j = middle + 1; j <= end; j++) {
				int indexA = nodes[i];
				int indexB = nodes[j];
				if (adj_matrix[indexA * num_nodes + indexB] == 1) {
					/* there is an edge going from set A to set B */
					setA += 1;
				}
				if (adj_matrix[indexB * num_nodes + indexA] == 1) {
					/* there is an edge going from set B to set A */
					setB += 1;
				}
			}
		}
		/* delete the edges */
		for (int i = start; i <= middle; i++) {
			for (int j = middle + 1; j <= end; j++) {
				int indexA = nodes[i];
				int indexB = nodes[j];
				if (setB >= setA) {
					/* debugging statements 
					if (adj_matrix[indexA * num_nodes + indexB] == 1) {
						printf("deleted back edge from %d to %d\n", i, j);
						printf("edges from setA to setB: %d\n", setA);
						printf("edges from setB to setA: %d\n", setB);
						printf("setA: ");
						for (int k = start; k <= middle; k++) {
							printf("%d, ", nodes[k]);
						}
						printf("\n");
						printf("setB: ");
						for (int k = middle + 1; k <= end; k++) {
							printf("%d, ", nodes[k]);
						}
						printf("\n");
						
					}
					*/
					adj_matrix[indexA * num_nodes + indexB] = 0;
				} else if (setA > setB) {
					/* debugging statements
					if (adj_matrix[indexB * num_nodes + indexA] == 1) {
						printf("deleted back edge from %d to %d\n", j, i);
						printf("edges from setA to setB: %d\n", setA);
						printf("edges from setB to setA: %d\n", setB);
						printf("setA: ");
						for (int k = start; k <= middle; k++) {
							printf("%d, ", nodes[k]);
						}
						printf("\n");
						printf("setB: ");
						for (int k = middle + 1; k <= end; k++) {
							printf("%d, ", nodes[k]);
						}
						printf("\n");
					}
					*/
					adj_matrix[indexB * num_nodes + indexA] = 0;
				}
			}
		}

	}
}

int solver(int * adj_matrix, int * rand_perm, int num_nodes) {

	delete_edges(adj_matrix, rand_perm, 0, num_nodes - 1, num_nodes);
	/* now the adjacency matrix has been modified with the deleted edges 
	   linearize the dag! */
	int * visited = malloc(sizeof(int) * num_nodes); 
	for(int i = 0; i < num_nodes; i++) {
		visited[i] = 0;
	}
	int * postorder = malloc(sizeof(int) * num_nodes);
	topological_sort(rand_perm, visited, postorder, adj_matrix, num_nodes);
	free(visited);
	free(postorder);
	ticks = 0;
	/* now rand_perm is in topological order */
	return num_forward_edges(rand_perm, adj_matrix, num_nodes);

}

/* returns the number of forward edges given an ordering of the nodes in the graph */
int num_forward_edges(int * order, int * adj_matrix, int num_nodes) {
	int forward_edges = 0;
	for (int i = 0; i < num_nodes; i++) {
		for (int j = 0; j < num_nodes; j++) {
			if (adj_matrix[i * num_nodes + j] == 1) {
				// printf("forward edge from %d to %d\n", i, j);
				forward_edges += 1;
			}
		}
	}
	return forward_edges;
}

void edge(int a, int b, int * adj_matrix, int num_nodes) {
	adj_matrix[a * num_nodes + b] = 1;
}

void test_one() {
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	int* visited = malloc(sizeof(int) * num_nodes); 
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		if (i < num_nodes) {
			visited[i] = 0;
		}
		adj_matrix[i] = 0;
	}

	edge(5, 0, adj_matrix, num_nodes);
	edge(4, 0, adj_matrix, num_nodes);
	edge(4, 1, adj_matrix, num_nodes);
	edge(3, 1, adj_matrix, num_nodes);
	edge(2, 3, adj_matrix, num_nodes);
	edge(5, 2, adj_matrix, num_nodes);

	int * postorder = malloc(sizeof(int) * num_nodes);
	int* order = malloc(sizeof(int) * num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		order[i] = i;
	}
	topological_sort(order, visited, postorder, adj_matrix, num_nodes);
	// /* order has the linearized ordered nodes now */
	ticks = 0;
	int e = num_forward_edges(order, adj_matrix, num_nodes);
	printf("num_forward_edges: expected 6, got %d\n", e);
	free(adj_matrix);
	free(visited);
	free(postorder);
	int expected_order[6] = {5, 4, 2, 3, 1, 0};
	for (int i = 0; i < num_nodes; i++) {
		int a = expected_order[i];
		int b = order[i];
		printf("expected %d, got %d\n", a, b);
	}

}

void test_two() {
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	int* visited = malloc(sizeof(int) * num_nodes); 
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		if (i < num_nodes) {
			visited[i] = 0;
		}
		adj_matrix[i] = 0;
	}

	edge(0, 1, adj_matrix, num_nodes);
	edge(0, 2, adj_matrix, num_nodes);
	edge(1, 4, adj_matrix, num_nodes);
	edge(2, 3, adj_matrix, num_nodes);
	edge(2, 4, adj_matrix, num_nodes);
	edge(3, 5, adj_matrix, num_nodes);
	edge(4, 5, adj_matrix, num_nodes);

	int * postorder = malloc(sizeof(int) * num_nodes);
	int* order = malloc(sizeof(int) * num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		order[i] = i;
	}
	topological_sort(order, visited, postorder, adj_matrix, num_nodes);
	// /* order has the linearized ordered nodes now */
	ticks = 0;
	int e = num_forward_edges(order, adj_matrix, num_nodes);
	printf("num_forward_edges: expected 7, got %d\n", e);
	free(adj_matrix);
	free(visited);
	free(postorder);
	int expected_order[6] = {0, 2, 3, 1, 4, 5};
	for (int i = 0; i < num_nodes; i++) {
		int a = expected_order[i];
		int b = order[i];
		printf("expected %d, got %d\n", a, b);
	}
}

void solver_test_one() {
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		adj_matrix[i] = 0;
	}
	/* order = {0, 1, 2, 3, 4, 5} */
	int* order = malloc(sizeof(int) * num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		order[i] = i;
	}

	edge(0, 1, adj_matrix, num_nodes);
	edge(0, 2, adj_matrix, num_nodes);
	edge(1, 4, adj_matrix, num_nodes);
	edge(2, 3, adj_matrix, num_nodes);
	edge(2, 4, adj_matrix, num_nodes);
	edge(3, 5, adj_matrix, num_nodes);
	edge(4, 5, adj_matrix, num_nodes);

	int edges = solver(adj_matrix, order, num_nodes);
	printf("solver result: expected 7, got %d\n", edges);
}

void solver_test_two() {
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	int* visited = malloc(sizeof(int) * num_nodes); 
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		adj_matrix[i] = 0;
	}
	int* order = malloc(sizeof(int) * num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		order[i] = i;
	}

	edge(5, 0, adj_matrix, num_nodes);
	edge(4, 0, adj_matrix, num_nodes);
	edge(4, 1, adj_matrix, num_nodes);
	edge(3, 1, adj_matrix, num_nodes);
	edge(2, 3, adj_matrix, num_nodes);
	edge(5, 2, adj_matrix, num_nodes);

	int edges = solver(adj_matrix, order, num_nodes);
	printf("solver result: expected 6, got %d\n", edges);
}

void solver_test_three() {
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		adj_matrix[i] = 0;
	}
	/* order = {0, 1, 2, 3, 4, 5} */
	int* order = malloc(sizeof(int) * num_nodes);
	for (int i = 0; i < num_nodes; i++) {
		order[i] = i;
	}

	edge(0, 1, adj_matrix, num_nodes);
	edge(0, 2, adj_matrix, num_nodes);
	edge(1, 4, adj_matrix, num_nodes);
	edge(2, 3, adj_matrix, num_nodes);
	edge(2, 4, adj_matrix, num_nodes);
	edge(3, 5, adj_matrix, num_nodes);
	edge(4, 5, adj_matrix, num_nodes);
	/* back edges */
	edge(3, 2, adj_matrix, num_nodes);
	edge(4, 1, adj_matrix, num_nodes);
	edge(5, 0, adj_matrix, num_nodes);

	int edges = solver(adj_matrix, order, num_nodes);
	printf("solver result: expected 7, got %d\n", edges);
}


int main( int argc, const char* argv[]) {
	printf("Test One: \n");
	test_one();
	printf("Test Two: \n");
	test_two();
	printf("Solver Test One: \n");
	solver_test_one();
	printf("Solver Test Two: \n");
	solver_test_two();
	printf("Solver Test Three: \n");
	solver_test_three();
}

