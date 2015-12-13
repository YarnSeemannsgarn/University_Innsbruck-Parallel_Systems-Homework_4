#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <mpi.h>

#include "util.h"

// Counting sort with given count
int *counting_sort(int *list, int *count, size_t N, int keys){
  int total = 0;
  int old_count;
  int i;
  for(i=0; i<keys; ++i){
    old_count = count[i];
    count[i] = total;
    total += old_count;
  }

  int *sortedList = malloc(N*sizeof(int));
  for(i=0; i<N; ++i){
    sortedList[count[list[i]%keys]] = list[i];
    count[list[i]%keys] += 1;
  }

  return sortedList;
}

// Part of counting sort
int *count_elements(int *list, size_t N, int keys){
  int *count = calloc(keys, sizeof(int));
  
  int i;
  for(i=0; i<N; ++i){
    count[list[i]%keys] += 1;
  }

  return count;
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
  int send_count = N / comm_size;

  // Create and initialise list with root
  double begin, end;
  int *list;
  int *result_count;
  int *result_list;
  if(rank == 0) {
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

    // malloc result count
    result_count = malloc(RANGE_MAX * sizeof(int));
    
    // Print list before sorting
    if(argc == 3){
      printf("List before sorting:\n");
      print_list(list, N);
    }

    printf("Starting counting sort for problem size %zu and %i processes\n", N, comm_size);
    begin = MPI_Wtime();
  }

  // Scatter list
  int *scatter_receive = malloc(send_count * sizeof(int));
  MPI_Scatter(list, send_count, MPI_INT, scatter_receive, send_count, MPI_INT, 0, MPI_COMM_WORLD);

  // Count parts in parallel
  int *count_part = count_elements(scatter_receive, send_count, RANGE_MAX);

  // Reduce
  MPI_Reduce(count_part, result_count, RANGE_MAX, MPI_INT, MPI_SUM, 0, MPI_COMM_WORLD);
  if(rank == 0) {
    result_list = counting_sort(list, result_count, N, RANGE_MAX);

    // Measure time result
    end = MPI_Wtime();
    printf("Time spent: %fs\n", end-begin);

    // Print list after sorting
    if(argc == 3){
      printf("\nList after sorting:\n");
      print_list(result_list, N);
    }

    // Free memory
    free(list);
    free(result_count);
    free(result_list);
  }

  // Free memory
  free(scatter_receive);
  free(count_part);

  MPI_Finalize();
  return EXIT_SUCCESS;
}
