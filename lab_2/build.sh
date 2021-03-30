#!/bin/bash

icx -O3 -Wall -Werror -o lab_2_clang  main.c -I $IPPROOT/include -L $IPPROOT/lib/intel64/tl/tbb/ -lippi_tl_tbb -lipps -lippcore_tl_tbb -lippvm -lippcore -lm
icc -O3 -Wall -Werror -o lab_2_intel  main.c -I $IPPROOT/include -L $IPPROOT/lib/intel64/tl/tbb/ -lippi_tl_tbb -lipps -lippcore_tl_tbb -lippvm -lippcore -lm