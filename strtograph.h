/*
 \*
 * We use a simple hashmap to build a graph structure out of a list of vertices
 * and edges that are given by strings and pairs of strings, respectively.
 *
 */

struct bucket {
	struct vlist *vstart;
	struct vlist *vend;
};

void push2bucket(struct bucket *B, struct vlist *vert) {
	struct vlist *v;

    // create copy of vertex
    v = (struct vlist *) malloc(sizeof(struct vlist));
    if(v == NULL) {
        fprintf(stderr, "Error allocating memory for hashmap");
        exit(-1);
    }
    v->next = NULL;
    v->ident = (char *) malloc(sizeof(char)*(strlen(vert->ident) + 1));
    if(v->ident == NULL) {
        fprintf(stderr, "Error allocating memory for hashmap.\n");
        exit(-1);
    }
    strcpy(v->ident, vert->ident);
	v->id = vert->id;


    // put vertex into bucket
    if(B->vstart == NULL) {
        B->vstart = v;
        B->vend = v;
    } else {
        B->vend->next = v;
        B->vend = v;
    }
}


unsigned long long djb2hash(unsigned char *str, INTN modulo) {
	unsigned long long hash = 5381;
	int c;

	while (c = *str++)
		hash = ((hash << 5) + hash) + c; // hash*33 + c

	return hash % modulo;
}

// this function returns id of the vertex with identifier string *str
// if such a vertex does not exist, it returns -1
int getid(char *str, struct bucket *bucketlist, INTN len) {
	struct vlist *v;
	INTN bucketnumber = djb2hash(str,len);

	if(bucketnumber >= len) {
		fprintf(stderr, "Error looking for a vertex in the hashmap.\n");	
		exit(-1);
	}

	for(v = bucketlist[bucketnumber].vstart; v != NULL; v = v->next)
		if( strcmp(v->ident, str) == 0) return v->id;

	return -1;		// vertex not found
}

void emptybuckets(struct bucket *bucketlist, INTN len) {
	INTN i;
	struct vlist *v;
	struct vlist *tmpv;

	for(i=0; i<len; i++) {
		v = bucketlist[i].vstart;
		while(v) {
			tmpv = v->next;
			free(v->ident);
			free(v);
			v = tmpv;
		}
	}
	free(bucketlist);
}

struct bucket *ini_buckets(struct strgraph *H) {
	INTN i;
	INTN len = H->vend->id + 1;
	struct vlist *v;
	struct bucket *bucketlist;

	// create and initialize list of buckets	
	bucketlist = (struct bucket *) calloc(sizeof(struct bucket), len);
	if(bucketlist == NULL) {
		fprintf(stderr, "Error allocating memory for hashmap.");
		exit(-1);
	}
	for(i=0; i<len; i++) {
		bucketlist[i].vstart = NULL;
		bucketlist[i].vend = NULL;
	}

	// push vertices into buckets
	for( v = H->vstart; v != NULL; v = v->next) {
		push2bucket(&bucketlist[djb2hash(v->ident, len)], v);
	}

	return bucketlist;
}

struct graph *ini_graph(struct strgraph *H, struct bucket *bucketlist) {
	struct vertex **varr;
	struct graph *G;
	struct elist *e;
\
	INTN i;
	INTN source;
	INTN target;
	INTN len = H->vend->id + 1;


    // initialize graph
    G = (struct graph *) malloc(sizeof(struct graph));
    varr = (struct vertex **) calloc(sizeof(struct vertex *), len);
    if(G == NULL || varr==NULL) {
        fprintf(stderr, "Memory allocation error trying to build the graph.\n");
        exit(-1);
    }
	G->arr = varr;
	G->num = len;

	// initialize vertices	
	for(i=0; i<len; i++) {
        varr[i] = (struct vertex *) malloc(sizeof(struct vertex));
        if(varr[i] == NULL) {
            fprintf(stderr, "Memory allocation error trying to build the graph.\n");
            exit(-1);
        }
        varr[i]->id = i;
        varr[i]->li = (struct list *) malloc(sizeof(struct list));
        if(varr[i]->li == NULL) {
            fprintf(stderr, "Memory allocation error trying to build the graph.\n");
            exit(-1);
        }
        varr[i]->li->ve = NULL;
        varr[i]->li->ne = NULL;
    }

	// add edges	
	for(e = H->estart; e!= NULL; e = e->next) {
		source = getid(e->source, bucketlist, len);
		target = getid(e->target, bucketlist, len);
		
		// we need to check if these vertices were actually found
		if( source != -1 && target != -1) {
			addDiEdge(varr[source], varr[target]);
		}
	}

	return G;
}


struct graph *make_graph(struct strgraph *H) {
	struct bucket *bucketlist;		// the hashmap
	struct graph *G;
	INTN len = H->vend->id + 1;


	// sanity check: exit if graph is empty
	if( H == NULL || H->vstart == NULL) {
		fprintf(stderr, "Error: received empty graph\n");
		exit(-1);
	}

	// initialize hashmap
	bucketlist = ini_buckets(H);

	// create graph structure
	G = ini_graph(H, bucketlist);

	// free allocated memory for hashmap
	emptybuckets(bucketlist, len);

	return G;
}
