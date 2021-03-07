#!/bin/bash

gcc -O3 -Wall -Werror -o lab1_par_1  main.c -lm -floop-parallelize-all -ftree-parallelize-loops=1
gcc -O3 -Wall -Werror -o lab1_par_2  main.c -lm -floop-parallelize-all -ftree-parallelize-loops=2
gcc -O3 -Wall -Werror -o lab1_par_3  main.c -lm -floop-parallelize-all -ftree-parallelize-loops=3
gcc -O3 -Wall -Werror -o lab1_par_4  main.c -lm -floop-parallelize-all -ftree-parallelize-loops=4
gcc -O3 -Wall -Werror -o lab1_seq  main.c -lm