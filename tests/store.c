#include "tests.h"

double simple_store_test(ARRAY_TYPE *arr, size_t size) {
    ARRAY_TYPE dummy = 0;

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < size; ++i) {
        arr[i] = dummy;
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double memset_store_test(ARRAY_TYPE *arr, size_t size) {
    const size_t dummy_size = 2048, mem_size = dummy_size * sizeof(ARRAY_TYPE);
    const ARRAY_TYPE value_to_fill = 228;
    ARRAY_TYPE dummy_arr[dummy_size];

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i <= size - dummy_size; i+=dummy_size) {
        memset(arr + i, value_to_fill, mem_size);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double intrinsics_store_test(ARRAY_TYPE *arr, size_t size) {
    __m256i buf0 = _mm256_setzero_si256();
    __m256i buf1 = _mm256_setzero_si256();
    __m256i buf2 = _mm256_setzero_si256();
    __m256i buf3 = _mm256_setzero_si256();
    __m256i buf4 = _mm256_setzero_si256();
    __m256i buf5 = _mm256_setzero_si256();
    __m256i buf6 = _mm256_setzero_si256();
    __m256i buf7 = _mm256_setzero_si256();
    __m256i *intrin_arr = (__m256i *) arr;
    size_t intrin_arr_size = size / (sizeof(__m256i) / sizeof(ARRAY_TYPE));

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < intrin_arr_size; i+= 8) {
        _mm256_stream_si256(intrin_arr + i, buf0);
        _mm256_stream_si256(intrin_arr + i + 1, buf1);
        _mm256_stream_si256(intrin_arr + i + 2, buf2);
        _mm256_stream_si256(intrin_arr + i + 3, buf3);
        _mm256_stream_si256(intrin_arr + i + 4, buf4);
        _mm256_stream_si256(intrin_arr + i + 5, buf5);
        _mm256_stream_si256(intrin_arr + i + 6, buf6);
        _mm256_stream_si256(intrin_arr + i + 7, buf7);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}