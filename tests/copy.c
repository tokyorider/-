#include "tests.h"

double simple_copy_test(const ARRAY_TYPE *in, ARRAY_TYPE *out, size_t size) {
    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < size; ++i) {
        out[i] = in[i];
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double memcpy_copy_test(const ARRAY_TYPE *in, ARRAY_TYPE *out, size_t size) {
    const size_t dummy_size = 2048,  mem_size = dummy_size * sizeof(ARRAY_TYPE);

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < size; i+= dummy_size) {
        memcpy(out + i, in + i, mem_size);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}

double intrinsics_copy_test(const ARRAY_TYPE *in, ARRAY_TYPE *out, size_t size) {
    __m256i buf0, buf1, buf2, buf3, buf4, buf5, buf6, buf7;
    __m256i *intrin_in = (__m256i *) in, *intrin_out = (__m256i *) out;
    size_t intrin_arr_size = size / (sizeof(__m256i) / sizeof(ARRAY_TYPE));

    struct timespec begin, end;
    clock_gettime(CLOCK_MONOTONIC_RAW, &begin);
    for (size_t i = 0; i < intrin_arr_size; i+= 8) {
        _mm256_stream_si256(intrin_out + i, intrin_in[i]);
        _mm256_stream_si256(intrin_out + i + 1, intrin_in[i + 1]);
        _mm256_stream_si256(intrin_out + i + 2, intrin_in[i + 2]);
        _mm256_stream_si256(intrin_out + i + 3, intrin_in[i + 3]);
        _mm256_stream_si256(intrin_out + i + 4, intrin_in[i + 4]);
        _mm256_stream_si256(intrin_out + i + 5, intrin_in[i + 5]);
        _mm256_stream_si256(intrin_out + i + 6, intrin_in[i + 6]);
        _mm256_stream_si256(intrin_out + i + 7, intrin_in[i + 7]);
    }
    clock_gettime(CLOCK_MONOTONIC_RAW, &end);

    return get_duration_from_timespecs(begin, end);
}