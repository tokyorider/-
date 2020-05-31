#include "tests/tests.h"

double get_duration_from_timespecs(const struct timespec begin, const struct timespec end) {
    return end.tv_sec - begin.tv_sec + (double) (end.tv_nsec - begin.tv_nsec) / 1e9;
}
