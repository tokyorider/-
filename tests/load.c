#include "tests.h"

double simple_load_test(const ARRAY_TYPE *arr, size_t size) {
    ARRAY_TYPE dummy;

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < size; ++i) {
        dummy = arr[i];
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double memcpy_load_test(const ARRAY_TYPE *arr, size_t size) {
    const size_t dummy_size = 2048,  mem_size = dummy_size * sizeof(ARRAY_TYPE);
    ARRAY_TYPE dummy_arr[dummy_size];

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < size; i+= dummy_size) {
        memcpy(dummy_arr, arr + i, mem_size);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double intrinsics_load_test(const ARRAY_TYPE *arr, size_t size) {
    __m256i buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7;
    __m256i *intrin_arr = (__m256i *) arr;
    size_t intrin_arr_size = size / (sizeof(__m256i) / sizeof(ARRAY_TYPE));

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < intrin_arr_size; i+= 8) {
        buf0 = _mm256_stream_load_si256(intrin_arr + i);
        buf1 = _mm256_stream_load_si256(intrin_arr + i + 1);
        buf2 = _mm256_stream_load_si256(intrin_arr + i + 2);
        buf3 = _mm256_stream_load_si256(intrin_arr + i + 3);
        buf4 = _mm256_stream_load_si256(intrin_arr + i + 4);
        buf5 = _mm256_stream_load_si256(intrin_arr + i + 5);
        buf6 = _mm256_stream_load_si256(intrin_arr + i + 6);
        buf7 = _mm256_stream_load_si256(intrin_arr + i + 7);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

