#ifndef SQUIGGLE_C_EXTRA
#define SQUIGGLE_C_EXTRA

/* Parallel sampling */
void sampler_parallel(double (*sampler)(uint64_t* seed), double* results, int n_threads, int n_samples);

/* Stats */
double array_get_median(double xs[], int n);
typedef struct ci_t {
    double low;
    double high;
} ci;
ci array_get_ci(ci interval, double* xs, int n);
ci array_get_90_ci(double xs[], int n);

void array_print_stats(double xs[], int n);
void array_print_histogram(double* xs, int n_samples, int n_bins);
void array_print_90_ci_histogram(double* xs, int n, int n_bins);

/* Algebra manipulations */

typedef struct normal_params_t {
    double mean;
    double std;
} normal_params;
normal_params algebra_sum_normals(normal_params a, normal_params b);

typedef struct lognormal_params_t {
    double logmean;
    double logstd;
} lognormal_params;
lognormal_params algebra_product_lognormals(lognormal_params a, lognormal_params b);

lognormal_params convert_ci_to_lognormal_params(ci x);
ci convert_lognormal_params_to_ci(lognormal_params y);

/* Utilities */

#define THOUSAND 1000
#define MILLION 1000000

#endif
