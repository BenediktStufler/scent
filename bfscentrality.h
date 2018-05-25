/*
 *	We calculate the closeness centrality of the vertices of a graph 
 *  using a simple breadth-first-search based algorithm
 *
 *  The workload is distributed on multiple threads
 *
 */




struct stat {
	int status;		// determines if we already queued the vertex in the past
	int dist;		// the distance of the vertex in the current BFS
};

// data that gets passed to a thread
struct gsegment {
	struct graph *G;
	int start;
	int end;
	int success;
};


// calculate closeness centrality of vertices with ids start, start+1, ..., end-1
void *centrality(void *seg) {
	struct graph *G = ((struct gsegment *)seg)->G;
	int start = ((struct gsegment *)seg)->start;
	int end = ((struct gsegment *)seg)->end;

	struct stat *arr;		// array that holds status information
	struct vertex **queue;	// we realize the queue as a static array to
							// avoid repeated memory allocation & deallocation
	struct vertex *v;		
	struct list *li;
	int i,j,k;
	int num = G->num;		// number of vertices in our graph
	int dist;

	int pop;			// index for queue

	


	//check for sanity of arguments
	if(start < 0 || end > num) {
		fprintf(stderr, "Argument out of range error in function centrality\n");
		return (void *) -1;
	}
	if(end <= start) {
		return (void *) 0;
	}
	
	// build helper arrays
	arr = calloc(sizeof(struct stat), num);
	queue = calloc(sizeof(struct vertex *), num);
	if(arr == NULL  || queue == NULL) {
		// memory allocation error
		fprintf(stderr, "Memory allocation error in function centrality\n");
		return (void *) -1;
	}

	// initialize helper arrays
	for(i=0; i<num; i++) {
		arr[i].status = -1;
	}



	// calculate closeness centrality
	// this is the part that needs to be as fast as possible
	for(i=start; i<end; i++) {

		dist=0;		// this variable will denote the sum of all distances
		queue[0] = G->arr[i];		// queue the starting vertex		
		arr[i].status = i;			// mark the starting vertex as queued
		arr[i].dist = 0;			// the starting vertex has distance zero
									// from itself

		// need j!= pop check for disconnected grpahs
		for(j=0, pop=1; j<num && j!=pop; j++) {		// iterate over the entire queue
			dist += arr[queue[j]->id].dist;	// add contribution of vertex to dist

			// queue all neighbours of our vertex that have not been queued yet
			li = queue[j]->li;
			while(li->ve != NULL) {
				if( arr[li->ve->id].status != i) {	// vertex yet unqueued
					queue[pop] = li->ve;			// queue vertex
					pop++;	// increment index for queue location

					arr[li->ve->id].status = i;	// mark vertex as queued
					arr[li->ve->id].dist = arr[queue[j]->id].dist + 1; //dist
				}
				li = li->ne;
			}
		}
			// save distance sum of vertex
			G->arr[i]->cent = dist;
	}

	// free helper arrays
	free(arr);
	free(queue);

	return (void *) 0;
}

int launchThreads(struct graph *G, int start, int end, int numThreads) {
	int chunkSize;				// roughly how many vertices each thread
								// has to take care of
	struct gsegment *segList;	// arguments for the separate threads
	pthread_t *th;				// array of threads
	int i;
	void *ret;
	
	/* divide the workload */
	chunkSize = (end - start) / numThreads;
	if(chunkSize*numThreads < end - start) chunkSize++;

	segList = calloc(sizeof(struct gsegment), numThreads);
	for(i=0; i<numThreads-1; i++) {
		segList[i].start = start + i * chunkSize;	
		segList[i].end = start + (i+1) * chunkSize;	
		segList[i].G = G;
	}
	segList[numThreads -1].start = chunkSize * (numThreads - 1);
	segList[numThreads -1].end = end;
	segList[numThreads -1].G = G;


	/* launch threads */	
	th = calloc(sizeof(pthread_t), numThreads);
	for(i=0; i<numThreads; i++) {
		if(pthread_create(&th[i], NULL, &centrality, &segList[i] )) {
			fprintf(stderr, "Error launching thread number %d\n", i);
			return -1;
		}
	}


	/* wait for threads to finish */
	for(i=0; i<numThreads; i++) {
		pthread_join(th[i], &ret);
		if(ret) {
			fprintf(stderr, "Error executing thread number %d\n", i);
			return -1;
		}	
	}

	/* clean up */
	free(segList);
	free(th);	

	return 0;
}
