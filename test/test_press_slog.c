/* -------------------------------------------------------------------------- */
/* -------------- DEPENDANCIES ---------------------------------------------- */

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <pthread.h>
#include <time.h>

#include "logger.h"

static long loop_count;

static void diff(struct timespec *start, struct timespec *end, struct timespec *interv)
{
    if ((end->tv_nsec - start->tv_nsec) < 0) {
        interv->tv_sec = end->tv_sec - start->tv_sec-1;
        interv->tv_nsec = 1000000000 + end->tv_nsec - start->tv_nsec;
    } else {
        interv->tv_sec = end->tv_sec - start->tv_sec;
        interv->tv_nsec = end->tv_nsec - start->tv_nsec;
    }
    return;
}

static void *work(void *ptr)
{
    long j = loop_count;
    struct timespec start, end, interv;
    clock_gettime(CLOCK_MONOTONIC, &start);
    while (j-- > 0) {
        slog_info("test", "%ld %s", j, "hello world");
    }
    clock_gettime(CLOCK_MONOTONIC, &end);

    diff(&start, &end, &interv);
    printf("cost time nsec %ld.\n", interv.tv_sec * 1000000000 + interv.tv_nsec);

    return 0;
}

int main(int argc, char** argv)
{
    int j = 0;

    if (argc != 3) {
        fprintf(stderr, "test nprocess nthreads nloop\n");
        exit(1);
    }

    log_init();

    int thread_count = 0;
    thread_count = atol(argv[1]);
    loop_count = atol(argv[2]);

    pthread_t tid[thread_count];
    for (j = 0; j < thread_count; j++) {
        pthread_create(&(tid[j]), NULL, work, NULL);
    }
    for (j = 0; j < thread_count; j++) {
        pthread_join(tid[j], NULL);
    }

    log_fini();

    return 0;
}


/* ============== EOF ======================================================= */