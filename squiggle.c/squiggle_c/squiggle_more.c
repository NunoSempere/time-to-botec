#include "squiggle.h"
#include <float.h>
#include <limits.h>
#include <math.h>
#include <omp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h> // memcpy

/* Cache optimizations */
#define CACHE_LINE_SIZE 64
// getconf LEVEL1_DCACHE_LINESIZE
// <https://stackoverflow.com/questions/794632/programmatically-get-the-cache-line-size>
typedef struct seed_cache_box_t {
    uint64_t seed;
    char padding[CACHE_LINE_SIZE - sizeof(uint64_t)];
    // Cache line size is 64 *bytes*, uint64_t is 64 *bits* (8 bytes). Different units!
} seed_cache_box;
// This avoids "false sharing", i.e., different threads competing for the same cache line
// Dealing with this shaves 4ms from a 12ms process, or a third of runtime
// <http://www.nic.uoregon.edu/~khuck/ts/acumem-report/manual_html/ch06s07.html>

/* Parallel sampler */
void sampler_parallel(double (*sampler)(uint64_t* seed), double* results, int n_threads, int n_samples)
{

    // Terms of the division:
    // a = b * quotient + reminder
    // a = b * (a/b)    + (a%b)
    // dividend: a
    // divisor: b
    // quotient = a/b
    // reminder = a%b
    // "divisor's multiple" := b*(a/b)

    // now, we have n_samples and n_threads
    // to make our life easy, each thread will have a number of samples of: a/b (quotient)
    // and we'll compute the remainder of samples separately
    // to possibly do by Jorge: improve so that the remainder is included in the threads

    int quotient = n_samples / n_threads;
    int divisor_multiple = quotient * n_threads;

    // uint64_t** seeds = malloc((size_t)n_threads * sizeof(uint64_t*));
    seed_cache_box* cache_box = (seed_cache_box*)malloc(sizeof(seed_cache_box) * (size_t)n_threads);
    // seed_cache_box cache_box[n_threads]; // we could use the C stack. On normal linux machines, it's 8MB ($ ulimit -s). However, it doesn't quite feel right.
    srand(1);
    for (int i = 0; i < n_threads; i++) {
        // Constraints:
        // - xorshift can't start with 0
        // - the seeds should be reasonably separated and not correlated
        cache_box[i].seed = (uint64_t)rand() * (UINT64_MAX / RAND_MAX);

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
            // It's possible I don't need the for, and could instead call omp
            // in some different way and get  the thread number with omp_get_thread_num()
            int lower_bound_inclusive = i * quotient;
            int upper_bound_not_inclusive = ((i + 1) * quotient); // note the < in the for loop below,

            for (int j = lower_bound_inclusive; j < upper_bound_not_inclusive; j++) {
                results[j] = sampler(&(cache_box[i].seed));
                /* 
                t starts at 0 and ends at T
                at t=0, 
                  thread i accesses: results[i*quotient +0], 
                  thread i+1 acccesses: results[(i+1)*quotient +0]
                at t=T
                  thread i accesses: results[(i+1)*quotient -1]
                  thread i+1 acccesses: results[(i+2)*quotient -1]
                The results[j] that are directly adjacent are 
                  results[(i+1)*quotient -1] (accessed by thread i at time T)
                  results[(i+1)*quotient +0] (accessed by thread i+1 at time 0)
                and these are themselves adjacent to
                  results[(i+1)*quotient -2] (accessed by thread i at time T-1)
                  results[(i+1)*quotient +1] (accessed by thread i+1 at time 2)
                If T is large enough, which it is, two threads won't access the same
                cache line at the same time.
                Pictorially:
                at t=0 ....i.........I.........
                at t=T .............i.........I
                and the two never overlap
                Note that results[j] is a double, a double has 8 bytes (64 bits)
                8 doubles fill a cache line of 64 bytes.
                So we specifically won't get problems as long as n_samples/n_threads > 8
                n_threads is normally 16, so n_samples > 128 
                Note also that this is only a problem in terms of speed, if n_samples<128
                the results are still computed, it'll just be slower
                */
            }
        }
    }
    for (int j = divisor_multiple; j < n_samples; j++) {
        results[j] = sampler(&(cache_box[0].seed));
        // we can just reuse a seed,
        // this isn't problematic because we;ve now stopped doing multithreading
    }

    free(cache_box);
}

/* Get confidence intervals, given a sampler */

typedef struct ci_t {
    double low;
    double high;
} ci;

