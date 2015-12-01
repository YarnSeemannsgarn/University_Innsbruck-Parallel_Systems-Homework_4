#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

void swap(int *a, int *b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

int **create_matrix(size_t M, size_t N)
{
    int *values = malloc(M * N * sizeof(int));
    int **rows = malloc(M * sizeof(int *));
    int i;
    for (i=0; i<M; ++i)
    {
        rows[i] = values + i*N;
    }
    return rows;
}

double **create_double_matrix(size_t M, size_t N)
{
    double *values = malloc(M * N * sizeof(double));
    double **rows = malloc(M * sizeof(double *));
    int i;
    for (i=0; i<M; ++i)
    {
        rows[i] = values + i*N;
    }
    return rows;
}


void destroy_matrix(int **matrix)
{
    free(*matrix);
    free(matrix);
}

void destroy_double_matrix(double **matrix){
  free(*matrix);
  free(matrix);
}

void print_matrix(int **matrix, size_t M, size_t N){
  int i, j;
  for(i=0; i<M; ++i){
    for(j=0; j<N; ++j){
      printf("%*i ", PRINTF_WIDTH, matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_double_matrix(double **matrix, size_t M, size_t N){
  int i, j;
  for(i=0; i<M; ++i){
    for(j=0; j<N; ++j){
      printf("%*.*f ", PRINTF_WIDTH, PRINTF_PRECISION, matrix[i][j]);
    }
    printf("\n");
  }
  printf("\n");
}

void print_list(int *list, size_t N){
  int i;
  for(i=0; i<N; ++i){
    printf("%i, ", list[i]);
  }
  printf("\n\n");
}
