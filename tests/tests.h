#ifndef LAB4_TESTS_H
#define LAB4_TESTS_H

#include <immintrin.h>
#include <stddef.h>
#include <string.h>
#include <time.h>

//0.25 Gbyte
#define ARRAY_SIZE 268435456 / sizeof(ARRAY_TYPE)

typedef int ARRAY_TYPE;

//load tests
double simple_load_test(const ARRAY_TYPE *, size_t);

double memcpy_load_test(const ARRAY_TYPE *, size_t);

double intrinsics_load_test(const ARRAY_TYPE *, size_t);

//store tests
double simple_store_test(ARRAY_TYPE *, size_t);

double memset_store_test(ARRAY_TYPE *, size_t);

double intrinsics_store_test(ARRAY_TYPE *, size_t);

//store tests
double simple_copy_test(const ARRAY_TYPE *, ARRAY_TYPE *,  size_t);

double memcpy_copy_test(const ARRAY_TYPE *, ARRAY_TYPE *, size_t);

double intrinsics_copy_test(const ARRAY_TYPE *, ARRAY_TYPE *, size_t);

double get_duration_from_timespecs(struct timespec, struct timespec);

#endif
