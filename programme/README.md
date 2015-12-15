How to run the programme/s
==========================

First of all build all programmes:

    $ make
    
Afterwards you can run the programme (e.g. counting_sort ) with mpirun as follows (do not forget to load the openmpi module):

    $ mpirun -np <processes> ./counting_sort_mpi <problem-size>
    
If you want to submit a job (e.g. bucket_sort) you have to run the appropriate script:

    $ ./bucket_sort.sh <processes> <problem-size>
    
If you want to clean up just do:

    $ make clean
    
Measurements (Task 4)
=====================

Host machine: LCC Cluster with 5 nodes, where each node contains 8 CPUs
Measurement technique: MPI_Wtime()

For each programme I tested different big problem sizes. For the following I found relatively "good" efficencies.

Programme: counting_sort_mpi with problem size 500000000

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |          20.33	|       - |         -
2      |          28.75 |    0.70 |      0.35
4      |          36.33 |    0.56 |      0.14
8      |          29.50 |    0.68 |      0.09
16     |          27.45 |    0.74 |      0.05

Here the runtimes are nearly the same. The speedup is lower than 1, because the communication overhead increases the runtimes. I tried to use bigger problem sizes, but I ran into memory problems then, even with flag -l h_vmem=8g. The efficencies can obviously not be considered. Either the algorithm is not appropriate for parallelizing counting sort or the implementation has to be adapted.


Programme: bucket sort with problem size 8000

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |           0.75 |       - |         -
2      |          19.40 |    0.04 |      0.02  
4      |          18.82 |    0.04 |	 0.01
8      |          17.93 |    0.04 |	 0.01
16     |          17.82 |    0.04 |	 0.01
32     |          17.96 |    0.04 |	 0.01       |



Programme: selection_sort with problem size 50000

Nprocs | Runtime (in s) | Speedup | Efficency
:------|---------------:|--------:|---------:
1      |           4.83 |       - |         -
2      |          43.78 |    0.11 |      0.56
4      |          44.67 |    0.11 |	 0.28
8      |          46.74 |    0.10 |      0.01
16     |          54.80 |    0.09 |	 0.01

Here the sequential runtime is much better than the parallized runtimes. The speedup is lower than 1, because the communication overhead increases the runtimes. I tried to use bigger problem sizes, but the tuntimes for the parallized parts were too large then. The efficencies can obviously not be considered. Either the algorithm is not appropriate for parallelizing counting sort or the implementation has to be adapted.