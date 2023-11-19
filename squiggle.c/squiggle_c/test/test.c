#include "../squiggle.h"
#include <math.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define TOLERANCE 5.0 / 1000.0
#define MAX_NAME_LENGTH 500

// Structs

struct array_expectations {
    double* array;
    int n;
    char* name;
    double expected_mean;
    double expected_std;
    double tolerance;
};

void test_array_expectations(struct array_expectations e)
{
    double mean = array_mean(e.array, e.n);
    double delta_mean = mean - e.expected_mean;

    double std = array_std(e.array, e.n);
    double delta_std = std - e.expected_std;

    if ((fabs(delta_mean) / fabs(mean) > e.tolerance) && (fabs(delta_mean) > e.tolerance)) {
        printf("[-] Mean test for %s NOT passed.\n", e.name);
        printf("Mean of %s: %f, vs expected mean: %f\n", e.name, mean, e.expected_mean);
        printf("delta: %f, relative delta: %f\n", delta_mean, delta_mean / fabs(mean));
    } else {
        printf("[x] Mean test for %s PASSED\n", e.name);
    }

    if ((fabs(delta_std) / fabs(std) > e.tolerance) && (fabs(delta_std) > e.tolerance)) {
        printf("[-] Std test for %s NOT passed.\n", e.name);
        printf("Std of %s: %f, vs expected std: %f\n", e.name, std, e.expected_std);
        printf("delta: %f, relative delta: %f\n", delta_std, delta_std / fabs(std));
    } else {
        printf("[x] Std test for %s PASSED\n", e.name);
    }

    printf("\n");
}

// Test unit uniform
void test_unit_uniform(uint64_t* seed)
{
    int n = 1000 * 1000;
    double* unit_uniform_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        unit_uniform_array[i] = sample_unit_uniform(seed);
    }

    struct array_expectations expectations = {
        .array = unit_uniform_array,
        .n = n,
        .name = "unit uniform",
        .expected_mean = 0.5,
        .expected_std = sqrt(1.0 / 12.0),
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(unit_uniform_array);
}

// Test uniforms
void test_uniform(double start, double end, uint64_t* seed)
{
    int n = 1000 * 1000;
    double* uniform_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        uniform_array[i] = sample_uniform(start, end, seed);
    }

    char* name = malloc(MAX_NAME_LENGTH * sizeof(char));
    snprintf(name, MAX_NAME_LENGTH, "[%f, %f] uniform", start, end);
    struct array_expectations expectations = {
        .array = uniform_array,
        .n = n,
        .name = name,
        .expected_mean = (start + end) / 2,
        .expected_std = sqrt(1.0 / 12.0) * fabs(end - start),
        .tolerance = fabs(end - start) * TOLERANCE,
    };

    test_array_expectations(expectations);
    free(name);
    free(uniform_array);
}

// Test unit normal
void test_unit_normal(uint64_t* seed)
{
    int n = 1000 * 1000;
    double* unit_normal_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        unit_normal_array[i] = sample_unit_normal(seed);
    }

    struct array_expectations expectations = {
        .array = unit_normal_array,
        .n = n,
        .name = "unit normal",
        .expected_mean = 0,
        .expected_std = 1,
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(unit_normal_array);
}

// Test normal
void test_normal(double mean, double std, uint64_t* seed)
{
    int n = 10 * 1000 * 1000;
    double* normal_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        normal_array[i] = sample_normal(mean, std, seed);
    }

    char* name = malloc(MAX_NAME_LENGTH * sizeof(char));
    snprintf(name, MAX_NAME_LENGTH, "normal(%f, %f)", mean, std);
    struct array_expectations expectations = {
        .array = normal_array,
        .n = n,
        .name = name,
        .expected_mean = mean,
        .expected_std = std,
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(name);
    free(normal_array);
}

// Test lognormal
void test_lognormal(double logmean, double logstd, uint64_t* seed)
{
    int n = 10 * 1000 * 1000;
    double* lognormal_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        lognormal_array[i] = sample_lognormal(logmean, logstd, seed);
    }

    char* name = malloc(MAX_NAME_LENGTH * sizeof(char));
    snprintf(name, MAX_NAME_LENGTH, "lognormal(%f, %f)", logmean, logstd);
    struct array_expectations expectations = {
        .array = lognormal_array,
        .n = n,
        .name = name,
        .expected_mean = exp(logmean + pow(logstd, 2) / 2),
        .expected_std = sqrt((exp(pow(logstd, 2)) - 1) * exp(2 * logmean + pow(logstd, 2))),
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(name);
    free(lognormal_array);
}

