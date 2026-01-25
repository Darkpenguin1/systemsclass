/*
 * axpy: Y += a*X[N]
 */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <sys/timeb.h>
#include <pthread.h>


/* read timer in second */
double read_timer() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time + (double) tm.millitm / 1000.0;
}

/* read timer in ms */
double read_timer_ms() {
    struct timeb tm;
    ftime(&tm);
    return (double) tm.time * 1000.0 + (double) tm.millitm;
}

#define REAL float
#define VECTOR_LENGTH 102400

/* initialize a vector with random floating point numbers */
void init(REAL A[], int N) {
    int i;
    for (i = 0; i < N; i++) {
        A[i] = (double) drand48();
    }
}

void axpy_kernel(int N, REAL *Y, REAL *X, REAL a) {
    int i;
    for (i = 0; i < N; ++i)
        Y[i] += a * X[i];
}

/**
 * Your implementation of pthread version of axpy computation using look chunking and worksharing
 * by dividing the total number of iterations amount the num_threads for parallel computing. 

 
This is the plan -> First we will divide the workload between threads within the main thread
for ex have a for loop that will iterate over the num of threads needed for ex we have 2 threads and N = 100
What I will do is that I will give the first thread 0-49 and thread 2 50-100 

This division will be done by simply doing N // num_threads we will use a floor division to be on the safe
side not sure if there will be collisions though 
 
The args needed for the worker function is just the same except num threads, start and end 
We will call pthread create here as well 
 
 */

typedef struct{
    int start; 
    int end; 
    REAL *X; 
    REAL *Y; 
    REAL a;
} ThreadArgs;

void *workerFunction(void *arg){
    ThreadArgs* thread = (ThreadArgs*) arg;

    for (int t = thread->start; t < thread->end; t++){
        thread->Y[t] += thread->a * thread->X[t];
    }

    return 0;
}
void axpy_kernel_threading(int N, REAL *Y, REAL *X, REAL a, int num_threads) {
    // int i;
    // for (i = 0; i < N; ++i)
    //     Y[i] += a * X[i];
    /*
        First step is dividing the arr size of course 
    */
    int blockSize = N / num_threads;
    int remainder = N % num_threads;
    pthread_t id;
    pthread_t threadArray[num_threads];
    ThreadArgs args[num_threads];
    int rc;
    int start = 0; 

    for (int i = 0; i < num_threads; i++){
        int extra = (i < remainder) ? 1 : 0; // frontload the remainders to the first few threads 
        int end = start + extra + blockSize;

        args[i].start = start;
        args[i].end = end;
        args[i].X = X;
        args[i].Y = Y;
        args[i].a = a;
        


        rc = pthread_create(&threadArray[i], NULL, workerFunction, (void*)&args[i]);

        start = end; 
        
       //  rc = pthread_create(&threadArray[i], NULL, printMessage, (void*)index);
    }
    for (int t = 0; t < num_threads; t++){
        pthread_join(threadArray[t], NULL);
    }
    

}



/* void doWorkThread(int N, REAL *Y, REAL *X, REAL a); helper function that will actually do work


same sort of loop though int i = passed in val for ex thread one will be i = 0 

for (i = 0; i < N; i++){
    Y[i] += a * X[i]
}


*/

int main(int argc, char *argv[]) {
    int N = VECTOR_LENGTH;
    int num_threads = 4;
    if (argc < 2) {
        fprintf(stderr, "Usage: axpy <n> [<num_threads>] (The default of n is %d, the default num_threads is %d)\n", N, num_threads);
    } else if (argc == 2) N = atoi(argv[1]);
    else {
        N = atoi(argv[1]);
        num_threads = atoi(argv[2]);
    }
    REAL *X = (REAL*)malloc(sizeof(REAL)*N);
    REAL *Y = (REAL*)malloc(sizeof(REAL)*N);

    srand48((1 << 12));
    init(X, N);
    init(Y, N);
    int num_runs = 10; int i;
    REAL a = 0.1234;
    /* example run */
    double elapsed; /* for timing */
    elapsed = read_timer();
    for (i=0; i<num_runs; i++) axpy_kernel(N, Y, X, a);
    elapsed = (read_timer() - elapsed)/num_runs;
    
    double elapsed2; /* for timing */
    elapsed2 = read_timer();
    for (i=0; i<num_runs; i++) axpy_kernel_threading(N, Y, X, a, num_threads);
    elapsed2 = (read_timer() - elapsed2)/num_runs;
    printf("======================================================================================================\n");
    printf("\tAXPY %d numbers, serial and threading\n", N);
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("Performance:\t\tRuntime (ms)\t MFLOPS \n");
    printf("------------------------------------------------------------------------------------------------------\n");
    printf("AXPY-serial:\t\t%4f\t%4f\n", elapsed * 1.0e3, 2*N / (1.0e6 * elapsed));
    printf("AXPY-%d threads:\t\t%4f\t%4f\n", num_threads, elapsed2 * 1.0e3, 2*N / (1.0e6 * elapsed2));
    return 0;
}
