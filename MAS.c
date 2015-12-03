#include <stdio.h>
#include <time.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

int bestSolutionsRank = 0;

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
	/*Overall Strategy: Keep 2 PQs containing 12 permutations with their rank. Pop out 
	the worst 7 and replace them with variations of the best 5 and add 2 random. 
	Then we solve the 6 and repush them into PQ, repeating the process of dropping 
	the worst ones.*/
	pri_queue pq1 = priq_new(12);
	pri_queue pq2 = priq_new(12);
	//Shuffle the nodes and pass them into the solver 12 times. Insert result into PQ

	int rank;
	int* nodes;
	int* copyMatrix;
	for (int twentyFour = 0; twentyFour < 24; ++twentyFour)
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

		//Split pushing into 2 groups
		if (twentyFour < 12)
		{
			priq_push(pq1, nodes, rank);
		} else {
			priq_push(pq2, nodes, rank);
		}
		

		//Free the matrix
		free(copyMatrix);
	}
	
	/*For 5 minutes, pop off 7 worst and free them. Then pop off the 5 best. Create a new PQ,
	and for each good permutation, push it into the new PQ, solve its variation, and then 
	push that variation into the PQ. After, create 2 new random permutations, solve them
	and add them into pq.*/

	pri_queue tempPQ1;
	pri_queue tempPQ2;
	time_t start = time(NULL);
	int* badResults;
	int v = 0;
	while (v < 45000) {
		v+=1;
		//Pop off the bad arrays and free them
		for (int i = 0; i < 7; ++i)
		{
			//Free PQ1
			badResults = priq_pop(pq1, NULL);
			free(badResults);

			//Free PQ2
			badResults = priq_pop(pq2, NULL);
			free(badResults);
		}

		tempPQ1 = priq_new(12);
		tempPQ2 = priq_new(12);
		int* goodResult;
		//Create the 6 variations of the good ones, then solve them and push into pq
		for (int newInserts = 0; newInserts < 14; ++newInserts)
		{
			if (newInserts < 5)
			{
				goodResult = priq_pop(pq1, &rank);
				priq_push(tempPQ1, goodResult, rank);
			} else if (newInserts < 10)
			{
				goodResult = priq_pop(pq2, &rank);
				priq_push(tempPQ2, goodResult, rank);
			}				

			//Create a variation of the goodResult or create a permutation if twelve > 10
			nodes = malloc(sizeof(int) * num_nodes);
			for (int i = 0; i < num_nodes; ++i)
			{
				if (newInserts < 10)
				{
					nodes[i] = goodResult[i];
				} else {
					nodes[i] = i;
				}
			}

			//if generating random permutations, shuffle the nodes
			if (newInserts >= 10)
			{
				shuffle(nodes, num_nodes);
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
			if (newInserts < 5)
			{
				priq_push(tempPQ1, nodes, rank);
			} else if (newInserts < 10)
			{
				priq_push(tempPQ2, nodes, rank);
			} else if (newInserts < 12){
				priq_push(tempPQ1 ,nodes, rank);
			} else {
				priq_push(tempPQ2, nodes, rank);
			}
			
			free(copyMatrix);
   		}

   		pq1 = tempPQ1;
   		pq2 = tempPQ2;
	}

	//Return the best solution as the result
	for (int i = 0; i < 11; ++i)
	{
		badResults = priq_pop(pq1, NULL);
		free(badResults);

		badResults = priq_pop(pq2, NULL);
		free(badResults);
	}

	//Push the last element of pq2 into pq1. Then, pop off the element from pq1 and free it
	//The last remaining element is the optimal solution
	int* otherBestElement = priq_pop(pq2, &bestSolutionsRank);
	priq_push(pq1, otherBestElement, bestSolutionsRank);

	//free worst solution
	otherBestElement = priq_pop(pq1, NULL);
	free(otherBestElement);

	int* best_solution = priq_pop(pq1, &bestSolutionsRank);
	return best_solution;
}

/**
 * Check if a file exists
 * @return true if and only if the file exists, false else
 */
int fileExists(const char* file) {
    struct stat buf;
    if (stat(file, &buf) == 0) {
    	return 1;
    } else {
    	return 0;
    }
}

int main(int argc, const char* argv[])
{
	//Formats the code to read the input, solve, then output
	while (1) {
		//LOOP through all in files
		// #pragma omp for
		for (int i = 1; i < 622; ++i)
		{
			//Read the file
			char inFile[15];
			sprintf(inFile, "instances/%d.in", i);
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
			/* check to see if you did better than the last one */
			/* check if there exists an out file, if not create the output file*/
			char outputFileName[15];
			sprintf(outputFileName, "outputs/%d.out", i);
			// FILE* oldOutFile = fopen(outputFileName, "r");
			//Create the output file
			if (access(outputFileName, F_OK ) == -1) {
				/*  there was no existing out file for this instance 
					so we close the file we tried opening, open a new 
					file and write the rank and node ordering to that */
				// fclose(oldOutFile);
				FILE* testOut = fopen(outputFileName, "w");
				fprintf(testOut, "%d\n", bestSolutionsRank);
				for (int i = 0; i < num_nodes; ++i)
				{
					fprintf(testOut, "%d ", optSol[i] + 1);
				}
				//close the output file
				fclose(testOut);
				
			} else {
				/* old out file exists, get the rank of the old file */
				FILE* oldOutFile = fopen(outputFileName, "r");
				char line2[500];
				fgets(line2, sizeof(line2), oldOutFile);
				int oldBestRank = atoi(line2);
				fclose(oldOutFile);
				if (oldBestRank < bestSolutionsRank) {
					/* replace with new best solution */
					FILE * newOut = fopen(outputFileName, "w");
					fprintf(newOut, "%d\n", bestSolutionsRank);
					for (int i = 0; i < num_nodes; ++i)
					{
						fprintf(newOut, "%d ", optSol[i] + 1);
					}
					//close the output file
					fclose(newOut);
				}
				/* else do nothing because old solution out file is better */
			}

			//Compute the number of forward edges from this result and then
			//write out that result followed by the node ordering


			//Free the Matrix and the optimal solution
			free(adjMatrix);
		}
	}

}
