#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string>
#include <iostream>

#define MAX 10
#define MM 600
#define NN 800
#define KK 400
// #define wypisz

/* create thread argument struct for thr_func() */
typedef struct thread_data_t {
    int tid;
    int **A, **B, **C; // C=A*B;
    int m, n, k;
} thread_data_t;

/* shared data between threads */
int i, j;
pthread_mutex_t lock_x;

void print(int **A, int m, int n) {
    for (int i = 0; i < m; i++) {
        for (int j = 0; j < n; j++) {
            printf("%d, ", A[i][j]);
        }
        printf("\n");
    }
}

int **rand(int m, int n) {
    int **A = new int *[m];
    for (int i = 0; i < m; i++) {
        A[i] = new int[n];
        for (int j = 0; j < n; j++) {
            A[i][j] = rand() % MAX;
        }
    }
    return A;
}

void del(int **A, int m, int n) {
    for (int i = 0; i < m; i++) {
        delete[] A[i];
    }
    delete[] A;
}

void *thr_func(void *arg) {
    thread_data_t *data = (thread_data_t *)arg;

    // printf("Th: %d\n", data->tid);
    int wi;
    while (i < data->m && j < data->n) {
        pthread_mutex_lock(&lock_x);
        if (i < data->m) {
            wi = i;
            i++;
        }
        pthread_mutex_unlock(&lock_x);
        printf("Th: %d, (%d)\n",data->tid,wi);
        for (int j = 0; j < data->n; j++) {
            data->C[wi][j] = 0;
            for (int kk = 0; kk < data->k; kk++) {
                data->C[wi][j] += data->A[wi][kk] * data->B[kk][j];
            }
        }
    }
    pthread_exit(NULL);
    return NULL;
}

int main(int argc, char **argv) {
    int NThreads = 12;
    if (argc > 1) {
        NThreads = std::stoi(argv[1]);
    }
    printf("Liczba watkow to %d\n", NThreads);
    pthread_t thr[NThreads];
    int i, rc;
    /* create a thread_data_t argument array */
    thread_data_t thr_data[NThreads];

    /* initialize shared data */
    i = 0;
    j = 0;
    int **A = rand(MM, KK);
    int **B = rand(KK, NN);
    int **C = rand(MM, NN);
#ifdef wypisz
    print(A, MM, KK);
    printf("********************\n");
    print(B, KK, NN);
    printf("====================\n");
#endif
    /* initialize pthread mutex protecting "shared_x" */
    pthread_mutex_init(&lock_x, NULL);

    /* create threads */
    for (i = 0; i < NThreads; ++i) {
        thr_data[i].tid = i;
        thr_data[i].m = MM;
        thr_data[i].n = NN;
        thr_data[i].k = KK;
        thr_data[i].A = A;
        thr_data[i].B = B;
        thr_data[i].C = C;
        if ((rc = pthread_create(&thr[i], NULL, thr_func, &thr_data[i]))) {
            fprintf(stderr, "error: pthread_create, rc: %d\n", rc);
            return EXIT_FAILURE;
        }
    }
    /* block until all threads complete */
    for (i = 0; i < NThreads; ++i) {
        pthread_join(thr[i], NULL);
    }
#ifdef wypisz
    print(C, MM, NN);
#endif
    del(A, MM, KK);
    del(B, KK, NN);
    del(C, MM, NN);
    return EXIT_SUCCESS;
}
