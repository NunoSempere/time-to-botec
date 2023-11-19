#include "../../../squiggle.h"
#include "../../../squiggle_more.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define NUM_SAMPLES 10000
#define STOP_BETA 1.0e-8
#define TINY_BETA 1.0e-30

// Incomplete beta function
struct box incbeta(double a, double b, double x)
{
    // Descended from <https://github.com/codeplea/incbeta/blob/master/incbeta.c>,
    // <https://codeplea.com/incomplete-beta-function-c>
    // but modified to return a box struct and doubles instead of doubles.
    // [ ] to do: add attribution in README
    // Original code under this license:
    /*
		 * zlib License
		 *
		 * Regularized Incomplete Beta Function
		 *
		 * Copyright (c) 2016, 2017 Lewis Van Winkle
		 * http://CodePlea.com
		 *
		 * This software is provided 'as-is', without any express or implied
		 * warranty. In no event will the authors be held liable for any damages
		 * arising from the use of this software.
		 *
		 * Permission is granted to anyone to use this software for any purpose,
		 * including commercial applications, and to alter it and redistribute it
		 * freely, subject to the following restrictions:
		 *
		 * 1. The origin of this software must not be misrepresented; you must not
		 *    claim that you wrote the original software. If you use this software
		 *    in a product, an acknowledgement in the product documentation would be
		 *    appreciated but is not required.
		 * 2. Altered source versions must be plainly marked as such, and must not be
		 *    misrepresented as being the original software.
		 * 3. This notice may not be removed or altered from any source distribution.
		 */
    if (x < 0.0 || x > 1.0) {
        return PROCESS_ERROR("x out of bounds [0, 1], in function incbeta");
    }

    /*The continued fraction converges nicely for x < (a+1)/(a+b+2)*/
    if (x > (a + 1.0) / (a + b + 2.0)) {
        struct box symmetric_incbeta = incbeta(b, a, 1.0 - x);
        if (symmetric_incbeta.empty) {
            return symmetric_incbeta; // propagate error
        } else {
            struct box result = {
                .empty = 0,
                .content = 1 - symmetric_incbeta.content
            };
            return result;
        }
    }

    /*Find the first part before the continued fraction.*/
    const double lbeta_ab = lgamma(a) + lgamma(b) - lgamma(a + b);
    const double front = exp(log(x) * a + log(1.0 - x) * b - lbeta_ab) / a;

    /*Use Lentz's algorithm to evaluate the continued fraction.*/
    double f = 1.0, c = 1.0, d = 0.0;

    int i, m;
    for (i = 0; i <= 200; ++i) {
        m = i / 2;

        double numerator;
        if (i == 0) {
            numerator = 1.0; /*First numerator is 1.0.*/
        } else if (i % 2 == 0) {
            numerator = (m * (b - m) * x) / ((a + 2.0 * m - 1.0) * (a + 2.0 * m)); /*Even term.*/
        } else {
            numerator = -((a + m) * (a + b + m) * x) / ((a + 2.0 * m) * (a + 2.0 * m + 1)); /*Odd term.*/
        }

        /*Do an iteration of Lentz's algorithm.*/
        d = 1.0 + numerator * d;
        if (fabs(d) < TINY_BETA)
            d = TINY_BETA;
        d = 1.0 / d;

        c = 1.0 + numerator / c;
        if (fabs(c) < TINY_BETA)
            c = TINY_BETA;

        const double cd = c * d;
        f *= cd;

        /*Check for stop.*/
        if (fabs(1.0 - cd) < STOP_BETA) {
            struct box result = {
                .empty = 0,
                .content = front * (f - 1.0)
            };
            return result;
        }
    }

    return PROCESS_ERROR("More loops needed, did not converge, in function incbeta");
}

struct box cdf_beta(double x)
{
    if (x < 0) {
        struct box result = { .empty = 0, .content = 0 };
        return result;
    } else if (x > 1) {
        struct box result = { .empty = 0, .content = 1 };
        return result;
    } else {
        double successes = 1, failures = (2023 - 1945);
        return incbeta(successes, failures, x);
    }
}

// Some testers
void test_inverse_cdf_box(char* cdf_name, struct box cdf_box(double))
{
    struct box result = inverse_cdf_box(cdf_box, 0.5);
    if (result.empty) {
        printf("Inverse for %s not calculated\n", cdf_name);
        exit(1);
    } else {
        printf("Inverse of %s at %f is: %f\n", cdf_name, 0.5, result.content);
    }
}

void test_and_time_sampler_box(char* cdf_name, struct box cdf_box(double), uint64_t* seed)
{
    printf("\nGetting some samples from %s:\n", cdf_name);
    clock_t begin = clock();
    for (int i = 0; i < NUM_SAMPLES; i++) {
        struct box sample = sampler_cdf_box(cdf_box, seed);
        if (sample.empty) {
            printf("Error in sampler function for %s", cdf_name);
        } else {
            // printf("%f\n", sample.content);
        }
    }
    clock_t end = clock();
    double time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
    printf("Time spent: %f\n", time_spent);
}

int main()
{
    // Test inverse cdf box
    test_inverse_cdf_box("cdf_beta", cdf_beta);

    // Test box sampler
    uint64_t* seed = malloc(sizeof(uint64_t));
    *seed = 1000; // xorshift can't start with 0
    test_and_time_sampler_box("cdf_beta", cdf_beta, seed);
    // Ok, this is slower than python!!
    // Partly this is because I am using a more general algorithm,
    //   which applies to any cdf
    // But I am also using absurdly precise convergence conditions.
    // This could be optimized.

    free(seed);
    return 0;
}
