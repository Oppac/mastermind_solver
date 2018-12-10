#!usr/bin/bash

module load OpenMPI/2.1.1-GCC-6.4.0-2.28
mpic++ -o main main.cpp
mpirun -np 11 main
