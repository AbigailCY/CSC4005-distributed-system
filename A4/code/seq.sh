#!/bin/bash
#PBS -l nodes=1:ppn=5,mem=1g
#PBS -q batch
#PBS -m abe
#PBS -V


echo Name: Chen Yuan
echo Student ID: 117010038
echo Assignment 4, Heat-Distribution Simulation, Sequential implementation.
echo size, runtime
echo;
./seq 50
echo;
./seq 200
echo;
./seq 800
echo;
./seq 2000