inline static void swp(int i, int j, double xs[])
{
    double tmp = xs[i];
    xs[i] = xs[j];
    xs[j] = tmp;
}

static int partition(int low, int high, double xs[], int length)
{
    if (low > high || high >= length) {
        printf("Invariant violated for function partition in %s (%d)", __FILE__, __LINE__);
        exit(1);
    }
    // Note: the scratchpad/ folder in commit 578bfa27 has printfs sprinkled throughout
    int pivot = low + (int)floor((high - low) / 2);
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

    double* ys = malloc((size_t)n * sizeof(double));
    memcpy(ys, xs, (size_t)n * sizeof(double));
    // ^: don't rearrange item order in the original array

    int low = 0;
    int high = n - 1;
    for (;;) {
        if (low == high) {
            double result = ys[low];
            free(ys);
            return result;
        }
        int pivot = partition(low, high, ys, n);
        if (pivot == k) {
            double result = ys[pivot];
            free(ys);
            return result;
        } else if (k < pivot) {
            high = pivot - 1;
        } else {
            low = pivot + 1;
        }
    }
}

ci array_get_ci(ci interval, double* xs, int n)
{

    int low_k = (int)floor(interval.low * n);
    int high_k = (int)ceil(interval.high * n);
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

double array_get_median(double xs[], int n)
{
    int median_k = (int)floor(0.5 * n);
    return quickselect(median_k, xs, n);
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

void array_print_stats(double xs[], int n)
{
    ci ci_90 = array_get_ci((ci) { .low = 0.05, .high = 0.95 }, xs, n);
    ci ci_80 = array_get_ci((ci) { .low = 0.1, .high = 0.9 }, xs, n);
    ci ci_50 = array_get_ci((ci) { .low = 0.25, .high = 0.75 }, xs, n);
    double median = array_get_median(xs, n);
    double mean = array_mean(xs, n);
    double std = array_std(xs, n);
    printf("| Statistic | Value |\n"
           "| --- | --- |\n"
           "| Mean   | %lf |\n"
           "| Median | %lf |\n"
           "| Std    | %lf |\n"
           "| 90%% confidence interval | %lf to %lf |\n"
           "| 80%% confidence interval | %lf to %lf |\n"
           "| 50%% confidence interval | %lf to %lf |\n",
           mean, median, std, ci_90.low, ci_90.high, ci_80.low, ci_80.high, ci_50.low, ci_50.high);
}

void array_print_histogram(double* xs, int n_samples, int n_bins)
{
    // Interface inspired by <https://github.com/red-data-tools/YouPlot>
    if (n_bins <= 1) {
        fprintf(stderr, "Number of bins must be greater than 1.\n");
        return;
    } else if (n_samples <= 1) {
        fprintf(stderr, "Number of samples must be higher than 1.\n");
        return;
    }

    int* bins = (int*)calloc((size_t)n_bins, sizeof(int));
    if (bins == NULL) {
        fprintf(stderr, "Memory allocation for bins failed.\n");
        return;
    }

    // Find the minimum and maximum values from the samples
    double min_value = xs[0], max_value = xs[0];
    for (int i = 0; i < n_samples; i++) {
        if (xs[i] < min_value) {
            min_value = xs[i];
        }
        if (xs[i] > max_value) {
            max_value = xs[i];
        }
    }

    // Avoid division by zero for a single unique value
    if (min_value == max_value) {
        max_value++;
    }

    // Calculate bin width
    double bin_width = (max_value - min_value) / n_bins;

    // Fill the bins with sample counts
    for (int i = 0; i < n_samples; i++) {
        int bin_index = (int)((xs[i] - min_value) / bin_width);
        if (bin_index == n_bins) {
            bin_index--; // Last bin includes max_value
        }
        bins[bin_index]++;
    }

    // Calculate the scaling factor based on the maximum bin count
    int max_bin_count = 0;
    for (int i = 0; i < n_bins; i++) {
        if (bins[i] > max_bin_count) {
            max_bin_count = bins[i];
        }
    }
    const int MAX_WIDTH = 50; // Adjust this to your terminal width
    double scale = max_bin_count > MAX_WIDTH ? (double)MAX_WIDTH / max_bin_count : 1.0;

    // Print the histogram
    for (int i = 0; i < n_bins; i++) {
        double bin_start = min_value + i * bin_width;
        double bin_end = bin_start + bin_width;

        int decimalPlaces = 1;
        if ((0 < bin_width) && (bin_width < 1)) {
            int magnitude = (int)floor(log10(bin_width));
            decimalPlaces = -magnitude;
            decimalPlaces = decimalPlaces > 10 ? 10 : decimalPlaces;
        }
        printf("[%*.*f, %*.*f", 4 + decimalPlaces, decimalPlaces, bin_start, 4 + decimalPlaces, decimalPlaces, bin_end);
        char interval_delimiter = ')';
        if (i == (n_bins - 1)) {
            interval_delimiter = ']'; // last bucket is inclusive
        }
        printf("%c: ", interval_delimiter);

        int marks = (int)(bins[i] * scale);
        for (int j = 0; j < marks; j++) {
            printf("█");
        }
        printf(" %d\n", bins[i]);
    }

    // Free the allocated memory for bins
    free(bins);
}

void array_print_90_ci_histogram(double* xs, int n_samples, int n_bins)
{
    // Code duplicated from previous function
    // I'll consider simplifying it at some future point
    // Possible ideas:
    // - having only one function that takes any confidence interval?
    // - having a utility function that is called by both functions?
    ci ci_90 = array_get_90_ci(xs, n_samples);

    if (n_bins <= 1) {
        fprintf(stderr, "Number of bins must be greater than 1.\n");
        return;
    } else if (n_samples <= 10) {
        fprintf(stderr, "Number of samples must be higher than 10.\n");
        return;
    }

    int* bins = (int*)calloc((size_t)n_bins, sizeof(int));
    if (bins == NULL) {
        fprintf(stderr, "Memory allocation for bins failed.\n");
        return;
    }

    double min_value = ci_90.low, max_value = ci_90.high;

    // Avoid division by zero for a single unique value
    if (min_value == max_value) {
        max_value++;
    }
    double bin_width = (max_value - min_value) / n_bins;

    // Fill the bins with sample counts
    int below_min = 0, above_max = 0;
    for (int i = 0; i < n_samples; i++) {
        if (xs[i] < min_value) {
            below_min++;
        } else if (xs[i] > max_value) {
            above_max++;
        } else {
            int bin_index = (int)((xs[i] - min_value) / bin_width);
            if (bin_index == n_bins) {
                bin_index--; // Last bin includes max_value
            }
            bins[bin_index]++;
        }
    }

    // Calculate the scaling factor based on the maximum bin count
    int max_bin_count = 0;
    for (int i = 0; i < n_bins; i++) {
        if (bins[i] > max_bin_count) {
            max_bin_count = bins[i];
        }
    }
    const int MAX_WIDTH = 40; // Adjust this to your terminal width
    double scale = max_bin_count > MAX_WIDTH ? (double)MAX_WIDTH / max_bin_count : 1.0;

    // Print the histogram
    int decimalPlaces = 1;
    if ((0 < bin_width) && (bin_width < 1)) {
        int magnitude = (int)floor(log10(bin_width));
        decimalPlaces = -magnitude;
        decimalPlaces = decimalPlaces > 10 ? 10 : decimalPlaces;
    }
    printf("(%*s, %*.*f): ", 6 + decimalPlaces, "-∞", 4 + decimalPlaces, decimalPlaces, min_value);
    int marks_below_min = (int)(below_min * scale);
    for (int j = 0; j < marks_below_min; j++) {
        printf("█");
    }
    printf(" %d\n", below_min);
    for (int i = 0; i < n_bins; i++) {
        double bin_start = min_value + i * bin_width;
        double bin_end = bin_start + bin_width;

        printf("[%*.*f, %*.*f", 4 + decimalPlaces, decimalPlaces, bin_start, 4 + decimalPlaces, decimalPlaces, bin_end);
        char interval_delimiter = ')';
        if (i == (n_bins - 1)) {
            interval_delimiter = ']'; // last bucket is inclusive
        }
        printf("%c: ", interval_delimiter);

        int marks = (int)(bins[i] * scale);
        for (int j = 0; j < marks; j++) {
            printf("█");
        }
        printf(" %d\n", bins[i]);
    }
    printf("(%*.*f, %*s): ", 4 + decimalPlaces, decimalPlaces, max_value, 6 + decimalPlaces, "+∞");
    int marks_above_max = (int)(above_max * scale);
    for (int j = 0; j < marks_above_max; j++) {
        printf("█");
    }
    printf(" %d\n", above_max);

    // Free the allocated memory for bins
    free(bins);
}

/* Algebra manipulations */

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
    double loghigh = log(x.high);
    double loglow = log(x.low);
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
