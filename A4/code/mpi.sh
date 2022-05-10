#!/bin/bash
#PBS -l nodes=1:ppn=5,mem=1g
#PBS -q batch
#PBS -m abe
#PBS -V

echo Name: Chen Yuan
echo Student ID: 117010038
echo Assignment 4, Heat-Distribution Simulation, MPI implementation.
echo processors, size, runtime
echo;

mpirun  -n 1 ./mpi 50 
mpirun  -n 2 ./mpi 50 
mpirun  -n 3 ./mpi 50
mpirun  -n 4 ./mpi 50
mpirun  -n 5 ./mpi 50
mpirun  -n 6 ./mpi 50   
mpirun  -n 7 ./mpi 50  
mpirun  -n 8 ./mpi 50 
mpirun  -n 9 ./mpi 50 
mpirun  -n 10 ./mpi 50 
mpirun  -n 11 ./mpi 50 
mpirun  -n 12 ./mpi 50 
mpirun  -n 13 ./mpi 50 
mpirun  -n 14 ./mpi 50 
mpirun  -n 15 ./mpi 50 
mpirun  -n 16 ./mpi 50 


mpirun  -n 1 ./mpi 200 
mpirun  -n 2 ./mpi 200
mpirun  -n 3 ./mpi 200
mpirun  -n 4 ./mpi 200
mpirun  -n 5 ./mpi 200
mpirun  -n 6 ./mpi 200  
mpirun  -n 7 ./mpi 200 
mpirun  -n 8 ./mpi 200
mpirun  -n 9 ./mpi 200
mpirun  -n 10 ./mpi 200 
mpirun  -n 11 ./mpi 200
mpirun  -n 12 ./mpi 200
mpirun  -n 13 ./mpi 200
mpirun  -n 14 ./mpi 200
mpirun  -n 15 ./mpi 200
mpirun  -n 16 ./mpi 200

mpirun  -n 1 ./mpi 800 
mpirun  -n 2 ./mpi 800
mpirun  -n 3 ./mpi 800
mpirun  -n 4 ./mpi 800
mpirun  -n 5 ./mpi 800
mpirun  -n 6 ./mpi 800  
mpirun  -n 7 ./mpi 800 
mpirun  -n 8 ./mpi 800
mpirun  -n 9 ./mpi 800
mpirun  -n 10 ./mpi 800 
mpirun  -n 11 ./mpi 800
mpirun  -n 12 ./mpi 800
mpirun  -n 13 ./mpi 800
mpirun  -n 14 ./mpi 800
mpirun  -n 15 ./mpi 800
mpirun  -n 16 ./mpi 800

mpirun  -n 1 ./mpi 2000
mpirun  -n 2 ./mpi 2000
mpirun  -n 3 ./mpi 2000
mpirun  -n 4 ./mpi 2000
mpirun  -n 5 ./mpi 2000
mpirun  -n 6 ./mpi 2000 
mpirun  -n 7 ./mpi 2000
mpirun  -n 8 ./mpi 2000
mpirun  -n 9 ./mpi 2000
mpirun  -n 10 ./mpi 2000 
mpirun  -n 11 ./mpi 2000
mpirun  -n 12 ./mpi 2000
mpirun  -n 13 ./mpi 2000
mpirun  -n 14 ./mpi 2000
mpirun  -n 15 ./mpi 2000
mpirun  -n 16 ./mpi 2000