#!/bin/bash

if [ $# -ne 2 ]; then
	echo "usage: $0 nprocs problem-size"
	exit 1
fi

nprocs=$1
problemsize=$2
qsub -cwd -o "selection_sort_mpi_${nprocs}_${problemsize}.dat" -e "selection_sort_mpi_${nprocs}_${problemsize}.dat" -N "selection_sort_mpi" -pe openmpi-roundrobin $nprocs helper.sge $nprocs ./selection_sort_mpi $problemsize
