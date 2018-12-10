#!usr/bin/bash

mpic++ -o main main.cpp
mpirun -np 11 main
