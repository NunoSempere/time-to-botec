#include "squiggle.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

/* Parallel sampler */
void sampler_parallel(double (*sampler)(uint64_t* seed), double* results, int n_threads, int n_samples)
{

    // Division terminology:
    // a =  b * quotient + reminder
    // a = (a/b)*b + (a%b)
    // dividend: a
    // divisor: b
    // quotient = a / b
    // reminder = a % b
    // "divisor's multiple" := (a/b)*b

    // now, we have n_samples and n_threads
    // to make our life easy, each thread will have a number of samples of: a/b (quotient)
    // and we'll compute the remainder of samples separately
    // to possibly do by Jorge: improve so that the remainder is included in the threads

    int quotient = n_samples / n_threads;
    /* int remainder = n_samples % n_threads; // not used, comment to avoid lint warning */
    int divisor_multiple = quotient * n_threads;

    uint64_t** seeds = malloc(n_threads * sizeof(uint64_t*));
    // printf("UINT64_MAX: %lu\n", UINT64_MAX);
    srand(1);
    for (uint64_t i = 0; i < n_threads; i++) {
        seeds[i] = malloc(sizeof(uint64_t));
        // Constraints:
        // - xorshift can't start with 0
        // - the seeds should be reasonably separated and not correlated
        *seeds[i] = (uint64_t)rand() * (UINT64_MAX / RAND_MAX);
        // printf("#%ld: %lu\n",i, *seeds[i]);

        // Other initializations tried:
        // *seeds[i] = 1 + i;
        // *seeds[i] = (i + 0.5)*(UINT64_MAX/n_threads);
        // *seeds[i] = (i + 0.5)*(UINT64_MAX/n_threads) + constant * i;
    }

    int i;
#pragma omp parallel private(i)
    {
#pragma omp for
        for (i = 0; i < n_threads; i++) {
            int lower_bound_inclusive = i * quotient;
            int upper_bound_not_inclusive = ((i + 1) * quotient); // note the < in the for loop below,
            // printf("Lower bound: %d, upper bound: %d\n", lower_bound, upper_bound);
            for (int j = lower_bound_inclusive; j < upper_bound_not_inclusive; j++) {
                results[j] = sampler(seeds[i]);
            }
        }
    }
    for (int j = divisor_multiple; j < n_samples; j++) {
        results[j] = sampler(seeds[0]);
        // we can just reuse a seed, this isn't problematic because we are not doing multithreading
    }

    for (uint64_t i = 0; i < n_threads; i++) {
        free(seeds[i]);
    }
    free(seeds);
}

/* Get confidence intervals, given a sampler */
// Not in core yet because I'm not sure how much I like the struct
// and the built-in 100k samples
// to do: add n to function parameters and document

typedef struct ci_t {
    double low;
    double high;
} ci;

static void swp(int i, int j, double xs[])
{
    double tmp = xs[i];
    xs[i] = xs[j];
    xs[j] = tmp;
}

static int partition(int low, int high, double xs[], int length)
{
    // To understand this function:
    // - see the note after gt variable definition
    // - go to commit 578bfa27 and the scratchpad/ folder in it, which has printfs sprinkled throughout
    int pivot = low + floor((high - low) / 2);
    double pivot_value = xs[pivot];
    swp(pivot, high, xs);
    int gt = low; /* This pointer will iterate until finding an element which is greater than the pivot. Then it will move elements that are smaller before it--more specifically, it will move elements to its position and then increment. As a result all elements between gt and i will be greater than the pivot. */
    for (int i = low; i < high; i++) {
        if (xs[i] < pivot_value) {
            swp(gt, i, xs);
            gt++;
        }
    }
    swp(high, gt, xs);
    return gt;
}

static double quickselect(int k, double xs[], int n)
{
    // https://en.wikipedia.org/wiki/Quickselect
    int low = 0;
    int high = n - 1;
    for (;;) {
        if (low == high) {
            return xs[low];
        }
        int pivot = partition(low, high, xs, n);
        if (pivot == k) {
            return xs[pivot];
        } else if (k < pivot) {
            high = pivot - 1;
        } else {
            low = pivot + 1;
        }
    }
}

ci array_get_ci(ci interval, double* xs, int n)
{

    int low_k = floor(interval.low * n);
    int high_k = ceil(interval.high * n);
    ci result = {
        .low = quickselect(low_k, xs, n),
        .high = quickselect(high_k, xs, n),
    };
    return result;
}
ci array_get_90_ci(double xs[], int n)
{
    return array_get_ci((ci) { .low = 0.05, .high = 0.95 }, xs, n);
}

