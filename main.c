#include <ctype.h>
#include <float.h>
#include <pthread.h>
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include "tests/tests.h"

#define EXPERIMENTS_AMOUNT 1
#define min(x, y) ((x) <= (y) ? (x) : (y))

void fill_array(ARRAY_TYPE *arr, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        arr[i] = i;
    }
}

typedef struct {
    void (*tests_function) (int);

    int threads_amount;
    
    pthread_barrier_t barrier;
} Args;

static Args global_args;

double get_experiment(double (*experiment_function) (), ARRAY_TYPE * reqired_arr, ARRAY_TYPE *non_required_arr, 
size_t size) {
    if (non_required_arr != NULL) {
        return experiment_function(reqired_arr, non_required_arr, size);
    } else {
        return experiment_function(reqired_arr, size);
    }
}

double get_best_experiment(double (*experiment_function) (), ARRAY_TYPE * reqired_arr, ARRAY_TYPE *non_required_arr, 
size_t size) {
    get_experiment(experiment_function, reqired_arr, non_required_arr, size);
    double best = DBL_MAX;
    for (size_t i = 0; i < EXPERIMENTS_AMOUNT; ++i) {
        best = min(best, get_experiment(experiment_function, reqired_arr, non_required_arr, size));
    }
    get_experiment(experiment_function, reqired_arr, non_required_arr, size);
    
    return best;
}

void test_load(int thread_id) {
    ARRAY_TYPE *load_arr = (ARRAY_TYPE *)malloc(ARRAY_SIZE * sizeof(ARRAY_TYPE));
    fill_array(load_arr, ARRAY_SIZE);

    pthread_barrier_wait(&(global_args.barrier));

    printf("Thread %d load\n", thread_id);
    printf("Thread %d simple: %f\n", thread_id, get_best_experiment(simple_load_test, load_arr, NULL, ARRAY_SIZE));
    printf("Thread %d memcpy: %f\n", thread_id, get_best_experiment(memcpy_load_test, load_arr, NULL, ARRAY_SIZE));
    printf("Thread %d intrin: %f\n", thread_id, get_best_experiment(intrinsics_load_test, load_arr, NULL, ARRAY_SIZE));

    free(load_arr);
}

void test_store(int thread_id) {
    ARRAY_TYPE *store_arr = (ARRAY_TYPE *)malloc(ARRAY_SIZE * sizeof(ARRAY_TYPE));

    pthread_barrier_wait(&(global_args.barrier));

    printf("Thread %d store\n", thread_id);
    printf("Thread %d simple: %f\n", thread_id, get_best_experiment(simple_store_test, store_arr, NULL, ARRAY_SIZE));
    printf("Thread %d memset: %f\n", thread_id, get_best_experiment(memset_store_test, store_arr, NULL, ARRAY_SIZE));
    printf("Thread %d intrin: %f\n", thread_id, get_best_experiment(intrinsics_store_test, store_arr, NULL, ARRAY_SIZE));

    free(store_arr);
}

void test_copy(int thread_id) {
    ARRAY_TYPE *in = (ARRAY_TYPE *)malloc(ARRAY_SIZE * sizeof(ARRAY_TYPE)),
                *out = (ARRAY_TYPE *)malloc(ARRAY_SIZE * sizeof(ARRAY_TYPE));
    fill_array(in, ARRAY_SIZE);

    pthread_barrier_wait(&(global_args.barrier));

    printf("Thread %d copy\n", thread_id);
    printf("Thread %d simple: %f\n", thread_id, get_best_experiment(simple_copy_test, in, out, ARRAY_SIZE));
    printf("Thread %d memcpy: %f\n", thread_id, get_best_experiment(memcpy_copy_test, in, out, ARRAY_SIZE));
    printf("Thread %d intrin: %f\n", thread_id, get_best_experiment(intrinsics_copy_test, in, out, ARRAY_SIZE));

    free(in);
    free(out);
}

void test_all(int thread_id) {
    test_load(thread_id);
    test_store(thread_id);
    test_copy(thread_id);
}

Args get_default_args() {
        Args args;

        args.tests_function = test_all;
        args.threads_amount = 1;

        return args;
    }

Args parse_args(int argc, char *argv[]) { 
    Args args = get_default_args();

    if (argc < 2) {
        return args;
    }

    int threads_amount;
    if ((threads_amount = atoi(argv[1])) > 0 && threads_amount <= 4) {
        args.threads_amount = threads_amount;
    }

    if (argc < 3) {
        return args;
    }

    if (strcmp(argv[2], "l") == 0) {
        args.tests_function = test_load;
    } else if (strcmp(argv[2], "s") == 0) {
        args.tests_function = test_store;
    } else if (strcmp(argv[2], "c") == 0) {
        args.tests_function = test_copy;
    }

    return args;
}

void* set_affinity_then_test(void* args) {
    int thread_id = *((int *) args);
    int core_id = *((int *) args + 1);

    cpu_set_t mask;
    CPU_ZERO(&mask);
    CPU_SET(core_id, &mask);
    sched_setaffinity(0, sizeof(mask), &mask);

    global_args.tests_function(thread_id);
}

int create_threads(pthread_t *test_threads) {
    for (int i = 0; i < global_args.threads_amount; ++i) {
        int thread_id = i, core_id = thread_id + i;
        int args[2];
        args[0] = thread_id;
        args[1] = core_id;

        if (pthread_create(test_threads + i, NULL, set_affinity_then_test, args) != 0) {
            return 1;
        }
    }

    return 0;
}

int join_threads(pthread_t *test_threads) {
    for (size_t i = 0; i < global_args.threads_amount; ++i) {
        if (pthread_join(test_threads[i], NULL) != 0) {
            return 1;
        }
    }

    return 0;
}

int main(int argc, char *argv[]) {
    global_args = parse_args(argc, argv);

    pthread_t test_threads[global_args.threads_amount];
    if (create_threads(test_threads) != 0) {
        fprintf(stderr, "Threads creation error\n");

        return 1;
    }

    if (pthread_barrier_init(&global_args.barrier, NULL, global_args.threads_amount) != 0) {
        fprintf(stderr, "Barrier initialization error\n");

        return 2;
    }

    if (join_threads(test_threads) != 0) {
        fprintf(stderr, "Threads joining error\n");

        return 3;
    }

    pthread_barrier_destroy(&global_args.barrier);
    return 0;
}
