#ifndef SQUIGGLE_C_EXTRA
#define SQUIGGLE_C_EXTRA

/* Parallel sampling */
void sampler_parallel(double (*sampler)(uint64_t* seed), double* results, int n_threads, int n_samples);

/* Get 90% confidence interval */
typedef struct ci_t {
    double low;
    double high;
} ci;
ci array_get_ci(ci interval, double* xs, int n);
ci array_get_90_ci(double xs[], int n);
ci sampler_get_ci(ci interval, double (*sampler)(uint64_t*), int n, uint64_t* seed);
ci sampler_get_90_ci(double (*sampler)(uint64_t*), int n, uint64_t* seed);

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

/* Error handling */
typedef struct box_t {
    int empty;
    double content;
    char* error_msg;
} box;
#define MAX_ERROR_LENGTH 500
#define EXIT_ON_ERROR 0
#define PROCESS_ERROR(error_msg) process_error(error_msg, EXIT_ON_ERROR, __FILE__, __LINE__)
box process_error(const char* error_msg, int should_exit, char* file, int line);
void array_print(double* array, int length);

/* Inverse cdf */
box inverse_cdf_double(double cdf(double), double p);
box inverse_cdf_box(box cdf_box(double), double p);

/* Samplers from cdf */
box sampler_cdf_double(double cdf(double), uint64_t* seed);
box sampler_cdf_box(box cdf(double), uint64_t* seed);

#endif
