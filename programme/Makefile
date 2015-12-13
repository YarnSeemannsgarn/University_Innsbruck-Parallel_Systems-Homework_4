CC=mpicc
CFLAGS=-c -Wall
SOURCES=counting_sort_mpi.c bucket_sort_mpi.c util.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLES=counting_sort_mpi bucket_sort_mpi

all: $(EXECUTABLES)

counting_sort_mpi: $(OBJECTS)
	$(CC) counting_sort_mpi.o util.o -o $@

bucket_sort_mpi: $(OBJECTS)
	$(CC) bucket_sort_mpi.o util.o -o $@

.c.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm -f $(EXECUTABLES) $(OBJECTS) 
