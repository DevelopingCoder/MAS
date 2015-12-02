#include <stdio.h>
#include <stdlib.h>

int ticks = 0;

/* dfs method fills out the information in visited[][] and postorder[][] */
void dfs(int * adj_matrix, int node, int * visited, int * postorder, int num_nodes) {
	visited[node] = 1;
	for (int i = 0; i < num_nodes; i++) {
		if (visited[i] == 0 && adj_matrix[node * num_nodes + i] == 1) {
			dfs(adj_matrix, i, visited, postorder, num_nodes);
		}
	}
	postorder[node] = ticks;
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
}

int solver(int * adj_matrix, int * rand_perm, int start, int end) {
	int num_nodes = end - start
	if (num_nodes == 1)
	{
		return 0;
	}
	int half =  (end - start)/2
	solver(adj_matrix, rand_perm, start, half)
	solver(adj_matrix, rand_perm, half, end)

	int setA;
	int setB;
	for (int i = start; i < half; ++i)
	{
		for (int j = half; j < end; ++j)
		{
			if (adj_matrix[i*num_nodes + j])
			{
				setA += 1;
			}
			else if (adj_matrix[j*num_nodes + i])
			{
				setB += 1;
			}	
		}
	}

	int deleted;
	if (setA >= setB)
	{
		for (int i = start; i < half; ++i)
		{
			for (int j = half; j < end; ++j)
			{
				adj_matrix[j*num_nodes + i] = 0;
			}
		}
		deleted = setB;
	}

	if (setA < setB)
	{
		for (int i = start; i < half; ++i)
		{
			for (int j = half; j < end; ++j)
			{
				adj_matrix[j*num_nodes + i] = 0;
			}
		}
		deleted = setA;
	}

	int subset_size = 1; 

	while (subset_size < num_nodes) {
		
		for (int s = 0; s < num_nodes; s += 2*subset_size)
		{
			int setA = 0;
			int setB = 0;
			/* find all edges from A to B and B to A */
			for (int i = s; i < s + subset_size; i++)
			{
				for (int j = s + subset_size; j < s + subset_size*2; j++)
				{
					if (i < num_nodes && j < num_nodes) {
						/* indices of the random permutation */
						int index_a = rand_perm[i];
						int index_b = rand_perm[j];
						if (adj_matrix[index_a * num_nodes + index_b] == 1) { setA += 1; }	
						if (adj_matrix[index_b * num_nodes + index_a] == 1) { setB += 1; }
					}
				}
			}
			/* delete smaller subset of edges going between sets */
			for (int i = s; i < s + subset_size/2; i++)
			{
				for (int j = s + subset_size/2; j < s + subset_size; j++)
				{
					int index_a = rand_perm[i];
					int index_b = rand_perm[j];
					/* check if there are more edges going from A to B or B to A */
					if (setA >= setB && adj_matrix[index_b * num_nodes + index_a] == 1) { 
						adj_matrix[index_b * num_nodes + index_a] = 0; 
					}
					else if (setA < setB && adj_matrix[index_a * num_nodes + index_b] == 1) { 
						adj_matrix[index_a * num_nodes + index_b] = 0; 
					}
				}
			}
		}
		subset_size = subset_size * 2;
	}
	/* now the adjacency matrix has been modified with the deleted edges 
	   linearize the dag! */
	int * visited = malloc(sizeof(int) * num_nodes); 
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
	printf("about to do test_one\n");
	int num_nodes = 6;
	int* adj_matrix = malloc(sizeof(int) * num_nodes * num_nodes);
	int* visited = malloc(sizeof(int) * num_nodes); 
	for (int i = 0; i < num_nodes * num_nodes; i++) {
		if (i < num_nodes) {
			visited[i] = 0;
		}
		adj_matrix[i] = 0;
	}

	// edge(5, 0, adj_matrix, num_nodes);
	// edge(0, 4, adj_matrix, num_nodes);
	// edge(4, 1, adj_matrix, num_nodes);
	// edge(3, 1, adj_matrix, num_nodes);
	// edge(2, 3, adj_matrix, num_nodes);
	// edge(5, 2, adj_matrix, num_nodes);

	// int * postorder = malloc(sizeof(int) * num_nodes);
	int order[6] = {0, 1, 2, 3, 4, 5};
	// topological_sort(order, visited, postorder, adj_matrix, num_nodes);
	// /* rand_perm has the order now */
	// ticks = 0;
	free(adj_matrix);
	free(visited);
	// free(postorder);
	int expected_order[6] = {5, 4, 2, 3, 1, 0};
	for (int i = 0; i < num_nodes; i++) {
		int a = expected_order[i];
		int b = order[i];
		printf("expected %d, got %d\n", a, b);
	}
}

int main( int argc, const char* argv[]) {
	test_one();
}