ci sampler_get_ci(ci interval, double (*sampler)(uint64_t*), int n, uint64_t* seed)
{
    double* xs = malloc(n * sizeof(double));
    /*for (int i = 0; i < n; i++) {
        xs[i] = sampler(seed);
    }*/
    sampler_parallel(sampler, xs, 16, n);
    ci result = array_get_ci(interval, xs, n);
    free(xs);
    return result;
}
ci sampler_get_90_ci(double (*sampler)(uint64_t*), int n, uint64_t* seed)
{
    return sampler_get_ci((ci) { .low = 0.05, .high = 0.95 }, sampler, n, seed);
}

/* Algebra manipulations */
// here I discover named structs,
// which mean that I don't have to be typing
// struct blah all the time.

#define NORMAL90CONFIDENCE 1.6448536269514727

typedef struct normal_params_t {
    double mean;
    double std;
} normal_params;

normal_params algebra_sum_normals(normal_params a, normal_params b)
{
    normal_params result = {
        .mean = a.mean + b.mean,
        .std = sqrt((a.std * a.std) + (b.std * b.std)),
    };
    return result;
}

typedef struct lognormal_params_t {
    double logmean;
    double logstd;
} lognormal_params;

lognormal_params algebra_product_lognormals(lognormal_params a, lognormal_params b)
{
    lognormal_params result = {
        .logmean = a.logmean + b.logmean,
        .logstd = sqrt((a.logstd * a.logstd) + (b.logstd * b.logstd)),
    };
    return result;
}

lognormal_params convert_ci_to_lognormal_params(ci x)
{
    double loghigh = logf(x.high);
    double loglow = logf(x.low);
    double logmean = (loghigh + loglow) / 2.0;
    double logstd = (loghigh - loglow) / (2.0 * NORMAL90CONFIDENCE);
    lognormal_params result = { .logmean = logmean, .logstd = logstd };
    return result;
}

ci convert_lognormal_params_to_ci(lognormal_params y)
{
    double h = y.logstd * NORMAL90CONFIDENCE;
    double loghigh = y.logmean + h;
    double loglow = y.logmean - h;
    ci result = { .low = exp(loglow), .high = exp(loghigh) };
    return result;
}

/* Scaffolding to handle errors */
// We will sample from an arbitrary cdf
// and that operation might fail
// so we build some scaffolding here

#define MAX_ERROR_LENGTH 500
#define EXIT_ON_ERROR 0
#define PROCESS_ERROR(error_msg) process_error(error_msg, EXIT_ON_ERROR, __FILE__, __LINE__)

struct box {
    int empty;
    double content;
    char* error_msg;
};

struct box process_error(const char* error_msg, int should_exit, char* file, int line)
{
    if (should_exit) {
        printf("@, in %s (%d)", file, line);
        exit(1);
    } else {
        char error_msg[MAX_ERROR_LENGTH];
        snprintf(error_msg, MAX_ERROR_LENGTH, "@, in %s (%d)", file, line); // NOLINT: We are being carefull here by considering MAX_ERROR_LENGTH explicitly.
        struct box error = { .empty = 1, .error_msg = error_msg };
        return error;
    }
}

/* Invert an arbitrary cdf at a point */
// Version #1:
// - input: (cdf: double => double, p)
// - output: Box(number|error)
struct box inverse_cdf_double(double cdf(double), double p)
{
    // given a cdf: [-Inf, Inf] => [0,1]
    // returns a box with either
    // x such that cdf(x) = p
    // or an error
    // if EXIT_ON_ERROR is set to 1, it exits instead of providing an error

    double low = -1.0;
    double high = 1.0;

    // 1. Make sure that cdf(low) < p < cdf(high)
    int interval_found = 0;
    while ((!interval_found) && (low > -FLT_MAX / 4) && (high < FLT_MAX / 4)) {
        // ^ Using FLT_MIN and FLT_MAX is overkill
        // but it's also the *correct* thing to do.

        int low_condition = (cdf(low) < p);
        int high_condition = (p < cdf(high));
        if (low_condition && high_condition) {
            interval_found = 1;
        } else if (!low_condition) {
            low = low * 2;
        } else if (!high_condition) {
            high = high * 2;
        }
    }

