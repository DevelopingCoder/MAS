#include <stdio.h>
#include <time.h>
#include <stdlib.h>


// Fisher-Yates shuffle taken from http://www.dispersiondesign.com/articles/algorithms/shuffle_array_order
void shuffle(int * a, int n)
{
      int i = n - 1;
      int j, temp;                
      while (i > 0)
      {
            j = ((rand() % (i + 1)) * time(NULL)) % (i + 1);
            temp = a[i];
            a[i] = a[j];
            a[j] = temp;
            i = i - 1;
      }
}

//PQ Implementation taken from http://rosettacode.org/wiki/Priority_queue#C

typedef struct { void * data; int pri; } q_elem_t;
typedef struct { q_elem_t *buf; int n, alloc; } pri_queue_t, *pri_queue;
 
#define priq_purge(q) (q)->n = 1
#define priq_size(q) ((q)->n - 1)
/* first element in array not used to simplify indices */
pri_queue priq_new(int size)
{
  if (size < 4) size = 4;
 
  pri_queue q = malloc(sizeof(pri_queue_t));
  q->buf = malloc(sizeof(q_elem_t) * size);
  q->alloc = size;
  q->n = 1;
 
  return q;
}
 
void priq_push(pri_queue q, void *data, int pri)
{
  q_elem_t *b;
  int n, m;
 
  if (q->n >= q->alloc) {
    q->alloc *= 2;
    b = q->buf = realloc(q->buf, sizeof(q_elem_t) * q->alloc);
  } else
    b = q->buf;
 
  n = q->n++;
  /* append at end, then up heap */
  while ((m = n / 2) && pri < b[m].pri) {
    b[n] = b[m];
    n = m;
  }
  b[n].data = data;
  b[n].pri = pri;
}
 
/* remove top item. returns 0 if empty. *pri can be null. */
void * priq_pop(pri_queue q, int *pri)
{
  void *out;
  if (q->n == 1) return 0;
 
  q_elem_t *b = q->buf;
 
  out = b[1].data;
  if (pri) *pri = b[1].pri;
 
  /* pull last item to top, then down heap. */
  --q->n;
 
  int n = 1, m;
  while ((m = n * 2) < q->n) {
    if (m + 1 < q->n && b[m].pri > b[m + 1].pri) m++;
 
    if (b[q->n].pri <= b[m].pri) break;
    b[n] = b[m];
    n = m;
  }
 
  b[n] = b[q->n];
  if (q->n < q->alloc / 2 && q->n >= 16)
    q->buf = realloc(q->buf, (q->alloc /= 2) * sizeof(b[0]));
 
  return out;
}
 
/* get the top element without removing it from queue */
void* priq_top(pri_queue q, int *pri)
{
  if (q->n == 1) return 0;
  if (pri) *pri = q->buf[1].pri;
  return q->buf[1].data;
}
 
