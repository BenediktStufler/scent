# scent
A simple approach to calculating the closeness centrality of vertices in a graph


1. Introduction

The closeness centrality of a vertex in a graph is the inverse of the average graph distance from this vertex to all other vertices.

We want to calculate the closeness centrality of each vertex in a graph with a couple of million vertices. Implementations of  algorithms that significantly reduce the number of calculations exist, but require too much memory. The main reason is that the entire distance matrix is calculated. Hence memory usage is quadratic in the number of vertices of the graph.

For this reason we use a simple breadth-first-search based approach instead. This requires more calculations, but memory usage is only linear in the number of vertices.

We do our best to limit the impact of the speed loss, by distributing the workload on multiple threads.


2. Usage

            scent [-?V] [-e END] [-i INFILE] [-o OUTFILE] [-s START] [-t THREADS]
            [--end=END] [--infile=INFILE] [--outfile=OUTFILE] [--start=START]            
            [--threads=THREADS] [--help] [--usage] [--version]

  -e, --end=END                     
  Restrict to vertices with index less than END

  -i, --infile=INFILE               
  Read graphml input from INFILE instead of standard input
  -o, --outfile=OUTFILE             
  Output to OUTFILE instead of to standard output

  -s, --start=START                 
  Restrict to vertices with index at least START

  -t, --threads=THREADS             
  Distribute the workload on THREADS many threads

  -?, --help                        
  Give this help list

  --usage                           
  Give a short usage message

  -V, --version                     
  Print program version
  
  
3. Example

scent -i example/test.graphml -t 4
