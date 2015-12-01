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





int main( int argc, const char* argv[])
{
	//Formats the code to read the input, solve, then output

	//Create the output file
	FILE* testOut = fopen('test.out', "w");


	//LOOP through all in files
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

		//Magic Happens Here
		// findSolution(adjMatrix, num_nodes);

		//Output the node ordered returning from the solution
		for (int i = 0; i < num_nodes; ++i)
		{
			for (int j = 0; j < num_nodes; ++j)
			{
				printf("%d\n", adjMatrix[i*num_nodes + j]);
				printf("hello\n");
				fprintf(testOut, "%d", adjMatrix[i*num_nodes + j]);
			}
		}

		//Free the Matrix
		free(adjMatrix);

		//Place a newline
		fprintf(testOut,"\n");
		
	}

	fclose(testOut);

	//See if PQ works properly on arrays
	printf("starting test\n");
	pri_queue pq = priq_new(12);

	int* nodes = malloc(sizeof(int) * 5);
	for (int i = 0; i < 5; ++i)
	{
		nodes[i] = 2*i;
	}
	int* nodes2 = malloc(sizeof(int) * 5);
	for (int i = 0; i < 5; ++i)
	{
		nodes2[i] = i;
	}

	priq_push(pq, nodes, 10);
	priq_push(pq, nodes2, 20);

	int* c;
	int p;
	while (c = priq_pop(pq, &p)){
    	printf("%d is the priority Value\n", p, c);
    	for (int i = 0; i < 5; ++i)
    	{
    		printf("%d", c[i]);
    	}
    	printf("\n\n");
    }
    free(nodes);
    free(nodes2);
}

// int findSolution(int* adjMatrix, int num_nodes) {
// 	adjMatrix[0] = 4;
// 	/*Overall Strategy: Keep a PQ containing 12 permutations with their rank. Pop out 
// 	the worst 6 and replace them with variations of the best 6. Then we solve the 6 and
// 	repush them into PQ, repeating the process of dropping the worst ones.*/

// 	//Set up initial array of nodes
// 	int* nodes = malloc(sizeof(int) * num_nodes);
// 	for (int i = 0; i < num_nodes; ++i)
// 	{
// 		nodes[i] = i;
// 	}
// 	//Set up a copy of the adjacency matrix


// 	pri_queue pq = priq_new(12);
// 	//Shuffle the nodes and pass them into the solver 12 times. Insert result into PQ
// 	for (int i = 0; i < 12; ++i)
// 	{
// 		shuffle(nodes, num_nodes);
// 		int* result = solver(adjMatrix, nodes);
// 		//Input the nodes into an array, which we insert into the pq

// 	}
	

// 	/*For 5 minutes, pop off 6 worst and free them. Then pop off the 6 best. Create a new PQ,
// 	and for each good permutation, push it into the new PQ, solve its variation, and then 
// 	push that variation into the PQ. */
// 	free(nodes);

// }