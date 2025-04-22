/*
 *
 * We define a data structure that holds our graph
 * We want to partition the vertex list and assign each partition class
 * to a separate thread. The thread then calculates the closeness centrality
 * of all vertices in the segment.
 *
 */ 

struct list {
	struct vertex *ve;		// the neighbour vertex
							// equal to NULL if we are at the end of the list
	struct list *ne;		// the next neighbour vertex in the list
};
	
struct vertex {
	struct list *li;		// list of all neighbouring vertices
	INTN id;				// unique id that will be used by the BFS search
							// the vertices in the graph are required to have
							// ids 0,1,2,...	
	INTD cent;				// sum of distances from this vertex to all others
};

struct graph {
	INTN num;				// the number of vertices
	struct vertex **arr;	// array with pointers to all vertices
};


// output closeness centrality of a segment of vertices
int outcent(struct graph *G, INTN start, INTN end, char *outfile) {
	INTN i;
	double num = (double) (G->num-1);
	FILE *outstream;

	//check for sanity of arguments
	if(start < 0 || end > G->num) {
		fprintf(stderr, "Argument out of range error in function outcent\n");
		return -1;
	}
	if(end <= start) {
		return 0;
	}

	// open output file if necessary
	if(outfile == NULL) {
		outstream = stdout;
	} else {
		outstream = fopen(outfile, "w");
		if(outstream == NULL) {
			fprintf(stderr, "Error opening output file.\n");
			exit(-1);
		}
	}

	// output closeness centrality of vertices
//	fprintf(outstream, "{");
	for(i=start; i<end-1; i++) {
		fprintf(outstream, "%17.17f, ", num / (double) G->arr[i]->cent);	
	}
	fprintf(outstream, "%17.17f", num / (double) G->arr[end-1]->cent);	
	//fprintf(outstream, "}\n");


	// close file if necessary
	if(outfile != NULL) fclose(outstream);	

	return 0;
}

void print_graph(struct graph *G) {
	INTN i;
	struct list *li;

	printf("Number of vertices: %d\n", G->num);
	for(i=0; i<G->num; i++) {
		printf("Neighbours of vertex %d:\n", i);
		for(li = G->arr[i]->li; li->ve != NULL; li = li->ne) {
			printf("\t %d\n", li->ve->id);
		}
	}
}

// adds a directed edge from v to w
// if the edge is already present a second (multi-)edge will be added
int addDiEdge(struct vertex *v, struct vertex *w) {
	struct list *iter;

	// let iter point to the last element of the list
	iter = v->li;
	while(iter->ve != NULL)
		iter = iter->ne;	

	// add new endpoint of list
	iter->ne = (struct list *) malloc(sizeof(struct list));
	if(iter->ne == NULL) {
		// memory allocation error
		fprintf(stderr, "Memory allocation error in function addDiEdge\n");
		return -1;
	}
	iter->ne->ve = NULL;
	iter->ne->ne = NULL;

	// add w as a neighbour
	iter->ve = w;

	return 0;
}

// adds a directed edge from v to w and from w to v
// if any edge is already present, then a second (multi-)edge will be added
int addEdge(struct vertex *v, struct vertex *w) {
	if(addDiEdge(v,w)) return -1;
	if(addDiEdge(w,v)) return -1;

	return 0;
}

/* function for building the following example graph
 *
 *	   /-- 2 -- 5
 *	  /	|
 * 0 -- 1 -- 3
 * 		\	|
 *	   \-- 4
 *
 */
struct graph *iniGraphExample() {
	struct graph *G;
	struct vertex **v;
	int i;

	// initialize graph
	G = (struct graph *) malloc(sizeof(struct graph));
	v = (struct vertex **) calloc(sizeof(struct vertex *), 6);

	if(G == NULL || v==NULL) {
		fprintf(stderr, "Memory allocation error in function iniGraphExample\n");	
		return NULL;
	}

	G->arr = v;
	G->num = 6;


	// initialize vertices
	for(i=0; i<6; i++) {
		v[i] = (struct vertex *) malloc(sizeof(struct vertex));	
		if(v[i] == NULL) {
			fprintf(stderr, "Memory allocation error in function iniGraphExample\n");	
			return NULL;
		}
		v[i]->id = i;
		v[i]->li = (struct list *) malloc(sizeof(struct list));	
		if(v[i]->li == NULL) {
			fprintf(stderr, "Memory allocation error in function iniGraphExample\n");	
			return NULL;
		}
		v[i]->li->ve = NULL;
		v[i]->li->ne = NULL;
	}


	// add edges
	if(addEdge(v[0],v[1])) return NULL;	

	if(addEdge(v[1],v[2])) return NULL;
	if(addEdge(v[1],v[3])) return NULL;
	if(addEdge(v[1],v[4])) return NULL;

	if(addEdge(v[2],v[3])) return NULL;
	if(addEdge(v[3],v[4])) return NULL;

	if(addEdge(v[2],v[5])) return NULL;

	return G;				// return the address of the graph we created
}



