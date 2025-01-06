package main

import (
	"bufio"
	"errors"
	"fmt"
	"git.nunosempere.com/NunoSempere/fermi/pretty"
	"git.nunosempere.com/NunoSempere/fermi/sample"
	"math"
	"os"
	"sort"
	"strconv"
	"strings"
)

/* Types and interfaces */

type Dist interface {
	Samples() []float64
}

type Scalar float64

type Lognormal struct {
	low  float64
	high float64
}

type Beta struct {
	a float64
	b float64
}

type FilledSamples struct {
	xs []float64
}

/* Dist interface functions */
// https://go.dev/tour/methods/9
func (p Scalar) Samples() []float64 {
	xs := make([]float64, N_SAMPLES)
	for i := 0; i < N_SAMPLES; i++ {
		xs[i] = float64(p)
	}
	return xs
}

func (ln Lognormal) Samples() []float64 {
	sampler := func(r sample.Src) float64 { return sample.Sample_to(ln.low, ln.high, r) }
	return sample.Sample_serially(sampler, N_SAMPLES)
}

func (beta Beta) Samples() []float64 {
	sampler := func(r sample.Src) float64 { return sample.Sample_beta(beta.a, beta.b, r) }
	return sample.Sample_serially(sampler, N_SAMPLES)
}

func (fs FilledSamples) Samples() []float64 {
	return fs.xs
}

/* Constants */
const HELP_MSG = "  Operation | Variable assignment | Special\n" +
	"    Operation:                             operator operand\n" +
	"          operator:                        (empty) | * | / | + | -\n" +
	"          operand:                         scalar | lognormal | beta\n" +
	"            lognormal:                     low high\n" +
	"            beta:                          beta alpha beta\n" +
	"    Clear stack:                           . | c | clear\n" +
	"    Other special operations:              help | debug | exit\n" +
	"  Examples: \n" +
	"    + 2\n" +
	"    / 2.5\n" +
	"    * 1 10 (interpreted as lognormal)\n" +
	"    + 1 10\n" +
	"    * beta 1 10\n" +
	"    1 10 (multiplication taken as default operation)\n" +
	"    .\n" +
	"    1 100\n" +
	"    exit\n"
const NORMAL90CONFIDENCE = 1.6448536269514727
const INIT_DIST Scalar = Scalar(1)
const N_SAMPLES = 100_000

/* Printers */
func prettyPrintDist(dist Dist) {
	switch v := dist.(type) {
	case Lognormal:
		fmt.Printf("=> ")
		pretty.PrettyPrint2Floats(v.low, v.high)
		fmt.Println()
	case Beta:
		fmt.Printf("=> beta ")
		pretty.PrettyPrint2Floats(v.a, v.b)
		fmt.Println()
	case Scalar:
		fmt.Printf("=> scalar ")
		w := float64(v)
		pretty.PrettyPrintFloat(w)
		fmt.Println()
	case FilledSamples:
		sorted_xs := make([]float64, N_SAMPLES)
		copy(sorted_xs, v.xs)
		sort.Slice(sorted_xs, func(i, j int) bool {
			return sorted_xs[i] < sorted_xs[j]
		})

		low := sorted_xs[N_SAMPLES/20]
		high := sorted_xs[N_SAMPLES*19/20]
		fmt.Printf("=> ")
		pretty.PrettyPrint2Floats(low, high)

		fmt.Printf(" (")
		pretty.PrettyPrintInt(N_SAMPLES)
		fmt.Printf(" samples)")
		fmt.Println()
	default:
		fmt.Printf("%v\n", v)
	}
}

func printAndReturnErr(err_msg string) error {
	fmt.Println(err_msg)
	fmt.Println(HELP_MSG)
	return errors.New(err_msg)
}

/* Operations */
// Generic operations with samples
func operateDistsAsSamples(dist1 Dist, dist2 Dist, op string) (Dist, error) {

	xs := dist1.Samples()
	ys := dist2.Samples()
	zs := make([]float64, N_SAMPLES)

	for i := 0; i < N_SAMPLES; i++ {
		switch op {
		case "*":
			zs[i] = xs[i] * ys[i]
		case "/":
			if ys[0] != 0 {
				zs[i] = xs[i] / ys[i]
			} else {
				fmt.Println("Error: When dividing as samples, division by zero")
				return nil, errors.New("Division by zero")
			}
		case "+":
			zs[i] = xs[i] + ys[i]
		case "-":
			zs[i] = xs[i] - ys[i]
		}
	}

	// fmt.Printf("%v\n", zs)
	return FilledSamples{xs: zs}, nil
}

// Multiplication
func multiplyLogDists(l1 Lognormal, l2 Lognormal) Lognormal {
	logmean1 := (math.Log(l1.high) + math.Log(l1.low)) / 2.0
	logstd1 := (math.Log(l1.high) - math.Log(l1.low)) / (2.0 * NORMAL90CONFIDENCE)

	logmean2 := (math.Log(l2.high) + math.Log(l2.low)) / 2.0
	logstd2 := (math.Log(l2.high) - math.Log(l2.low)) / (2.0 * NORMAL90CONFIDENCE)

	logmean_product := logmean1 + logmean2
	logstd_product := math.Sqrt(logstd1*logstd1 + logstd2*logstd2)

	h := logstd_product * NORMAL90CONFIDENCE
	loglow := logmean_product - h
	loghigh := logmean_product + h
	return Lognormal{low: math.Exp(loglow), high: math.Exp(loghigh)}

}

