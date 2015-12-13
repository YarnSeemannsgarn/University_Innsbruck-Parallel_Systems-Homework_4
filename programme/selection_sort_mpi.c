#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <mpi.h>

#include "util.h"

struct {
  float min;
  int index;
} in, out;

// https://en.wikipedia.org/wiki/Selection_sort#Implementation
void selection_sort(int *list, size_t N){
  int i, j, iMin;
  for(j=0; j<N-1; ++j){
    iMin = j;
    for(i=j+1; i<N; ++i){
      if(list[i] < list[iMin]){
	iMin = i;
      }
    }

    if(iMin != j){
      swap(&list[j], &list[iMin]);
    }
  }
}

int main(int argc, char *argv[]){
  // MPI Init, rank & size
  int rank, comm_size;
  MPI_Init(&argc, &argv);
  MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  MPI_Comm_size(MPI_COMM_WORLD, &comm_size);

  // Handle parameter
  if(argc != 2 && argc != 3){
    fprintf(stderr, USAGE_MESSAGE, argv[0]);
    return EXIT_FAILURE;
  }
  size_t N = atoi(argv[1]);

  // Create and initialise list with root
  double begin, end;
  int *list;
  if (rank == 0) {
    list = (int *) malloc(N * sizeof(int));
    if(list == NULL){
      fprintf(stderr, MEMORY_MESSAGE);
      return EXIT_FAILURE;
    }
    int i;
    srand48(time(NULL));
    for(i=0; i<N; ++i){
      list[i] = lrand48() % RANGE_MAX;
      }
    
    // Print list before sorting
    if(argc == 3){
      printf("List before sorting:\n");
      print_list(list, N);
    }

      // Measure time
    printf("Starting counting sort for problem size %zu and %i processes\n", N, comm_size);
    begin = MPI_Wtime();
  }

  int *list_itereator;
  int send_counts[comm_size];
  int displs[comm_size];
  displs[0] = 0;
  int i;
  for (i = 0; i < N; i++) {
    if (rank == 0) {
      list_itereator = &list[i];
    }
    
    // Calculate send_counts and displs in each process (no broadcast overhead)
    int j;
    for (j = 0; j < comm_size; j++) {
      send_counts[j] = ((N-i) / comm_size);
      if ((j+1) <= ((N-i) % comm_size)) {
	send_counts[j]++;
      }
      if (j < (comm_size-1)) {
	displs[j+1] = displs[j] + send_counts[j];
      }
    }

    int recv_count = send_counts[rank];
    int list_part[recv_count];
    MPI_Scatterv(list_itereator, send_counts, displs, MPI_INT, list_part, recv_count, MPI_INT, 0, MPI_COMM_WORLD);

    // Find minimum and right index in each process
    int iMin;
    int index = i;
    if (recv_count > 0) {
      iMin = list_part[0];
      for (j = 1; j < recv_count; j++) {
	if (list_part[j] < iMin) {
	  iMin = list_part[j];
	  index = i + j;
	}
      }
      
      for (j = 0; j < rank; j++) {
	index += send_counts[j];
      }
    } else {
      iMin = RANGE_MAX + 1; // we need a dummy value, which cannot be the minimum
    }


    // Reduce and get global minima with index
    in.min = (float) iMin;
    in.index = index;
    MPI_Reduce(&in, &out, 1, MPI_FLOAT_INT, MPI_MINLOC, 0, MPI_COMM_WORLD); 

    // Swap positions 
    if (rank == 0) {
      swap(&list[i], &list[out.index]);
    }
  }


  if (rank == 0) {
    // Measure time result
    end = MPI_Wtime();
    printf("Time spent: %fs\n", end-begin);

    // Print list after sorting
    if(argc == 3){
      printf("\nList after sorting:\n");
      print_list(list, N);
    }

    // Free memory
    free(list);
  }

  MPI_Finalize();
  return EXIT_SUCCESS;
}
