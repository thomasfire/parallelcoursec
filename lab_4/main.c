#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>

#ifndef NOSTATUS
#include <unistd.h>
#include <pthread.h>
#endif

#define DBL_MAX 1.79769e+308

#ifdef _OPENMP
#include "omp.h"
#else
// это специальная конструкция, позволяющая вернуть значение.
#define omp_get_wtime() ({ \
    struct timeval T; \
    gettimeofday(&T, NULL);                     \
    (double)((double)(T.tv_sec) + (double)(T.tv_usec)/100000); \
})
#endif

// Var = 1 + (6 * 4 * 13) % (7,8,6,7) = 1 + 312 % (7,8,6,7)
// Map M1 - 5 - (M1[i] / PI) ^ 3
// Map M2 - 1 - abs(sin(M2[i] + M2[i-1]))
// Merge -  1 - M1[i] ^ M2[i]
// Sort  -  5 - Gnome Sort

#ifndef EXPERIMENTS
#define EXPERIMENTS 50
#endif

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

#define STRINGIFY(a) #a
// _Pragma - спасибо старым стандартам, теперь можно засунуть прагмы в дефайны
// Оно в итоге разворачивается в `#pragma omp parallel for default(none) shared(arr, n) reduction(min:storage) num_threads(TN)`
#define STORE_MIN_VAL(storage, arr, n, maxv) do{ \
    (storage)=maxv;                              \
     _Pragma( STRINGIFY(omp parallel for default(none) shared(arr, n) reduction(min:storage) num_threads(TN)) )\
    for (size_t _q = 0; _q < (n); _q++)                  \
        if ((arr)[_q] != 0 && (arr)[_q] < (storage))   \
            (storage) = (arr)[_q];                   \
}while(0)

#define ISVALGOOD(val, minv) (((long long)((val)/(minv)))%2 == 0)

#define REDUCEARR(target, arr, n, tt, maxv) do{ \
    tt buff;                                    \
    STORE_MIN_VAL(buff, arr, n, maxv);          \
    _Pragma( STRINGIFY(omp parallel for default(none) shared(buff, arr, n) reduction(+:target) num_threads(TN)) )\
    for (size_t _q = 0; _q < (n); _q++)           \
        if (ISVALGOOD((arr)[_q],buff))            \
            (target)+=SIN((arr)[_q]);               \
}while(0)

#define MERGE_SORTED(arr, szn, target) do { \
    const unsigned middle = (szn)/2;        \
    unsigned last_1 = 0, last_2 = middle;\
    for (size_t _q = 0; _q < (szn); _q++) { \
        if (last_2 >= (szn) || ((arr)[last_1] < (arr)[last_2] && last_1 < middle)) (target)[_q] = (arr)[last_1++];  \
        else (target)[_q] = (arr)[last_2++];  \
    }\
} while (0)


#ifndef NOSTATUS
static void *state_notifyer(void *data) {
    unsigned *si = (unsigned *) data;

    while (*si < EXPERIMENTS) {
        printf("Status: %u%%\n", (unsigned)((*si) * (100.0 / (double) EXPERIMENTS)));
        sleep(1);
    }
    return NULL;
}
#endif

int main(int argc, const char *argv[]) {
    if (argc != 3) {
        printf("%s N TN\n", argv[0]);
        return 1;
    }

    const size_t N = atoll(argv[1]);
    const unsigned TN = atol(argv[2]) ?: 1;

    if (!N) {
        printf("Bad args\n");
        return 1;
    }

    unsigned delta_ms;
    double full_result = 0;
    unsigned si = 0;

    /**
     * Спасибо Клименкову за отличные курсы!
     * Интересная херня: я попытался сунуть всё под omp sections, но вместо этого время исполнения увеличилась с
     * 550 мс до 2х секунд! Возможно там надо было таймеры по-хитрому вставлять (тем более что ровно 2 секунды было),
     * но я уже потоки вставил, пусть так будет, тем более что теперь оно работает без опенмп (зато без птредов не работает больше)
     * */
#ifndef NOSTATUS
    pthread_t second_thread;
    pthread_create(&second_thread, NULL, state_notifyer, &si);
#endif

    const double T1 = omp_get_wtime();

    for (si = 0; si < EXPERIMENTS; si++) {
        srand(si);
        unsigned copy = si;
        const size_t N_2 = N / 2;
        previous = 0;
        double *M1 = malloc(N * sizeof(double));
        double *M2 = malloc((N_2) * sizeof(double));

#pragma omp parallel sections private(copy) shared(si) num_threads(2)
        {
#pragma omp section
            {
                copy = si;
                FILL_ARRAY(M1, N, copy, 1, A);
            }
#pragma omp section
            {
                copy = si;
                FILL_ARRAY(M2, N_2, copy, A, A10);
            }
        }

#pragma omp parallel sections num_threads(2)
        {
#pragma omp section
            { MAP_ARR(M1, N, double, M1MAPF); }
#pragma omp section
            { MAP_ARR(M2, N_2, double, M2MAPF); }
        }

        MERGE_ARR(M1, M2, N_2, double, MERGEF);

#pragma omp parallel sections shared(N_2, M2) num_threads(2)
        {
#pragma omp section
            {
                const unsigned msize = N_2 / 2;
                GNOMESORT((M2), (msize), double);
            }
#pragma omp section
            {
                double *M2P = &M2[N_2 / 2];
                const unsigned msize = N_2 - N_2 / 2;
                GNOMESORT(M2P, msize, double);
            }
        }

        MERGE_SORTED(M2, N_2, M1); // да, я клоун 🤡

        double result = 0;
        REDUCEARR(result, M1, N_2, double, DBL_MAX);

        full_result += result;
        // printf("Experiment %u/%u result: %f\n", si + 1, EXPERIMENTS, result);
        free(M1);
        free(M2);
    }


    const double T2 = omp_get_wtime();
    delta_ms = (unsigned) (1000 * (T2 - T1));

#ifndef NOSTATUS
    int32_t *result;
    pthread_cancel(second_thread);
    pthread_join(second_thread, (void **) &result);
#endif
    // printf("Result: %f", full_result);
    // printf("\nN=%zu. Milliseconds passed: %ld\n", N, delta_ms);
    printf("%u,%zu,%u,%f\n", TN, N, delta_ms, full_result);
    return 0;
}
