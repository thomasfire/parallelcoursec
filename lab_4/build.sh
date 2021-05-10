#!/bin/bash

gcc -O3 -Werror -o lab4_gcc_stat main.c -lm -fopenmp -lpthread
gcc -O3 -Werror -o lab4_gcc main.c -lm -fopenmp -lpthread -DNOSTATUS
gcc -O3 -Werror -o lab4_gcc_no_omp main.c -lm -lpthread