func multiplyDists(old_dist Dist, new_dist Dist) (Dist, error) {

	switch o := old_dist.(type) {
	case Lognormal:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDists(o, n), nil
			case Scalar:
				return multiplyLogDists(o, Lognormal{low: float64(n), high: float64(n)}), nil
			default:
				return operateDistsAsSamples(old_dist, new_dist, "*")
			}
		}
	case Scalar:
		{
			if o == 1 {
				return new_dist, nil
			}
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDists(Lognormal{low: float64(o), high: float64(o)}, n), nil
			case Scalar:
				return Scalar(float64(o) * float64(n)), nil
			default:
				return operateDistsAsSamples(old_dist, new_dist, "*")
			}
		}
	}
	return operateDistsAsSamples(old_dist, new_dist, "*")
}

func divideDists(old_dist Dist, new_dist Dist) (Dist, error) {

	switch o := old_dist.(type) {
	case Lognormal:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				// to do: check division by zero
				if n.high == 0 || n.low == 0 {
					fmt.Println("Error: Can't divide by 0.0")
					return nil, errors.New("Error: division by zero")
				}
				return multiplyLogDists(o, Lognormal{low: 1.0 / n.high, high: 1.0 / n.low}), nil
			case Scalar:
				// to do: check division by zero
				if n == 0.0 {
					fmt.Println("Error: Can't divide by 0.0")
					return nil, errors.New("Error: division by zero scalar")
				}
				return multiplyLogDists(o, Lognormal{low: 1.0 / float64(n), high: 1.0 / float64(n)}), nil
			default:
				return operateDistsAsSamples(old_dist, new_dist, "/")
			}
		}
	case Scalar:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDists(Lognormal{low: float64(o), high: float64(o)}, Lognormal{low: 1.0 / n.high, high: 1.0 / n.low}), nil
			case Scalar:
				// to do: check division by zero
				if n == 0.0 {
					fmt.Println("Error: Can't divide by 0.0")
					return nil, errors.New("Error: division by zero scalar")
				}
				return Scalar(float64(o) / float64(n)), nil
			default:
				return operateDistsAsSamples(old_dist, new_dist, "/")
			}
		}
	default:
		return operateDistsAsSamples(old_dist, new_dist, "/")
	}
}

// Generic distribution operations
func operateDists(old_dist Dist, new_dist Dist, op string) (Dist, error) {
	switch op {
	case "*":
		return multiplyDists(old_dist, new_dist)
	case "/":
		return divideDists(old_dist, new_dist)
	case "+":
		return operateDistsAsSamples(old_dist, new_dist, "+")
	case "-":
		return operateDistsAsSamples(old_dist, new_dist, "-")
	default:
		return nil, printAndReturnErr("Can't combine distributions in this way")
	}
}

/* Parser and repl */
func parseWordsErr(err_msg string) (string, Dist, error) {
	return "", nil, printAndReturnErr(err_msg)
}
func parseWordsIntoOpAndDist(words []string) (string, Dist, error) {

	op := ""
	var dist Dist

	switch words[0] {
	case "*", "/", "+", "-":
		op = words[0]
		words = words[1:]
	default:
		op = "*" // later, change the below to
	}

	switch len(words) {
	case 0:
		return parseWordsErr("Operator must have operand; can't operate on nothing")
	case 1:
		single_float, err := strconv.ParseFloat(words[0], 64) // abstract this away to search for K/M/B/T/etc.
		if err != nil {
			return parseWordsErr("Trying to operate on a scalar, but scalar is neither a float nor an assigned variable")
		}
		dist = Scalar(single_float)
	case 2:
		new_low, err1 := strconv.ParseFloat(words[0], 64)
		new_high, err2 := strconv.ParseFloat(words[1], 64)
		if err1 != nil || err2 != nil {
			return parseWordsErr("Trying to operate by a distribution, but distribution is not specified as two floats")
		}
		dist = Lognormal{low: new_low, high: new_high}
	case 3:
		if words[0] == "beta" || words[0] == "b" {
			a, err1 := strconv.ParseFloat(words[1], 64)
			b, err2 := strconv.ParseFloat(words[2], 64)
			if err1 != nil || err2 != nil {
				return parseWordsErr("Trying to specify a beta distribution? Try beta 1 2")
			}
			dist = Beta{a: a, b: b}
		} else {
			return parseWordsErr("Input not understood or not implemented yet")
		}
	default:
		return parseWordsErr("Input not understood or not implemented yet")
	}
	return op, dist, nil

}

func main() {
	reader := bufio.NewReader(os.Stdin)
	var old_dist Dist = INIT_DIST

replForLoop:
	for {

		new_line, _ := reader.ReadString('\n')
		words := strings.Split(strings.TrimSpace(new_line), " ")

		switch {
		case strings.TrimSpace(new_line) == "": /* Empty line case */
			continue replForLoop
		case words[0] == "exit" || words[0] == "e":
			os.Exit(0)
		case words[0] == "help" || words[0] == "h":
			fmt.Println(HELP_MSG)
			continue replForLoop
		case words[0] == "debug" || words[0] == "d":
			fmt.Printf("%v", old_dist)
			continue replForLoop
		case words[0] == "clear" || words[0] == "c" || words[0] == ".":
			old_dist = INIT_DIST
			fmt.Println()
			continue replForLoop
		}

		op, new_dist, err := parseWordsIntoOpAndDist(words)
		if err != nil {
			continue replForLoop
		}
		combined_dist, err := operateDists(old_dist, new_dist, op)
		if err != nil {
			continue replForLoop
		}
		old_dist = combined_dist
		prettyPrintDist(old_dist)
	}
}
