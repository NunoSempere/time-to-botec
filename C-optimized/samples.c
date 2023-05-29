#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <omp.h>

const float PI = 3.14159265358979323846;

#define N 1000000

//Array helpers

void array_print(float* array, int length) {
    for (int i = 0; i < length; i++)
    {
        printf("item[%d] = %f\n", i, array[i]);
    }
    printf("\n");
}

void array_fill(float* array, int length, float item) {
    int i;
    #pragma omp private (i)
    {
        #pragma omp for
        for (i = 0; i < length; i++)
        {
            array[i] = item;
        }
    }    
}

float array_sum(float* array, int length) {
    float output = 0.0;
    for (int i = 0; i < length; i++)
    {
        output += array[i];
    }
    return output;
}

void array_cumsum(float* array_to_sum, float* array_cumsummed, int length) {
    array_cumsummed[0] = array_to_sum[0];
    for (int i = 1; i < length; i++)
    {
        array_cumsummed[i] = array_cumsummed[i-1] + array_to_sum[i]; 
    }
}
 
float rand_float(float to) {
    return ((float)rand()/(float)RAND_MAX) * to;
}

float ur_normal() {
    float u1 = rand_float(1.0);
    float u2 = rand_float(1.0);
    float z = sqrtf(-2.0 * log(u1)) * sin(2 * PI * u2);
    return z;
}

inline float random_uniform(float from, float to) {
    return ((float)rand()/(float)RAND_MAX)*(to-from)+from;
}

inline float random_normal(float mean, float sigma) {
    return (mean + sigma * ur_normal());
}

inline float random_lognormal(float logmean, float logsigma) {
    return expf(random_normal(logmean, logsigma));
}

inline float random_to(float low, float high) {
    const float NORMAL95CONFIDENCE = 1.6448536269514722;
    float loglow = logf(low);
    float loghigh = logf(high);
    float logmean = (loglow + loghigh) / 2;
    float logsigma = (loghigh - loglow) / (2.0 * NORMAL95CONFIDENCE);
    return random_lognormal(logmean, logsigma);
}

void array_random_to(float* array, int length, float low, float high) {
    int i;
    #pragma omp private(i) 
    {
        #pragma omp for
        for (i = 0; i < length; i++)
        {
            array[i] = random_to(low, high);
        }
    }
}

int split_array_get_my_length(int index, int total_length, int n_threads) {
    return (total_length%n_threads > index ? total_length/n_threads+1 : total_length/n_threads);
}

//Old version, don't use it!! Optimized version is called mixture_f. This one is just for display
void mixture(float* dists[], float* weights, int n_dists, float* results) {
    float sum_weights = array_sum(weights, n_dists);
    float* normalized_weights = malloc(n_dists * sizeof(float));
    for (int i = 0; i < n_dists; i++) {normalized_weights[i] = weights[i] / sum_weights;}
    
    float* cummulative_weights = malloc(n_dists * sizeof(float));
    array_cumsum(normalized_weights, cummulative_weights, n_dists);

    //create var holders
    float p1, p2;
    int index_found, index_counter, sample_index, i;

    #pragma omp parallel private (i, p1, p2, index_found, index_counter, sample_index) 
    {
        #pragma omp for
        for (i = 0; i < N; i++)
        {
            p1 = random_uniform(0, 1);
            p2 = random_uniform(0, 1);

            index_found = 0;
            index_counter = 0;

            while ((index_found == 0) && (index_counter < n_dists))
            {
                if (p1 < cummulative_weights[index_counter])
                {
                    index_found = 1;
                } else
                {
                    index_counter++;
                }
            }
            if (index_found != 0)
            {
                sample_index = (int) (p2 * N);
                results[i] = dists[index_counter][sample_index];
            } 
            else printf("This shouldn't be able to happen.\n"); 
        }
    }
    free(normalized_weights);
    free(cummulative_weights);
}

void mixture_f(float (*samplers[])(void), float* weights, int n_dists, float** results, int n_threads) {
    float sum_weights = array_sum(weights, n_dists);
    float* normalized_weights = malloc(n_dists * sizeof(float));
    for (int i = 0; i < n_dists; i++) {normalized_weights[i] = weights[i] / sum_weights;}
    
    float* cummulative_weights = malloc(n_dists * sizeof(float));
    array_cumsum(normalized_weights, cummulative_weights, n_dists);

    //create var holders
    float p1;
    int sample_index, i, own_length;

    #pragma omp parallel private (i, p1, sample_index, own_length) 
    {
        #pragma omp for
        for (i = 0; i < n_threads; i++)
        {
            own_length = split_array_get_my_length(i, N, n_threads);
            for (int j = 0; j < own_length; j++)
            {
                p1 = random_uniform(0, 1);
                for (int k = 0; k < n_dists; k++)
                {
                    if (p1 < cummulative_weights[k])
                    {
                        results[i][j] = samplers[k]();
                        break;
                    }
                }
            }
        }
    }
    free(normalized_weights);
    free(cummulative_weights);
}

float sample_0() {
    return 0;
}

float sample_1() {
    return 1;
}

float sample_few() {
    return random_to(1, 3);
}

float sample_many() {
    return random_to(2, 10);
}

void split_array_allocate(float** meta_array, int length, int divide_into) {
    int own_length;

    for (int i = 0; i < divide_into; i++)
    {
        own_length = split_array_get_my_length(i, length, divide_into);
        meta_array[i] = malloc(own_length*sizeof(float));
    }
    
}

void split_array_free(float** meta_array, int divided_into) {
    for (int i = 0; i < divided_into; i++)
    {
        free(meta_array[i]);
    }
    free(meta_array);
}

float split_array_sum(float** meta_array, int length, int divided_into) {
    int i;
    float output;
    float* partial_sum = malloc(divided_into*sizeof(float));

    #pragma omp private(i) shared(partial_sum)
    for (int i = 0; i < divided_into; i++)
    {
        float own_partial_sum = 0;
        int own_length = split_array_get_my_length(i, length, divided_into);
        for (int j = 0; j < own_length; j++)
        {
            own_partial_sum += meta_array[i][j];
        }
        partial_sum[i] = own_partial_sum;
    }
    for (int i = 0; i < divided_into; i++)
    {
        output += partial_sum[i];
    }
    return output;
}

int main() {
    clock_t start, end;
    start = clock();
    //initialize randomness
    srand(time(NULL));
    //Toy example
    // Declare variables in play
    float p_a, p_b, p_c;
    int n_threads = omp_get_max_threads();
    float** dist_mixture = malloc(n_threads*sizeof(float*));
    split_array_allocate(dist_mixture, N, n_threads);

    // Initialize variables
    p_a = 0.8;
    p_b = 0.5;
    p_c = p_a * p_b;

    // Generate mixture
    int n_dists = 4;
    float weights[] = { 1 - p_c, p_c / 2, p_c / 4, p_c / 4 };
    float (*samplers[])(void) = {sample_0, sample_1, sample_few, sample_many};

    mixture_f(samplers, weights, n_dists, dist_mixture, n_threads);
    // printf("Sum(dist_mixture, N)/N = %f\n", split_array_sum(dist_mixture, N, n_threads) / N);

    end = clock();
    split_array_free(dist_mixture, n_threads);

    printf("Total time (ms): %f\n", ((double) (end-start)) / CLOCKS_PER_SEC * 1000);
    return 0;
}