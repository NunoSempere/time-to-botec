package main

import "fmt"
import "math"
import "sync"
import rand "math/rand/v2"

type src = *rand.Rand
type func64 = func(src) float64

// https://pkg.go.dev/math/rand/v2

func sample_unit_uniform(r src) float64 {
	return r.Float64()
}

func sample_unit_normal(r src) float64 {
	return r.NormFloat64()
}

func sample_uniform(start float64, end float64, r src) float64 {
	return sample_unit_uniform(r)*(end-start) + start
}

func sample_normal(mean float64, sigma float64, r src) float64 {
	return mean + sample_unit_normal(r)*sigma
}

func sample_lognormal(logmean float64, logstd float64, r src) float64 {
	return (math.Exp(sample_normal(logmean, logstd, r)))
}

func sample_normal_from_90_ci(low float64, high float64, r src) float64 {
	var normal90 float64 = 1.6448536269514727
	var mean float64 = (high + low) / 2.0
	var std float64 = (high - low) / (2.0 * normal90)
	return sample_normal(mean, std, r)

}

func sample_to(low float64, high float64, r src) float64 {
	// Given a (positive) 90% confidence interval,
	// returns a sample from a lognorma with a matching 90% c.i.
	// Key idea: If we want a lognormal with 90% confidence interval [a, b]
	// we need but get a normal with 90% confidence interval [log(a), log(b)].
	// Then see code for sample_normal_from_90_ci
	var loglow float64 = math.Log(low)
	var loghigh float64 = math.Log(high)
	return math.Exp(sample_normal_from_90_ci(loglow, loghigh, r))
}

func sample_mixture(fs []func64, weights []float64, r src) float64 {

	// fmt.Println("weights initially: ", weights)
	var sum_weights float64 = 0
	for _, weight := range weights {
		sum_weights += weight
	}

	var total float64 = 0
	var cumsummed_normalized_weights = append([]float64(nil), weights...)
	for i, weight := range weights {
		total += weight / sum_weights
		cumsummed_normalized_weights[i] = total
	}

	var result float64
	var flag int = 0
	var p float64 = r.Float64()

	for i, cnw := range cumsummed_normalized_weights {
		if p < cnw {
			result = fs[i](r)
			flag = 1
			break
		}
	}
	// fmt.Println(cumsummed_normalized_weights)

	if flag == 0 {
		result = fs[len(fs)-1](r)
	}
	return result

}

func slice_fill(xs []float64, fs func64, r src) {
	for i := range xs {
		xs[i] = fs(r)
	}
}

func sample_parallel(f func64, n_samples int) []float64 {
	var num_threads = 16
	var xs = make([]float64, n_samples)
	var wg sync.WaitGroup
	var h = n_samples / num_threads
	wg.Add(num_threads)
	for i := range num_threads {
		var xs_i = xs[i*h : (i+1)*h]
		go func(f func64) {
			defer wg.Done()
			var r = rand.New(rand.NewPCG(uint64(i), uint64(i+1)))
			for i := range xs_i {
				xs_i[i] = f(r)
			}
		}(f)
	}

	wg.Wait()
	return xs

}

func main() {

	var p_a float64 = 0.8
	var p_b float64 = 0.5
	var p_c float64 = p_a * p_b
	ws := [4](float64){1 - p_c, p_c / 2, p_c / 4, p_c / 4}

	sample_0 := func(r src) float64 { return 0 }
	sample_1 := func(r src) float64 { return 1 }
	sample_few := func(r src) float64 { return sample_to(1, 3, r) }
	sample_many := func(r src) float64 { return sample_to(2, 10, r) }
	fs := [4](func64){sample_0, sample_1, sample_few, sample_many}

	model := func(r src) float64 { return sample_mixture(fs[0:], ws[0:], r) }
	n_samples := 1_000_000
	xs := sample_parallel(model, n_samples)
	var avg float64 = 0
	for _, x := range xs {
		avg += x
	}
	avg = avg / float64(n_samples)
	fmt.Printf("Average: %v\n", avg)
	/*
		n_samples := 1_000_000
		var r = rand.New(rand.NewPCG(uint64(1), uint64(2)))
		var avg float64 = 0
		for i := 0; i < n_samples; i++ {
			avg += sample_mixture(fs[0:], ws[0:], r)
		}
		avg = avg / float64(n_samples)
		fmt.Printf("Average: %v\n", avg)
	*/
}
