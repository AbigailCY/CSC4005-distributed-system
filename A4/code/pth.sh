#!/bin/bash
#PBS -l nodes=1:ppn=5,mem=1g
#PBS -q batch
#PBS -m abe
#PBS -V


echo Name: Chen Yuan
echo Student ID: 117010038
echo Assignment 4, Heat-Distribution Simulation, Pthread implementation.
echo threads, size, runtime
echo;
./pth 1  50 
./pth 2  50 
./pth 3  50
./pth 4  50
./pth 5  50
./pth 6  50   
./pth 7  50  
./pth 8  50 
./pth 9  50 
./pth 10  50 
./pth 11  50 
./pth 12  50 
./pth 13  50 
./pth 14  50 
./pth 15  50 
./pth 16  50 


./pth 1  200 
./pth 2  200
./pth 3  200
./pth 4  200
./pth 5  200
./pth 6  200  
./pth 7  200 
./pth 8  200
./pth 9  200
./pth 10  200 
./pth 11  200
./pth 12  200
./pth 13  200
./pth 14  200
./pth 15  200
./pth 16  200

./pth 1  800 
./pth 2  800
./pth 3  800
./pth 4  800
./pth 5  800
./pth 6  800  
./pth 7  800 
./pth 8  800
./pth 9  800
./pth 10  800 
./pth 11  800
./pth 12  800
./pth 13  800
./pth 14  800
./pth 15  800
./pth 16  800

./pth 1  2000
./pth 2  2000
./pth 3  2000
./pth 4  2000
./pth 5  2000
./pth 6  2000 
./pth 7  2000
./pth 8  2000
./pth 9  2000
./pth 10  2000 
./pth 11  2000
./pth 12  2000
./pth 13  2000
./pth 14  2000
./pth 15  2000
./pth 16  2000