    if (!interval_found) {
        return PROCESS_ERROR("Interval containing the target value not found, in function inverse_cdf");
    } else {

        int convergence_condition = 0;
        int count = 0;
        while (!convergence_condition && (count < (INT_MAX / 2))) {
            double mid = (high + low) / 2;
            int mid_not_new = (mid == low) || (mid == high);
            // double width = high - low;
            // if ((width < 1e-8) || mid_not_new){
            if (mid_not_new) {
                convergence_condition = 1;
            } else {
                double mid_sign = cdf(mid) - p;
                if (mid_sign < 0) {
                    low = mid;
                } else if (mid_sign > 0) {
                    high = mid;
                } else if (mid_sign == 0) {
                    low = mid;
                    high = mid;
                }
            }
        }

        if (convergence_condition) {
            struct box result = { .empty = 0, .content = low };
            return result;
        } else {
            return PROCESS_ERROR("Search process did not converge, in function inverse_cdf");
        }
    }
}

// Version #2:
// - input: (cdf: double => Box(number|error), p)
// - output: Box(number|error)
struct box inverse_cdf_box(struct box cdf_box(double), double p)
{
    // given a cdf: [-Inf, Inf] => Box([0,1])
    // returns a box with either
    // x such that cdf(x) = p
    // or an error
    // if EXIT_ON_ERROR is set to 1, it exits instead of providing an error

    double low = -1.0;
    double high = 1.0;

    // 1. Make sure that cdf(low) < p < cdf(high)
    int interval_found = 0;
    while ((!interval_found) && (low > -FLT_MAX / 4) && (high < FLT_MAX / 4)) {
        // ^ Using FLT_MIN and FLT_MAX is overkill
        // but it's also the *correct* thing to do.
        struct box cdf_low = cdf_box(low);
        if (cdf_low.empty) {
            return PROCESS_ERROR(cdf_low.error_msg);
        }

        struct box cdf_high = cdf_box(high);
        if (cdf_high.empty) {
            return PROCESS_ERROR(cdf_low.error_msg);
        }

        int low_condition = (cdf_low.content < p);
        int high_condition = (p < cdf_high.content);
        if (low_condition && high_condition) {
            interval_found = 1;
        } else if (!low_condition) {
            low = low * 2;
        } else if (!high_condition) {
            high = high * 2;
        }
    }

    if (!interval_found) {
        return PROCESS_ERROR("Interval containing the target value not found, in function inverse_cdf");
    } else {

        int convergence_condition = 0;
        int count = 0;
        while (!convergence_condition && (count < (INT_MAX / 2))) {
            double mid = (high + low) / 2;
            int mid_not_new = (mid == low) || (mid == high);
            // double width = high - low;
            if (mid_not_new) {
                // if ((width < 1e-8) || mid_not_new){
                convergence_condition = 1;
            } else {
                struct box cdf_mid = cdf_box(mid);
                if (cdf_mid.empty) {
                    return PROCESS_ERROR(cdf_mid.error_msg);
                }
                double mid_sign = cdf_mid.content - p;
                if (mid_sign < 0) {
                    low = mid;
                } else if (mid_sign > 0) {
                    high = mid;
                } else if (mid_sign == 0) {
                    low = mid;
                    high = mid;
                }
            }
        }

        if (convergence_condition) {
            struct box result = { .empty = 0, .content = low };
            return result;
        } else {
            return PROCESS_ERROR("Search process did not converge, in function inverse_cdf");
        }
    }
}

/* Sample from an arbitrary cdf */
// Before: invert an arbitrary cdf at a point
// Now: from an arbitrary cdf, get a sample
struct box sampler_cdf_box(struct box cdf(double), uint64_t* seed)
{
    double p = sample_unit_uniform(seed);
    struct box result = inverse_cdf_box(cdf, p);
    return result;
}
struct box sampler_cdf_double(double cdf(double), uint64_t* seed)
{
    double p = sample_unit_uniform(seed);
    struct box result = inverse_cdf_double(cdf, p);
    return result;
}
double sampler_cdf_danger(struct box cdf(double), uint64_t* seed)
{
    double p = sample_unit_uniform(seed);
    struct box result = inverse_cdf_box(cdf, p);
    if (result.empty) {
        exit(1);
    } else {
        return result.content;
    }
}

/* array print: potentially useful for debugging */

void array_print(double xs[], int n)
{
    printf("[");
    for (int i = 0; i < n - 1; i++) {
        printf("%f, ", xs[i]);
    }
    printf("%f", xs[n - 1]);
    printf("]\n");
}