// Test lognormal to
void test_to(double low, double high, uint64_t* seed)
{
    int n = 10 * 1000 * 1000;
    double* lognormal_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        lognormal_array[i] = sample_to(low, high, seed);
    }


    char* name = malloc(MAX_NAME_LENGTH * sizeof(char));
    snprintf(name, MAX_NAME_LENGTH, "to(%f, %f)", low, high);
    
		const double NORMAL95CONFIDENCE = 1.6448536269514722;
    double loglow = logf(low);
    double loghigh = logf(high);
    double logmean = (loglow + loghigh) / 2;
    double logstd = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
    
		struct array_expectations expectations = {
        .array = lognormal_array,
        .n = n,
        .name = name,
        .expected_mean = exp(logmean + pow(logstd, 2) / 2),
        .expected_std = sqrt((exp(pow(logstd, 2)) - 1) * exp(2 * logmean + pow(logstd, 2))),
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(name);
    free(lognormal_array);
}

// Test beta

void test_beta(double a, double b, uint64_t* seed)
{
    int n = 10 * 1000 * 1000;
    double* beta_array = malloc(sizeof(double) * n);

    for (int i = 0; i < n; i++) {
        beta_array[i] = sample_beta(a, b, seed);
    }

    char* name = malloc(MAX_NAME_LENGTH * sizeof(char));
    snprintf(name, MAX_NAME_LENGTH, "beta(%f, %f)", a, b);
    struct array_expectations expectations = {
        .array = beta_array,
        .n = n,
        .name = name,
        .expected_mean = a / (a + b),
        .expected_std = sqrt((a * b) / (pow(a + b, 2) * (a + b + 1))),
        .tolerance = TOLERANCE,
    };

    test_array_expectations(expectations);
    free(name);
}

int main()
{
    // set randomness seed
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with a seed of 0

    printf("Testing unit uniform\n");
    test_unit_uniform(seed);

    printf("Testing small uniforms\n");
    for (int i = 0; i < 100; i++) {
        double start = sample_uniform(-10, 10, seed);
        double end = sample_uniform(-10, 10, seed);
        if (end > start) {
            test_uniform(start, end, seed);
        }
    }

    printf("Testing wide uniforms\n");
    for (int i = 0; i < 100; i++) {
        double start = sample_uniform(-1000 * 1000, 1000 * 1000, seed);
        double end = sample_uniform(-1000 * 1000, 1000 * 1000, seed);
        if (end > start) {
            test_uniform(start, end, seed);
        }
    }

    printf("Testing unit normal\n");
    test_unit_normal(seed);

    printf("Testing small normals\n");
    for (int i = 0; i < 100; i++) {
        double mean = sample_uniform(-10, 10, seed);
        double std = sample_uniform(0, 10, seed);
        if (std > 0) {
            test_normal(mean, std, seed);
        }
    }

    printf("Testing larger normals\n");
    for (int i = 0; i < 100; i++) {
        double mean = sample_uniform(-1000 * 1000, 1000 * 1000, seed);
        double std = sample_uniform(0, 1000 * 1000, seed);
        if (std > 0) {
            test_normal(mean, std, seed);
        }
    }

    printf("Testing smaller lognormals\n");
    for (int i = 0; i < 100; i++) {
        double mean = sample_uniform(-1, 1, seed);
        double std = sample_uniform(0, 1, seed);
        if (std > 0) {
            test_lognormal(mean, std, seed);
        }
    }

    printf("Testing larger lognormals\n");
    for (int i = 0; i < 100; i++) {
        double mean = sample_uniform(-1, 5, seed);
        double std = sample_uniform(0, 5, seed);
        if (std > 0) {
            test_lognormal(mean, std, seed);
        }
    }

    printf("Testing lognormals — sample_to(low, high) syntax\n");
    for (int i = 0; i < 100; i++) {
        double low = sample_uniform(0, 1000 * 1000, seed);
        double high = sample_uniform(0, 1000 * 1000, seed);
        if (low < high) {
            test_to(low, high, seed);
        }
    }
		// Bonus example
		test_to(10, 10 * 1000, seed);

    printf("Testing beta distribution\n");
    for (int i = 0; i < 100; i++) {
        double a = sample_uniform(0, 1000, seed);
        double b = sample_uniform(0, 1000, seed);
        if ((a > 0) && (b > 0)) {
            test_beta(a, b, seed);
        }
    }

    printf("Testing larger beta distributions\n");
    for (int i = 0; i < 100; i++) {
        double a = sample_uniform(0, 1000 * 1000, seed);
        double b = sample_uniform(0, 1000 * 1000, seed);
        if ((a > 0) && (b > 0)) {
            test_beta(a, b, seed);
        }
    }

    free(seed);
}
