#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <mpi.h>

#include "util.h"

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
