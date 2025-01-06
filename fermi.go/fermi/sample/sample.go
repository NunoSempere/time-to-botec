package sample

import (
	"math"
	"sync"

	rand "math/rand/v2"

	"github.com/pkg/errors"
)

// https://pkg.go.dev/math/rand/v2

type State = *rand.Rand
type func64 = func(State) float64
type func64i = func(int, State) float64

var global_state = rand.New(rand.NewPCG(uint64(1), uint64(2)))

func Int(n int, r State) int {
	return r.IntN(n)
}

func Unit_uniform(r State) float64 {
	return r.Float64()
}

func Unit_normal(r State) float64 {
	return r.NormFloat64()
}

func Uniform(start float64, end float64, r State) float64 {
	return Unit_uniform(r)*(end-start) + start
}

func Normal(mean float64, sigma float64, r State) float64 {
	return mean + Unit_normal(r)*sigma
}

func Lognormal(logmean float64, logstd float64, r State) float64 {
	return (math.Exp(Normal(logmean, logstd, r)))
}

func Normal_from_90_ci(low float64, high float64, r State) float64 {
	var normal90 float64 = 1.6448536269514727
	var mean float64 = (high + low) / 2.0
	var std float64 = (high - low) / (2.0 * normal90)
	return Normal(mean, std, r)
}

func To(low float64, high float64, r State) float64 {
	// Given a (positive) 90% confidence interval,
	// returns a sample from a lognorma with a matching 90% c.i.
	// Key idea: If we want a lognormal with 90% confidence interval [a, b]
	// we need but get a normal with 90% confidence interval [log(a), log(b)].
	// Then see code for Normal_from_90_ci
	var loglow float64 = math.Log(low)
	var loghigh float64 = math.Log(high)
	return math.Exp(Normal_from_90_ci(loglow, loghigh, r))
}

func Gamma(alpha float64, r State) float64 {

	// a simple method for generating gamma variables, marsaglia and wan tsang, 2001
	// https://dl.acm.org/doi/pdf/10.1145/358407.358414
	// see also the references/ folder
	// note that the wikipedia page for the gamma distribution includes a scaling parameter
	// k or beta
	// https://en.wikipedia.org/wiki/gamma_distribution
	// such that gamma_k(alpha, k) = k * gamma(alpha)
	// or gamma_beta(alpha, beta) = gamma(alpha) / beta
	// so far i have not needed to use this, and thus the second parameter is by default 1.

	if alpha >= 1 {
		var d, c, x, v, u float64
		d = alpha - 1.0/3.0
		c = 1.0 / math.Sqrt(9.0*d)

		for {

		InnerLoop:
			for {
				x = Unit_normal(r)
				v = 1.0 + c*x
				if v > 0.0 {
					break InnerLoop
				}
			}

			v = v * v * v
			u = Unit_uniform(r)

			if u < 1.0-0.0331*(x*x*x*x) { // Condition 1
				// the 0.0331 doesn't inspire much confidence
				// however, this isn't the whole story
				// by knowing that Condition 1 implies condition 2
				// we realize that this is just a way of making the algorithm faster
				// i.e., of not using the logarithms
				return d * v
			}
			if math.Log(u) < 0.5*(x*x)+d*(1.0-v+math.Log(v)) { // Condition 2
				return d * v
			}

		}

	} else {
		return Gamma(1.0+alpha, r) * math.Pow(Unit_uniform(r), 1.0/alpha)
	}
}

func Beta(a float64, b float64, r State) float64 {
	gamma_a := Gamma(a, r)
	gamma_b := Gamma(b, r)
	return gamma_a / (gamma_a + gamma_b)
}

func Mixture_once(fs []func64, weights []float64, r State) float64 {

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

	if flag == 0 {
		result = fs[len(fs)-1](r)
	}
	return result

}

func Serially(f func64i, n_samples int) []float64 {
	xs := make([]float64, n_samples)
	// var global_state = rand.New(rand.NewPCG(uint64(1), uint64(2)))
	for i := 0; i < n_samples; i++ {
		xs[i] = f(i, global_state)
	}
	return xs
}

func Mixture_serially_from_samples(fs [][]float64, weights []float64, n_samples int) ([]float64, error) {

	// Checks
	if len(weights) != len(fs) {
		return nil, errors.New("Mixture must have dists and weights alternated")
	}
	for _, f := range fs {
		if len(f) < n_samples {
			return nil, errors.New("Mixture components don't have enough samples")
		}
	}
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
	if total == 0.0 {
		return nil, errors.New("Cummulative sum of weights in mixture must be > 0.0")
	}

	// fmt.Printf("Weights: %v\n", cumsummed_normalized_weights)

	xs := make([]float64, n_samples)
	// var global_state = rand.New(rand.NewPCG(uint64(1), uint64(2)))
	for i := 0; i < n_samples; i++ {
		var flag int = 0
		var p float64 = global_state.Float64()
		for j, cnw := range cumsummed_normalized_weights {
			if p < cnw {
				xs[i] = fs[j][i]
				flag = 1
				break
			}
		}
		if flag == 0 {
			xs[i] = fs[len(fs)-1][i]
		}
	}
	return xs, nil
}

func Mixture_serially_from_samplers(fs []func64i, weights []float64, n_samples int) ([]float64, error) {

	// Checks
	if len(weights) != len(fs) {
		return nil, errors.New("Mixture must have dists and weights alternated")
	}
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
	if total == 0.0 {
		return nil, errors.New("Cummulative sum of weights in mixture must be > 0.0")
	}

	// fmt.Printf("Weights: %v\n", cumsummed_normalized_weights)
	xs := make([]float64, n_samples)
	for i := 0; i < n_samples; i++ {
		var flag int = 0
		var p float64 = global_state.Float64()
		for j, cnw := range cumsummed_normalized_weights {
			if p < cnw {
				xs[i] = fs[j](i, global_state)
				flag = 1
				break
			}
		}
		if flag == 0 {
			xs[i] = fs[len(fs)-1](i, global_state)
		}
	}
	return xs, nil
}

func Parallel(f func64, n_samples int) []float64 {
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
			for j := range xs_i {
				xs_i[j] = f(r)
			}
		}(f)
	}

	wg.Wait()
	return xs

}

/*
func main() {

	var p_a float64 = 0.8
	var p_b float64 = 0.5
	var p_c float64 = p_a * p_b
	ws := [4](float64){1 - p_c, p_c / 2, p_c / 4, p_c / 4}

	Sample_0 := func(r State) float64 { return 0 }
	Sample_1 := func(r State) float64 { return 1 }
	Sample_few := func(r State) float64 { return Sample_to(1, 3, r) }
	Sample_many := func(r State) float64 { return Sample_to(2, 10, r) }
	fs := [4](func64){Sample_0, Sample_1, Sample_few, Sample_many}

	model := func(r State) float64 { return Mixture(fs[0:], ws[0:], r) }
	n_samples := 1_000_000
	xs := Parallel(model, n_samples)
	var avg float64 = 0
	for _, x := range xs {
		avg += x
	}
	avg = avg / float64(n_samples)
	fmt.Printf("Average: %v\n", avg)
	/*
		  // Without concurrency:
			n_samples := 1_000_000
			var r = rand.New(rand.NewPCG(uint64(1), uint64(2)))
			var avg float64 = 0
			for i := 0; i < n_samples; i++ {
				avg += Mixture(fs[0:], ws[0:], r)
			}
			avg = avg / float64(n_samples)
			fmt.Printf("Average: %v\n", avg)
}
*/
