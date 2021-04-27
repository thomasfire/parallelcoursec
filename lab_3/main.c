#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

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

#define ABS(val) (val >= 0 ? val : -val)
#define SIN(val) __builtin_sin(val)

#define GNOMESORT(arr, n, tt) do { \
    size_t index = 0; \
    while (index < n) { \
        if (index == 0) index++; \
        if (arr[index] >= arr[index - 1]) index++; \
        else { \
            SWAP(arr[index], arr[index - 1], tt); \
            index--; \
        } \
    } \
} while(0)

#define M1MAPF(arr, i, tt) do { \
    tt buff = arr[i] / M_PI;        \
    arr[i] = (buff*buff*buff);           \
} while(0)

static double previous = 0;
#define M2MAPF(arr1, i, tt) do { \
    tt buff = SIN(((arr1)[i] + previous)); \
    previous = (arr1)[i];          \
    (arr1)[i] = ABS(buff);         \
} while (0)

#define MERGEF(arr1, arr2, tt, i) do {(arr2)[i] = pow((arr1)[i], (arr2)[i]);} while(0)

#define RESTRICT_RAND(val, minv, maxv) ((val) % ((maxv) + 1 - (minv)) + (minv))

#define FILL_ARRAY(arr, n, seed, minv, maxv) do{ \
    for (size_t _q = 0; _q < (n); _q++)          \
        {(arr)[_q] = RESTRICT_RAND(rand_r(&(seed)), minv, maxv);} \
}while(0)

#define MAP_ARR(arr, n, tt, func) do{ \
    for (size_t _q = 0; _q < (n); _q++)           \
        func(arr, _q, tt);                    \
}while(0)

#define MERGE_ARR(arr1, arr2, n, tt, func) do{ \
    for (size_t _q = 0; _q < (n); _q++)          \
        func(arr1, arr2, tt, _q);              \
}while(0)

#define STORE_MIN_VAL(storage, arr, n, maxv) do{ \
    (storage)=maxv;                                \
    for (size_t _q = 0; _q < (n); _q++)                  \
        if ((arr)[_q] != 0 && (arr)[_q] < (storage))   \
            (storage) = (arr)[_q];                   \
}while(0)

#define ISVALGOOD(val, minv) (((long long)((val)/(minv)))%2 == 0)

#define REDUCEARR(target, arr, n, tt, maxv) do{ \
    tt buff;                                    \
    STORE_MIN_VAL(buff, arr, n, maxv);          \
    for (size_t _q = 0; _q < (n); _q++)           \
        if (ISVALGOOD((arr)[_q],buff))            \
            (target)+=SIN((arr)[_q]);               \
}while(0)

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        printf("%s N TN\n", argv[0]);
        return 1;
    }

    size_t N = atoll(argv[1]);
    unsigned TN = atol(argv[2]) ?: 1;

    if (!N) {
        printf("Bad args\n");
        return 1;
    }

    struct timeval T1, T2;
    long delta_ms;
    double full_result = 0;
    gettimeofday(&T1, NULL);


    // schedule(static, 2)
#pragma omp parallel for default(none) private(previous) shared(N) reduction(+:full_result) num_threads(TN)
    for (unsigned si = 0; si < EXPERIMENTS; si++) {
        srand(si);
        unsigned copy = si;
        size_t N_2 = N / 2;
        previous = 0;
        double *M1 = malloc(N * sizeof(double));
        double *M2 = malloc((N_2) * sizeof(double));

        FILL_ARRAY(M1, N, copy, 1, A);
        copy = si;
        FILL_ARRAY(M2, N_2, copy, A, A10);

        MAP_ARR(M1, N, double, M1MAPF);
        MAP_ARR(M2, N_2, double, M2MAPF);

        MERGE_ARR(M1, M2, N_2, double, MERGEF);

        GNOMESORT(M2, N_2, double);
        double result = 0;
        REDUCEARR(result, M2, N_2, double, DBL_MAX);
        full_result += result;
        // printf("Experiment %u/%u result: %f\n", si + 1, EXPERIMENTS, result);
        free(M1);
        free(M2);
    }

    gettimeofday(&T2, NULL);
    delta_ms = 1000 * (T2.tv_sec - T1.tv_sec) + (T2.tv_usec - T1.tv_usec) / 1000;
    // printf("Result: %f", full_result);
    // printf("\nN=%zu. Milliseconds passed: %ld\n", N, delta_ms);
    printf("%u,%zu,%ld,%f\n", TN, N, delta_ms, full_result);
    return 0;
}
