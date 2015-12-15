#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: $0 nprocs problem-size"
	exit 1
fi

nprocs=$1
problemsize=$2
qsub -cwd -o "bucket_sort_mpi_${nprocs}_${problemsize}.dat" -e "bucket_sort_mpi_${nprocs}_${problemsize}.dat" -N "bucket_sort_mpi" -l h_vmem=8g -pe openmpi-roundrobin $nprocs helper.sge $nprocs ./bucket_sort_mpi $problemsize
