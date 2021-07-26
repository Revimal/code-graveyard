#include <stdio.h>
#include <stdlib.h>

#include "cbench-x86/benchmark_toolkit.h"
#include "../rmspace.c"

BENCHMARK_HELPER_ALIGN static const char benchmark_data_sample[] =
#include "sample.txt"
;

#define BENCHMARK_DATA_SIZE sizeof(benchmark_data_sample)

size_t rmspace_raw(char *input, size_t bufsz) {
    size_t pos = 0;
    size_t idx;

    for(idx = 0; idx < bufsz; ++idx) {
        if (input[idx] == '\t' || input[idx] == ' ' || input[idx] == '\n' || input[idx] == '\r')
            continue;
        input[pos++] = input[idx];
    }

    return pos;
}

int main(BENCHMARK_HELPER_UNUSED int argc, BENCHMARK_HELPER_UNUSED char *argv[]) {
    BENCHMARK_HELPER_ALIGN char *benchmark_data_copied;
    BENCHMARK_HELPER_ALIGN char *benchmark_test_raw;
    BENCHMARK_HELPER_ALIGN char *benchmark_test_acc;
    BENCHMARK_HELPER_ALIGN uint64_t benchmark_result_raw;
    BENCHMARK_HELPER_ALIGN uint64_t benchmark_result_acc;
    BENCHMARK_HELPER_ALIGN size_t benchmark_variable_idx;

    if (!BENCHMARK_COUNTER_TRUSTABLE())
        return ENOSYS;

    benchmark_data_copied = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE, BENCHMARK_DATA_SIZE);
    benchmark_test_raw = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE, BENCHMARK_DATA_SIZE);
    benchmark_test_acc = aligned_alloc(BENCHMARK_DEFINE_PAGESIZE, BENCHMARK_DATA_SIZE);
    if (!benchmark_data_copied || !benchmark_test_raw || !benchmark_test_acc) {
        if (benchmark_data_copied)
            free(benchmark_data_copied);
        if (benchmark_test_raw)
            free(benchmark_test_raw);
        if (benchmark_test_acc)
            free(benchmark_test_acc);

        return ENOMEM;
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx)
        benchmark_data_copied[benchmark_variable_idx] = benchmark_data_sample[benchmark_variable_idx];


    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx)
        benchmark_test_raw[benchmark_variable_idx] = benchmark_data_copied[benchmark_variable_idx];

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx) {
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_data_copied[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_test_raw[benchmark_variable_idx]);
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx) {
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_data_copied[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_test_raw[benchmark_variable_idx]);
    }

    BENCHMARK_TEMPLATE_PREPARE();
    BENCHMARK_TEMPLATE_TESTCASE(benchmark_result_raw, {
        rmspace_raw(benchmark_test_raw, BENCHMARK_DATA_SIZE);
    });

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx)
        benchmark_test_acc[benchmark_variable_idx] = benchmark_data_copied[benchmark_variable_idx];

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx) {
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_data_copied[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_PREFAULT(&benchmark_test_raw[benchmark_variable_idx]);
    }

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx) {
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_data_copied[benchmark_variable_idx]);
        BENCHMARK_BENCHTOOL_CLFLUSH(&benchmark_test_raw[benchmark_variable_idx]);
    }

    BENCHMARK_TEMPLATE_PREPARE();
    BENCHMARK_TEMPLATE_TESTCASE(benchmark_result_acc, {
        rmspace_all(benchmark_test_acc, BENCHMARK_DATA_SIZE);
    });

    for (benchmark_variable_idx = 0; benchmark_variable_idx < BENCHMARK_DATA_SIZE; ++benchmark_variable_idx)
        if (benchmark_test_raw[benchmark_variable_idx] != benchmark_test_acc[benchmark_variable_idx]) {
            free(benchmark_data_copied);
            free(benchmark_test_raw);
            free(benchmark_test_acc);
            return EINVAL;
        }

    fprintf(stdout, "[RAW]: CYCLE=%020"PRIu64"\n", benchmark_result_raw);
    fprintf(stdout, "[ACC]: CYCLE=%020"PRIu64"\n", benchmark_result_acc);

    free(benchmark_data_copied);
    free(benchmark_test_raw);
    free(benchmark_test_acc);

    return 0;
}