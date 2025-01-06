package main

import (
	"errors"
	"fmt"
	"git.nunosempere.com/NunoSempere/fermi/sample"
	"math"
	"sort"
	"strconv"
)

func PrettyPrintInt(n int) {
	switch {
	case math.Abs(float64(n)) >= 1_000_000_000_000:
		fmt.Printf("%.2fT", float64(n)/1_000_000_000_000.0)
	case math.Abs(float64(n)) >= 1_000_000_000:
		fmt.Printf("%.2fB", float64(n)/1_000_000_000.0)
	case math.Abs(float64(n)) >= 1_000_000:
		fmt.Printf("%.2fM", float64(n)/1_000_000.0)
	case math.Abs(float64(n)) >= 1_000:
		fmt.Printf("%.2fK", float64(n)/1_000.0)
	default:
		fmt.Printf("%d", n)
	}
}

func PrettyPrintFloat(f float64) {
	switch {
	case math.Abs(f) >= 1_000_000_000_000:
		fmt.Printf("%.2fT", f/1_000_000_000_000)
	case math.Abs(f) >= 1_000_000_000:
		fmt.Printf("%.2fB", f/1_000_000_000)
	case math.Abs(f) >= 1_000_000:
		fmt.Printf("%.2fM", f/1_000_000)
	case math.Abs(f) >= 1_000:
		fmt.Printf("%.2fK", f/1_000)

	case math.Abs(f) <= 0.0001:
		fmt.Printf("%.6f", f)
	case math.Abs(f) <= 0.001:
		fmt.Printf("%.5f", f)
	case math.Abs(f) <= 0.01:
		fmt.Printf("%.4f", f)
	case math.Abs(f) <= 0.1:
		fmt.Printf("%.3f", f)
	default:
		fmt.Printf("%.2f", f)
	}

}
func PrettyPrint2Floats(low float64, high float64) {
	PrettyPrintFloat(low)
	fmt.Printf(" ")
	PrettyPrintFloat(high)
}

func multiplyOrPassThroughError(a float64, b float64, err error) (float64, error) {
	if err != nil {
		return b, err
	} else {
		return a * b, nil
	}
}

func ParseFloat(word string) (float64, error) {
	// l = len(word) // assuming no UTF stuff
	switch len(word) {
	case 0:
		return 0, errors.New("String to be parsed into float must not be the empty string")
	case 1:
		return strconv.ParseFloat(word, 64)
	}

	n := len(word) - 1
	f, err := strconv.ParseFloat(word[:n], 64)
	switch word[n] {
	case '%':
		return multiplyOrPassThroughError(0.01, f, err)
	case 'K':
		return multiplyOrPassThroughError(1_000, f, err)
	case 'M':
		return multiplyOrPassThroughError(1_000_000, f, err)
	case 'B':
		return multiplyOrPassThroughError(1_000_000_000, f, err)
	case 'T':
		return multiplyOrPassThroughError(1_000_000_000_000, f, err)
	default:
		return strconv.ParseFloat(word, 64)
	}

}

/* Printers */
func PrettyPrintDist(dist Dist) {
	switch v := dist.(type) {
	case Lognormal:
		fmt.Printf("=> ")
		PrettyPrint2Floats(v.low, v.high)
		fmt.Println()
	case Beta:
		fmt.Printf("=> beta ")
		PrettyPrint2Floats(v.a, v.b)
		fmt.Println()
	case Scalar:
		fmt.Printf("=> scalar ")
		w := float64(v)
		PrettyPrintFloat(w)
		fmt.Println()
	case FilledSamples:
		n := len(v.xs)
		sorted_xs := make([]float64, n)
		copy(sorted_xs, v.xs)
		sort.Slice(sorted_xs, func(i, j int) bool {
			return sorted_xs[i] < sorted_xs[j]
		})

		low := sorted_xs[int(math.Round(float64(n)*0.05))]
		high := sorted_xs[int(math.Round(float64(n)*0.95))]
		fmt.Printf("=> ")
		PrettyPrint2Floats(low, high)

		fmt.Printf(" (")
		PrettyPrintInt(N_SAMPLES)
		fmt.Printf(" samples)")
		fmt.Println()
	default:
		fmt.Printf("%v\n", v)
	}
}

func PrettyPrintStats(dist Dist) {
	xs := sample.Serially(dist.Sampler, N_SAMPLES)
	n := len(xs)

	mean := 0.0
	for i := 0; i < n; i++ {
		mean += xs[i]
	}
	mean /= float64(n)
	fmt.Printf("Mean:   %f\n", mean)

	stdev := 0.0
	for i := 0; i < n; i++ {
		stdev += math.Pow(xs[i]-mean, 2)
	}
	stdev = math.Sqrt(stdev / float64(n))
	fmt.Printf("Stdev:  %f\n", stdev)

	sorted_xs := make([]float64, n)
	copy(sorted_xs, xs)
	sort.Slice(sorted_xs, func(i, j int) bool {
		return sorted_xs[i] < sorted_xs[j]
	})
	print_ci := func(ci float64, prefix string) {
		x := sorted_xs[int(math.Round(float64(n)*ci))]
		fmt.Printf("%s%f\n", prefix, x)
	}
	print_ci(0.01, "ci 1%:  ")
	print_ci(0.05, "ci 5%:  ")
	print_ci(0.10, "ci 10%: ")
	print_ci(0.25, "ci 25%: ")
	print_ci(0.50, "ci 50%: ")
	print_ci(0.75, "ci 75%: ")
	print_ci(0.90, "ci 90%: ")
	print_ci(0.95, "ci 95%: ")
	print_ci(0.99, "ci 99%: ")
}

func PrintAndReturnErr(err_msg string) error {
	fmt.Println(err_msg)
	fmt.Println("Type \"help\" (without quotes) to see a pseudogrammar and examples")
	return errors.New(err_msg)
}
