#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
#include <string.h>

#include "util.h"

struct bucket_t{
  int *values;
  int counter;
};

// For quicksort
int cmpfunc (const void * a, const void * b)
{
   return ( *(int*)a - *(int*)b );
}

// Construckt buckets
// Algorithm from https://en.wikipedia.org/wiki/Bucket_sort#Pseudocode
struct bucket_t *construct_buckets(int *list, size_t N, int bucket_count){
  struct bucket_t *buckets = malloc(sizeof(struct bucket_t) * bucket_count);
  int i;
  for(i=0; i<bucket_count; ++i){
    buckets[i].values = malloc(N * sizeof(int)); // allocate initial list size memory for each bucket, 
                                              // because one bucket could contain all values... 
                                              // can be optimized with linked list, but than the sorting is
                                              // more complicated
    if(buckets[i].values == NULL){
      fprintf(stderr, MEMORY_MESSAGE);
      return NULL;
    }
    buckets[i].counter = 0;
  }
  
  // Scatter: Go over the original array, putting each object in its bucket.
  struct bucket_t *current_bucket;
  int bucket_size = (int) ceil(((double) RANGE_MAX) / ((double) bucket_count));
  int *elem;
  for(i=0; i<N; ++i){
    elem = &list[i];
    current_bucket = &buckets[*elem / bucket_size];
    current_bucket->values[current_bucket->counter] = *elem;
    current_bucket->counter++;
  }

  return buckets;
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

  // Calculate bucket count as the floored square root of N
  int bucket_count = (int) floor(sqrt(((double) N)));
  int buckets_per_process = bucket_count / comm_size;
  int buckets_rest = bucket_count % comm_size;
  int buckets_rest_border = (buckets_per_process + 1) * buckets_rest ;

  // Offsets for merged buckets
  int merged_buckets[N];
  int send_counts_scatter[comm_size];
  memset(send_counts_scatter, 0, sizeof(int)*comm_size);
  int displs[comm_size];
  int merged_buckets_offsets[bucket_count];

  // Create and initialise list with root
  double begin, end;
  int *list;
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

    // Print list before sorting
    if(argc == 3){
      printf("List before sorting:\n");
      print_list(list, N);
    }

    printf("Starting counting sort for problem size %zu\n", N);
    begin = MPI_Wtime();

    // Construct buckets
    struct bucket_t *buckets;
    buckets = construct_buckets(list, N, bucket_count);

    // Merge buckets to one array and bookmark offsets
    int *merged_value = &merged_buckets[0];
    int offset = 0;
    displs[0] = 0;
    int offset_counter = 1;
    int j = -1;
    if (argc == 3) {
      printf("Buckets = %i, Processes = %i, Buckets/Process = %i, Restbuckets = %i, Restborder = %i\n", bucket_count, comm_size, buckets_per_process, buckets_rest, buckets_rest_border); 
    }
    for (i = 0; i < bucket_count; i++) {
      struct bucket_t *current_bucket = &buckets[i];
      
      // Offsets for processes (after scatter)
      merged_buckets_offsets[i] = current_bucket->counter;

      // Offsets for scatter
      j++;
      if ((i <= buckets_rest_border  && j == (buckets_per_process + 1)) || 
	  (i > buckets_rest_border && j == buckets_per_process)) {
	displs[offset_counter] = offset;
	offset_counter++;
	j = 0;
      }
      offset += current_bucket->counter;
      send_counts_scatter[offset_counter-1] += current_bucket->counter;

      // Merge buckets one array
      int k;
      for (k = 0; k < current_bucket->counter; k++) {
	*merged_value = current_bucket->values[k];
	merged_value++;
      }
      free(current_bucket->values);
    }
    free(buckets);
  }

  // Broadcast sendcount, displs and merged_bucket_offsets from root
  MPI_Bcast(send_counts_scatter, comm_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(displs, comm_size, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Bcast(merged_buckets_offsets, bucket_count, MPI_INT, 0, MPI_COMM_WORLD);

  // Scatter merged buckets
  int recv_count = send_counts_scatter[rank];
  int merged_buckets_part[recv_count];
  MPI_Scatterv(merged_buckets, send_counts_scatter, displs, MPI_INT, merged_buckets_part, recv_count, MPI_INT, 0, MPI_COMM_WORLD);

  // Every process quicksorts its buckets
  int start_bucket_offset, end_bucket_offset, i;
  if ((buckets_rest > 0) && 
      ((buckets_per_process + 1) * (rank + 1) <= buckets_rest_border)) {
    start_bucket_offset = rank * (buckets_per_process + 1);
    end_bucket_offset = start_bucket_offset + (buckets_per_process + 1);
  } else {
    start_bucket_offset = buckets_rest * (buckets_per_process + 1) + (rank - buckets_rest) * buckets_per_process;
    end_bucket_offset = start_bucket_offset + buckets_per_process;
  }

  int *part_offset = merged_buckets_part;
  for (i = start_bucket_offset; i < end_bucket_offset; i++) {
    qsort(part_offset, merged_buckets_offsets[i], sizeof(int), cmpfunc);
    part_offset += merged_buckets_offsets[i];
  }
  
  // Gather sorted buckets
  MPI_Gatherv(merged_buckets_part, send_counts_scatter[rank], MPI_INT, list, send_counts_scatter, displs, MPI_INT, 0, MPI_COMM_WORLD);

  if(rank == 0) {
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
