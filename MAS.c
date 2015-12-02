#include <stdio.h>
#include <time.h>
#include <stdlib.h>


// Fisher-Yates shuffle taken from http://www.dispersiondesign.com/articles/algorithms/shuffle_array_order
void shuffle(int * a, int n)
{
      int i = n - 1;
      int j, temp;                

      srand(time(NULL));

      while (i > 0)
      {
            j = rand() % (i + 1);
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



int main(int argc, const char* argv[])
{
	//Formats the code to read the input, solve, then output
	//Create the output file
	FILE* testOut = fopen("test.out", "w");

	//LOOP through all in files
	// #pragma omp for
	for (int i = 1; i < 2; ++i)
	{
		//Read the file
		char inFile[15];
		sprintf(inFile, "test%d.in", i);
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

		// int* optSol = findSolution(adjMatrix, num_nodes);
		int optSol[4] = {1,2,3,4};

		//Write the node ordering returned from the solution
		for (int i = 0; i < num_nodes; ++i)
		{
			fprintf(testOut, "%d ", optSol[i]);
		}
		//Place a newline
		fprintf(testOut,"\n");

		//Free the Matrix and the optimal solution
		free(adjMatrix);
	}

	fclose(testOut);

	// //See if PQ works properly on arrays
	// printf("starting test\n");
	// pri_queue pq = priq_new(12);

	// int* nodes = malloc(sizeof(int) * 5);
	// for (int i = 0; i < 5; ++i)
	// {
	// 	nodes[i] = 2*i;
	// }
	// int* nodes2 = malloc(sizeof(int) * 5);
	// for (int i = 0; i < 5; ++i)
	// {
	// 	nodes2[i] = i;
	// }

	// priq_push(pq, nodes, 100);
	// priq_push(pq, nodes2, 20);

	// int* c;
	// int p;
	// while (c = priq_pop(pq, &p)){
 //    	printf("%d is the priority Value\n", p, c);
 //    	for (int i = 0; i < 5; ++i)
 //    	{
 //    		printf("%d", c[i]);
 //    	}
 //    	printf("\n\n");
 //    }
 //    free(nodes);
 //    free(nodes2);
}

int findSolution(int* adjMatrix, int num_nodes) {
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
	while ((time(NULL) - start)/60 < 5) {

		//Pop off the bad arrays and free them
		for (int i = 0; i < 6; ++i)
		{
			int* badResults = priq_pop(pq, NULL);
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
			nodes[randomIndex] = nodes[randomIndex2];
			nodes[randomIndex2] = nodes[randomIndex];

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
		free(priq_pop(pq, NULL));
	}
	int* best_solution = priq_pop(pq, NULL);
	return best_solution;
}