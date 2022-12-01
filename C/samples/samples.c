#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <gsl/gsl_rng.h>
#include <gsl/gsl_randist.h>

#define N 10000000
/* 
 * For very high values of N, you will want to increase the maximum stack trace, otherwise you will suffer a segmentation fault
 * In Ubuntu/bash you can do this with $ ulimit -Ss 256000 ## ~256Mbs
 * And confirm it with $ ulimit -a
*/

/* Helpers */
void print(double *ys){
  for(int i=0; i<N; i++){
    printf("%f\n", ys[i]);
  }
  printf("\n");
}

void fill(double *ys, float f){
  for(int i=0; i<N; i++){
    ys[i] = f;
  }
}

double sum(double *ps, int n){
  double result = 0;
  for(int i=0; i<n; i++){
    result += ps[i];
  }
  return(result);
}

void cumsum(double *ps, double *rs, int n){
  double counter = 0;
  for(int i=0; i<n; i++){
    counter += ps[i];
    rs[i] = counter;
  }
}

/* Distributions*/
void normal(gsl_rng * r, double *ys, double mean, double std){
  for(int i=0; i<N; i++){
    ys[i] = mean + gsl_ran_gaussian(r, std);
  }
}

void lognormal(gsl_rng * r, double *ys, double zeta, double sigma){
  for(int i=0; i<N; i++){
    ys[i] = gsl_ran_lognormal(r, zeta, sigma);
  }
}

void to(gsl_rng * r, double *ys, double low, double high){
  double normal95confidencePoint = 1.6448536269514722; 
  double log_low = log(low);
  double log_high = log(high);
  double zeta = (log_low + log_high)/2;
  double sigma = (log_high - log_low) / (2.0 * normal95confidencePoint);
  lognormal(r, ys, zeta, sigma);
}

/* Mixture of distributions */
void mixture(gsl_rng * r, double *dists[], double *weights, int n, double *results){
  /* Get cummulative, normalized weights */
  double sum_weights = sum(weights, n);
  double normalized_weights[n];
  for(int i=0; i<n; i++){
    normalized_weights[i] = weights[i]/sum_weights;
  }
  double cummulative_weights[n];
  cumsum(normalized_weights, cummulative_weights, n);
  
  /* Get N uniformly distributed vars */
  for(int i=0; i<N; i++){
    double p_1 = gsl_rng_uniform(r);
    double p_2 = gsl_rng_uniform(r);
    
    int index_found = 0;
    int index_counter = 0;
    while ((index_found == 0) && (index_counter < n) ){
      if(p_1 < cummulative_weights[index_counter]){
        index_found = 1;
      }else{
        index_counter++;
      }
    }
    if(index_found == 0) {
      printf("\nThis shouldn't have happened");
      // gsl_rng_free (r);
      // abort(); // this shouldn't have happened.
    }else{
      int sample_index = (int) floor(p_2 * N);
      results[i] = dists[index_counter][sample_index];
    }
  }


}

/* Main */
int main(void){
  /* Initialize GNU Statistical Library (GSL) stuff */
  const gsl_rng_type * T;
  gsl_rng * r;
  // gsl_rng_env_setup();
  T = gsl_rng_default;
  r = gsl_rng_alloc (T);
  
  /* Toy example */
  /* Declare variables in play */
  double p_a, p_b, p_c;
  double dist_none[N], dist_one[N], dist_few[N], dist_many[N], dist_mixture[N];
  
  /* Initialize variables */ 
  p_a = 0.8;
  p_b = 0.5;
  p_c = p_a * p_b;
  
  fill(dist_none, 0);
  fill(dist_one, 1);
  to(r, dist_few, 1, 3);
  to(r, dist_many, 2, 10);

  /* Generate mixture */
  int n = 4;
  double weights[] = { 1 - p_c, p_c/2, p_c/4, p_c/4};
  double *dists[] = { dist_none, dist_one, dist_few, dist_many };
  
  mixture(r, dists, weights, n, dist_mixture);
  printf("%f\n", sum(dist_mixture, N)/N);

  /* Clean up GSL */
  gsl_rng_free (r);

  /* Return success*/
  return EXIT_SUCCESS;
}
