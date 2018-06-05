/*
 * scent - a simple approach for calculating the closeness centrality
 *         of vertices in a graph
 *
 * The closeness centrality C(v) of a vertex v in a graph G is the inverse of 
 * the average graph distance from this vertex to all other vertices.
 *
 * We want to calculate this for each vertex of a graph with a couple of million
 * vertices. Time efficient algorithms exist but require too much memory.
 * The main reason is that the entire distance matrix is calculated. Hence
 * memory usage is quadratic in the number of vertices of the graph.
 *
 * For this reason we use a simple breadth-first-search based approach instead.
 * This requires more calculations, about O(#vertices*(#vertices + #edges)).
 * But memory usage is only linear in the number of vertices.
 *
 * We do our best to limit the impact of the speed loss by distributing the
 * workload on multiple threads.
    Copyright (C) 2018 Benedikt Stufler

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */




#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <argp.h>
#include <inttypes.h>
#include <expat.h>



/*
 * define the data types for large integers
 * keep in mind that the data type int may occupy only 32bit on 64bit machines
 *
 * INTN: the data type that needs to be able to store the NUMBER of vertices
 * INTD: the data type that needs to be able to store the sum of distances
 *       from a single vertex to the rest
 */
#define INTN unsigned int
#define INTD unsigned long long

/*
 * provides a data structure that holds a graph
*/
#include "graphstructure.h"

/*
 * provides a bfs base algorithm that calculates the closeness centrality
 * of a list of vertices in a graph
 */
#include "bfscentrality.h"

/*
 * parses command line options using the argp library
 */
#include "cmdparse.h"

/*
 * parses a graphml file using the expat XML parser library
 */
#include "graphmlparse.h"


int main(int argc, char *argv[]) {
	
	struct graph *G;		// holds our graph
	struct cmdarg comarg;	// holds our command line options


	/* read command line options */
	if(getcmdargs(&comarg, argc, argv)) {
		fprintf(stderr, "Error reading command line options\n");
		return -1;
	}

	/* parse graphml input */
	G = parsegraphml(comarg.infile);


	/* sanity checks */
	// if no start vertex was specified start with the first vertex of the list
	if(comarg.start == -1) comarg.start = 0;
	// if no end vertex was specified we stop at the end of the list
	if(comarg.end == -1) comarg.end =  G->num;
	// if there is nothing to do we should stop right now
	if(comarg.end <= comarg.start) return 0;
	// more than one thread per vertex makes no sense and causes problems
	if(comarg.threads > comarg.end - comarg.start)
		comarg.threads = comarg.end - comarg.start;
	// gotta have at least one thread
	if(comarg.threads <= 0) comarg.threads = 1;


	/* launch threads */
	launchThreads(G, comarg.start, comarg.end, comarg.threads);


	/* output closeness centrality of vertices start, start +1, ... , end-1 */
	if(outcent(G, comarg.start, comarg.end, comarg.outfile)) {
		fprintf(stderr, "Error calculating closeness centrality\n");
		return -1;
	}

	return 0;
}


