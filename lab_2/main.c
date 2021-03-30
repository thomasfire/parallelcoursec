#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "ipp.h"

#define DBL_MAX 1.79769e+308

// Var = 1 + (6 * 4 * 13) % (7,8,6,7) = 1 + 312 % (7,8,6,7)
// Map M1 - 5 - (M1[i] / PI) ^ 3
// Map M2 - 1 - abs(sin(M2[i] + M2[i-1]))
// Merge -  1 - M1[i] ^ M2[i]
// Sort  -  5 - Gnome Sort

#define EXPERIMENTS 50
#define A 312
#define A10 3120

#define SWAP(x, y, tt) do { \
    tt buff = x;            \
    x=y;                    \
    y=buff;                 \
} while(0)

#define SIN(val) __builtin_sin(val)

#define GNOMESORT(arr, n, tt) do { \
    size_t index = 0; \
    while (index < n) { \
        if (index == 0) index++; \
        if (arr[index] >= arr[index - 1]) index++; \
        else { \
            SWAP(arr[index], arr[index - 1], tt);  \
            index--; \
        } \
    } \
} while(0)

#define M1MAPF(arr, n) do { \
    ippsDivC_64f(arr, M_PI, arr, n); /* поэлементно делим на пи */ \
    ippsPowx_64f_A53(arr, 3, arr, n); /* поэлементно возводим в третью степень */ \
} while(0)

#define M2MAPF(arr1, arr2, n) do { \
    ippsAdd_64f(arr1, arr2, arr1, n); /* поэлементно складываем */\
    ippsSin_64f_A53(arr1, arr1, n); /* поэлементно пишем синус */   \
    ippsAbs_64f_A53(arr1, arr1, n); /* поэлементно берем модуль */        \
} while (0)

#define MERGEF(arr1, arr2, n) do {ippsPow_64f_A53(arr1, arr2, arr2, n);} while(0) // поэлементно возводим в степень

#define RESTRICT_RAND(val, minv, maxv) (val % (maxv + 1 - minv) + minv)

#define FILL_ARRAY(arr, n, seed, minv, maxv) do{ \
    for (size_t _q = 0; _q < n; _q++)          \
        {arr[_q] = RESTRICT_RAND(rand_r(&seed), minv, maxv);} \
}while(0)

#define STORE_MIN_VAL(storage, arr, n) do{ ippsMin_64f(arr, n, &storage); }while(0) // находим минимум в массиве

#define ISVALGOOD(val, minv) (((long long)(val/minv))%2 == 0)

#define REDUCEARR(target, arr, n, tt) do{ \
    tt buff;                                    \
    STORE_MIN_VAL(buff, arr, n);          \
    for (size_t _q = 0; _q < n; _q++)           \
        if (ISVALGOOD(arr[_q],buff))            \
            target+=SIN(arr[_q]);               \
}while(0)

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        printf("%s N TN\n", argv[0]);
        return 1;
    }

    size_t N = atoll(argv[1]);
    size_t TN = atoll(argv[2]) ?: 1;

    // printf("ippInit status: %u\n", ippInit());
    // printf("ippSetNumThreads status: %u\n", ippSetNumThreads(TN));
    ippInit();
    ippSetNumThreads(TN);

    if (!N) {
        printf("Bad args\n");
        return 1;
    }

    struct timeval T1, T2;
    long delta_ms;
    double full_result = 0;
    gettimeofday(&T1, NULL);

    size_t N_2 = N / 2;
    double *M1 = ippMalloc(N * sizeof(double));
    double *M2 = ippMalloc((N_2) * sizeof(double));
    double *M2_CP = ippMalloc((N_2 + 1) * sizeof(double));

    for (unsigned si = 0; si < EXPERIMENTS; si++) {
        srand(si);
        unsigned copy = si;

        M2_CP[0] = 0;

        FILL_ARRAY(M1, N, copy, 1, A);
        copy = si;
        FILL_ARRAY(M2, N_2, copy, A, A10);
        ippsCopy_64f(M2, &M2_CP[1], N_2); // типа memcpy

        M1MAPF(M1, N);
        M2MAPF(M2, M2_CP, N_2);

        MERGEF(M1, M2, N_2);

        GNOMESORT(M2, N_2, double);
        double result = 0;
        REDUCEARR(result, M2, N_2, double);
        full_result += result;
        // printf("Experiment %u/%u result: %f\n", si + 1, EXPERIMENTS, result);
    }
    ippFree(M1);
    ippFree(M2);
    ippFree(M2_CP);

    gettimeofday(&T2, NULL);
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
    // printf("Result: %f", full_result);
    // printf("\nN=%zu. Milliseconds passed: %ld\n", N, delta_ms);
    printf("%zu,%ld,%f\n", N, delta_ms, full_result);
    return 0;
}
