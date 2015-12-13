#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "util.h"

void swap(int *a, int *b){
  int tmp = *a;
  *a = *b;
  *b = tmp;
}

void print_list(int *list, size_t N){
  int i;
  for(i=0; i<N; ++i){
    printf("%i, ", list[i]);
  }
  printf("\n\n");
}
