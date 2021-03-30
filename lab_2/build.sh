#!/bin/bash

icx -O3 -Wall -Werror -o lab_2_clang  main.c -I $IPPROOT/include -L $IPPROOT/lib/intel64 -lippi -lipps -lippcore -lippvm
icc -O3 -Wall -Werror -o lab_2_intel  main.c -I $IPPROOT/include -L $IPPROOT/lib/intel64 -lippi -lipps -lippcore -lippvm