#ifndef UTIL_H
#define UTIL_H

#include <stddef.h>

#define RANGE_MAX 10000
#define PRINTF_WIDTH 10
#define PRINTF_PRECISION 4
#define MEMORY_MESSAGE "Not enough memory. Choose a smaller problem size!\n"
#define USAGE_MESSAGE "Usage: %s <problem-size> [<debug>]\n"

void swap(int *a, int *b);
void print_list(int *list, size_t N);

#endif