/* this is O(n log n), but probably not the best */
void priq_combine(pri_queue q, pri_queue q2)
{
  int i;
  q_elem_t *e = q2->buf + 1;
 
  for (i = q2->n - 1; i >= 1; i--, e++)
    priq_push(q, e->data, e->pri);
  priq_purge(q2);
}



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
	if (subset_size > 0) {
		/* find middle index of subset to split into more subsets */
		int middle = start + subset_size/ 2; 
		delete_edges(adj_matrix, nodes, start, middle, num_nodes); // setA
		delete_edges(adj_matrix, nodes, middle+1, end, num_nodes); //setB
		//printf("back to %d\n", );
		int setA = 0;
		int setB = 0;
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
				if (setB >= setA && setB != 0 && adj_matrix[indexA * num_nodes + indexB] == 1) {
					adj_matrix[indexA * num_nodes + indexB] = 0;
				} else if (setA > setB && adj_matrix[indexB * num_nodes + indexA] == 1) {
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


int* findSolution(int* adjMatrix, int num_nodes) {
	/*Overall Strategy: Keep a PQ containing 12 permutations with their rank. Pop out 
	the worst 6 and replace them with variations of the best 6. Then we solve the 6 and
	repush them into PQ, repeating the process of dropping the worst ones.*/
	pri_queue pq = priq_new(12);
	//Shuffle the nodes and pass them into the solver 12 times. Insert result into PQ

	int rank;
	int* nodes;
	int* copyMatrix;
	for (int twelve = 0; twelve < 12; ++twelve)
	{
		//Set up initial array of nodes
		nodes = malloc(sizeof(int) * num_nodes);
		for (int i = 0; i < num_nodes; ++i)
		{
			nodes[i] = i;
		}

		//Set up a copy of the adjacency matrix
		copyMatrix = malloc(sizeof(int) * num_nodes * num_nodes);
		for (int i = 0; i < num_nodes; ++i)
		{
			for (int j = 0; j < num_nodes; j++)
			{
				copyMatrix[i*num_nodes + j] = adjMatrix[i*num_nodes + j]; 
			}
		}


		shuffle(nodes, num_nodes);

		//Solver will modify 'nodes' and return the number of forward edges
		rank = solver(copyMatrix, nodes, num_nodes);
		priq_push(pq, nodes, rank);

		//Free the matrix
		free(copyMatrix);
	}
	
	/*For 5 minutes, pop off 6 worst and free them. Then pop off the 6 best. Create a new PQ,
	and for each good permutation, push it into the new PQ, solve its variation, and then 
	push that variation into the PQ. */

	pri_queue tempPQ;
	time_t start = time(NULL);
	int v = 0;
	int* badResults;
	while (v < 500) {
		v+=1;
		//Pop off the bad arrays and free them
		for (int i = 0; i < 6; ++i)
		{
			badResults = priq_pop(pq, NULL);
			free(badResults);
		}

		tempPQ = priq_new(12);
		int* goodResult;
		while (goodResult = priq_pop(pq, &rank)){
			priq_push(tempPQ, goodResult, rank);

			//Create a variation of the goodResult
			nodes = malloc(sizeof(int) * num_nodes);
			for (int i = 0; i < num_nodes; ++i)
			{
				nodes[i] = goodResult[i];
			}
			int randomIndex = rand() % num_nodes;
			int randomIndex2 = rand() % num_nodes;
			int temp = nodes[randomIndex];
			nodes[randomIndex] = nodes[randomIndex2];
			nodes[randomIndex2] = temp;

			//Set up a copy of the adjacency matrix
			copyMatrix = malloc(sizeof(int) * num_nodes * num_nodes);
			for (int i = 0; i < num_nodes; ++i)
			{
				for (int j = 0; j < num_nodes; j++)
				{
					copyMatrix[i*num_nodes + j] = adjMatrix[i*num_nodes + j]; 
				}
			}
			
			rank = solver(copyMatrix, nodes, num_nodes);
			priq_push(tempPQ, nodes, rank);
			free(copyMatrix);
   		}

   		pq = tempPQ;
	}

	//Return the best solution as the result
	for (int i = 0; i < 11; ++i)
	{
		badResults = priq_pop(pq, NULL);
		// for (int i = 0; i < num_nodes; ++i)
		// 	{
		// 		printf("%d ", badResults[i]);
		// 	}
		// 	printf("\n\n");
		free(badResults);
	}

	int ranking;
	int* best_solution = priq_pop(pq, &ranking);
	printf("%d\n", ranking);
	return best_solution;
}

int main(int argc, const char* argv[])
{
	//Formats the code to read the input, solve, then output
	//Create the output file
	FILE* testOut = fopen("test.out", "w");

	//LOOP through all in files
	// #pragma omp for
	for (int i = 1; i < 5; ++i)
	{
		//Read the file
		char inFile[15];
		sprintf(inFile, "aMASing%d.in", i);
		FILE* testIn = fopen(inFile, "r");
		char line[500];

		//THe first read should be the number of nodes
		fgets(line, sizeof(line), testIn);
		int num_nodes = atoi(line);

		//Malloc space for the matrix
		int* adjMatrix = malloc(sizeof(int) * num_nodes * num_nodes);

		//Iterate through the file line by line, loading numbers into the matrix
		for (int i = 0; i < num_nodes; ++i)
		{
			fgets(line, sizeof(line), testIn);
			for (int j = 0; j < 2*num_nodes; j+=2) //+= 2 because we skip spaces
			{
				int edge = line[j] - '0';
				adjMatrix[i*num_nodes + j/2] = edge; //j/2 to account for the skipping
			}
		}
		fclose(testIn);

		int* optSol = findSolution(adjMatrix, num_nodes);

		//Write the node ordering returned from the solution
		for (int i = 0; i < num_nodes; ++i)
		{
			fprintf(testOut, "%d ", optSol[i] + 1);
		}
		//Place a newlinew
		fprintf(testOut,"\n");

		//Free the Matrix and the optimal solution
		free(adjMatrix);
	}

	fclose(testOut);
}
