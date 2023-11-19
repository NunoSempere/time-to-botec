#ifndef SQUIGGLE_C_EXTRA
#define SQUIGGLE_C_EXTRA 

// Box
struct box {
    int empty;
    double content;
    char* error_msg;
};

// Macros to handle errors
#define MAX_ERROR_LENGTH 500
#define EXIT_ON_ERROR 0
#define PROCESS_ERROR(error_msg) process_error(error_msg, EXIT_ON_ERROR, __FILE__, __LINE__)
struct box process_error(const char* error_msg, int should_exit, char* file, int line);

// Inverse cdf
struct box inverse_cdf_double(double cdf(double), double p);
struct box inverse_cdf_box(struct box cdf_box(double), double p);

// Samplers from cdf
struct box sampler_cdf_double(double cdf(double), uint64_t* seed);
struct box sampler_cdf_box(struct box cdf(double), uint64_t* seed);

// Get 90% confidence interval
typedef struct ci_t {
    float low;
    float high;
} ci;
ci get_90_confidence_interval(double (*sampler)(uint64_t*), uint64_t* seed);

// small algebra manipulations

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
ci               convert_lognormal_params_to_ci(lognormal_params y);

void parallel_sampler(double (*sampler)(uint64_t* seed), double* results, int n_threads, int n_samples);

#endif
