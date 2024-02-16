package main

import "fmt"
import "math"
import rand "math/rand/v2"

var r = rand.New(rand.NewPCG(1, 2))

// https://pkg.go.dev/math/rand/v2

func sample_unit_uniform() float64 {
	return r.Float64()
}

func sample_unit_normal() float64 {
	return r.NormFloat64()
}

func sample_uniform(start float64, end float64) float64 {
	return sample_unit_uniform()*(end-start) + start
}

func sample_normal(mean float64, sigma float64) float64 {
	return mean + sample_unit_normal()*sigma
}

func sample_lognormal(logmean float64, logstd float64) float64 {
	return (math.Exp(sample_normal(logmean, logstd)))
}

func sample_normal_from_90_ci(low float64, high float64) float64 {
	var normal90 float64 = 1.6448536269514727
	var mean float64 = (high + low) / 2.0
	var std float64 = (high - low) / (2.0 * normal90)
	return sample_normal(mean, std)

}

func sample_to(low float64, high float64) float64 {
	// Given a (positive) 90% confidence interval,
	// returns a sample from a lognorma with a matching 90% c.i.
	// Key idea: If we want a lognormal with 90% confidence interval [a, b]
	// we need but get a normal with 90% confidence interval [log(a), log(b)].
	// Then see code for sample_normal_from_90_ci
	var loglow float64 = math.Log(low)
	var loghigh float64 = math.Log(high)
	return math.Exp(sample_normal_from_90_ci(loglow, loghigh))
}

type func64 func() float64

func sample_mixture(fs []func64, weights []float64) float64 {
	var sum_weights float64 = 0
	for _, weight := range weights {
		sum_weights += weight
	}

	var total float64 = 0
	for i, weight := range weights {
		total += weight / sum_weights
		weights[i] = total
	}

	var result float64
	var flag int = 0
	var p float64 = r.Float64()

	for i, weight := range weights {
		if p < weight {
			result = fs[i]()
			flag = 1
			break
		}
	}

	if flag == 0 {
		result = fs[len(fs)-1]()
	}
	return result
	// return weights[0]

}

func main() {

	f1 := func() float64 { return sample_to(1, 10) }
	f2 := func() float64 { return sample_to(100, 1000) }
	fs := [2](func64){f1, f2}
	ws := [2](float64){0.4, 0.1}

	var n_samples int = 1000000
	var avg float64 = 0
	for i := 0; i < n_samples; i++ {
		avg += sample_mixture(fs[0:], ws[0:])
	}
	avg = avg / float64(n_samples)
	fmt.Printf("%v\n", avg)

}
