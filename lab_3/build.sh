#!/bin/bash

gcc -O3 -Werror -o lab3_gcc main.c -lm -fopenmp
clang -O3 -Werror -o lab3_clang  main.c -lm -fopenmp
