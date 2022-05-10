#!/bin/bash
g++ ./seq.cpp -o ./seq -lX11
mpic++ ./mpi.cpp -lX11 -o ./mpi 
g++ ./pth.cpp -o ./pth -lpthread -lX11
g++ -o ./openmp ./openmp.cpp -fopenmp
mpic++ ./mpi_omp.cpp -lX11 -o ./mpi_omp -fopenmp

sh seq.sh
sh mpi.sh
sh openmp.sh
sh pth.sh

OPM_NUM_THREADS=4 mpirun -n 9 ./mpi_omp 50
OPM_NUM_THREADS=4 mpirun -n 5 ./mpi_omp 200
OPM_NUM_THREADS=4 mpirun -n 14 ./mpi_omp 800
OPM_NUM_THREADS=4 mpirun -n 14 ./mpi_omp 2000