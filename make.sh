#!usr/bin/bash

mpic++ -o main main.cpp
mpirun -np 6 main
