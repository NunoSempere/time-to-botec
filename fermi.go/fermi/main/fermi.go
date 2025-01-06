package main

import (
	"bufio"
	"flag"
	"fmt"
	"git.nunosempere.com/NunoSempere/fermi/sample"
	"math"
	"os"
	"strings"
)

/* Types and interfaces */
type Stack struct {
	old_dist Dist
	vars     map[string]Dist
}

type Dist interface {
	Sampler(int, sample.State) float64
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
func (p Scalar) Sampler(i int, r sample.State) float64 {
	return float64(p)
}

func (ln Lognormal) Sampler(i int, r sample.State) float64 {
	return sample.To(ln.low, ln.high, r)
}

func (beta Beta) Sampler(i int, r sample.State) float64 {
	return sample.Beta(beta.a, beta.b, r)
}

func (fs FilledSamples) Sampler(i int, r sample.State) float64 {
	// This is a bit subtle, because sampling from FilledSamples randomly iteratively converges
	// to something different than the initial distribution
	// So instead we have an i parameter.
	return fs.xs[i]
}

/* Constants */
const HELP_MSG = "1. Grammar:\n" +
	"  Operation | Variable assignment | Special\n" +
	"    Operation:                             operator operand\n" +
	"          operator:                        (empty) | * | / | + | -\n" +
	"          operand:                         scalar | lognormal | beta | variable\n" +
	"            lognormal:                     low high\n" +
	"            beta:                          beta alpha beta\n" +
	"    Variable assignment:                   =: variable_name\n" +
	"    Variable assignment and clear stack:   =. variable_name\n" +
	"    Special commands: \n" +
	"         Comment:                          # this is a comment\n" +
	"         Summary stats:                    stats\n" +
	"         Clear stack:                      clear | c | .\n" +
	"         Print debug info:                 debug | d\n" +
	"         Print help message:               help  | h\n" +
	"         Start additional stack:           operator (\n" +
	"         Return from additional stack      )\n" +
	"         Exit:                             exit  | e\n" +
	"  Examples: \n" +
	"    + 2\n" +
	"    / 2.5\n" +
	"    * 1 10 (interpreted as lognormal)\n" +
	"    + 1 10\n" +
	"    * beta 1 10\n" +
	"    1 10 (multiplication taken as default operation)\n" +
	"    =: x\n" +
	"    .\n" +
	"    1 100\n" +
	"    + x\n" +
	"    # this is a comment\n" +
	"    * 1 12 # this is an operation followed by a comment\n" +
	"    * (\n" +
	"    1 10\n" +
	"    + beta 1 100\n" +
	"    )\n" +
	"    =. y\n" +
	"    mx x 1 y 2.33\n" +
	"    + mx x 30% y 70%\n" +
	"    exit\n" +
	"\n" +
	"2. Command flags:\n" +
	"  -echo\n" +
	"        Specifies whether inputs should be echoed back. Useful if reading from a file\n." +
	"  -f string\n" +
	"        Specifies a file with a model to run\n" +
	"  -n int\n" +
	"        Specifies the number of samples to draw when using samples (default 100000)\n" +
	"  -h    Shows help message\n"

const NORMAL90CONFIDENCE = 1.6448536269514727
const INIT_DIST Scalar = Scalar(1)

var N_SAMPLES = 100_000

/* Operations */
// Generic operations with samples
func operateDistsAsSamples(dist1 Dist, dist2 Dist, op string) (Dist, error) {

	xs := sample.Serially(dist1.Sampler, N_SAMPLES)
	ys := sample.Serially(dist2.Sampler, N_SAMPLES)
	zs := make([]float64, N_SAMPLES)

	for i := 0; i < N_SAMPLES; i++ {
		switch op {
		case "*":
			zs[i] = xs[i] * ys[i]
		case "/":
			if ys[0] != 0 {
				zs[i] = xs[i] / ys[i]
			} else {
				// fmt.Println("Error: When dividing as samples, division by zero")
				return nil, FermiError{Code: "Division by zero", Msg: "When operating on samples, division by zero"}
			}
		case "+":
			zs[i] = xs[i] + ys[i]
		case "-":
			zs[i] = xs[i] - ys[i]
		default:
			// fmt.Println("Error: Operation not recognized")
			return nil, FermiError{Code: "Unknown operation", Msg: "When operating on samples, operation not recognized"}
		}
	}

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

func multiplyLogDistAndScalar(l Lognormal, s Scalar) (Dist, error) {
	if s == 0.0 {
		return Scalar(0.0), nil
	} else if s < 0.0 {
		return operateDistsAsSamples(s, l, "*")
	} else {
		return multiplyLogDists(l, Lognormal{low: float64(s), high: float64(s)}), nil
	}
}

func multiplyDists(old_dist Dist, new_dist Dist) (Dist, error) {

	switch o := old_dist.(type) {
	case Lognormal:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDists(o, n), nil
			case Scalar:
				return multiplyLogDistAndScalar(o, n)
			}
		}
	case Scalar:
		{
			switch o {
			case 1.0:
				return new_dist, nil
			case 0.0:
				return Scalar(0.0), nil
			}
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDistAndScalar(n, o)
			case Scalar:
				return Scalar(float64(o) * float64(n)), nil
			}
		}
	}
	return operateDistsAsSamples(old_dist, new_dist, "*")
}

func divideDists(old_dist Dist, new_dist Dist) (Dist, error) {

	switch o := old_dist.(type) {
	// I miss you, OCaml switches
	case Lognormal:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				if n.high == 0 || n.low == 0 {
					return nil, FermiError{Code: "Division by zero", Msg: "When operating two lognormals, one of the parameters is zero, which would result in division by zero"}
				}
				return multiplyLogDists(o, Lognormal{low: 1.0 / n.high, high: 1.0 / n.low}), nil
			case Scalar:
				if n == 0.0 {
					return nil, FermiError{Code: "Division by zero", Msg: "When operating a lognormal with a scalar, trying to divide but the scalar is zero"}
				}
				return multiplyLogDistAndScalar(o, Scalar(1.0/n))
			}
		}
	case Scalar:
		{
			switch n := new_dist.(type) {
			case Lognormal:
				return multiplyLogDistAndScalar(Lognormal{low: 1.0 / n.high, high: 1.0 / n.low}, o)
			case Scalar:
				if n == 0.0 {
					return nil, FermiError{Code: "Division by zero", Msg: "When operating two scalars, trying to divide but the divisor is is zero"}
				}
				return Scalar(float64(o) / float64(n)), nil
			}
		}
	}
	return operateDistsAsSamples(old_dist, new_dist, "/")
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
		return nil, FermiError{Code: "Unknown operation", Msg: "When operating distributions, operation not recognized"}
	}
}

/* Mixtures */
func parseMixture(words []string, vars map[string]Dist) (Dist, error) {
	// mx, mix, var weight var weight var weight ...
	// Check syntax
	switch {
	case len(words) < 1:
		return nil, FermiError{Code: "Not a mixture", Msg: "Input can't be a mixture, since it doesn't have enough words"}
	case words[0] != "mx":
		return nil, FermiError{Code: "Not a mixture", Msg: "Input can't be a mixture, since it is not preceded by the mx keyword"}
	case len(words)%2 != 1:
		return nil, FermiError{Code: "Not a mixture", Msg: "When parsing a mixture, input doesn't have equal number of variables and weights. \nMixture syntax: \nmx x 20% y 70% z 10%\ni.e.: mx var weight var2 weight2 ... var_n weight_n"}
	case len(words) < 5:
		return nil, FermiError{Code: "Not a mixture", Msg: "When parsing a mixture, not enough words. \nMixture syntax: \nmx x 20% y 70% z 10%\ni.e.: mx var weight var2 weight2 ... var_n weight_n"}
	}
	words = words[1:] // crop "mx" at the beginning

	var fs []func(int, sample.State) float64
	var weights []float64

	for i, word := range words {
		if i%2 == 0 {
			dist, exists := vars[word]
			if !exists {
				return nil, FermiError{Code: "Not a mixture variable", Msg: "When parsing a mixture, expected mixture variable but didn't get a variable. \nMixture syntax: \nmx x 2.5 y 8 z 10\ni.e.: mx var weight var2 weight2 ... var_n weight_n"}
			}
			f := dist.Sampler
			fs = append(fs, f)
		} else {
			weight, err := ParseFloat(word)
			if err != nil {
				return nil, FermiError{Code: "Not a mixture weight", Msg: "When parsing a mixture, expected mixture weight but didn't get a variable. \nMixture syntax: \nmx x 2.5 y 8 z 10\ni.e.: mx var weight var2 weight2 ... var_n weight_n"}
			}
			weights = append(weights, weight)
		}
	}
	// Sample from mixture
	xs, err := sample.Mixture_serially_from_samplers(fs, weights, N_SAMPLES)
	if err != nil {
		return nil, PrintAndReturnErr(err.Error())
	}
	return FilledSamples{xs: xs}, nil
}

/* Parser and repl */
func parseWordsIntoOpAndDist(words []string, vars map[string]Dist) (string, Dist, error) {
	parseWordsErr := func(msg string) (string, Dist, error) {
		return "", nil, FermiError{Code: "Malformed input", Msg: "When parsing words into operator and distribution: " + msg}
	}

	op := ""
	var dist Dist

	switch words[0] {
	case "*", "/", "+", "-":
		op = words[0]
		words = words[1:]
	default:
		op = "*"
	}

	switch {
	case len(words) == 0:
		return parseWordsErr("Operator must have operand; can't operate on nothing")
	case len(words) == 1:
		var_word, var_word_exists := vars[words[0]]
		single_float, err1 := ParseFloat(words[0]) // abstract this away to search for K/M/B/T/etc.
		switch {
		case var_word_exists:
			dist = var_word
		case err1 == nil:
			dist = Scalar(single_float)
		case err1 != nil && !var_word_exists:
			return parseWordsErr("Trying to operate on a scalar, but scalar is neither a float nor an assigned variable")
		}
	case len(words) == 2:
		new_low, err1 := ParseFloat(words[0])
		new_high, err2 := ParseFloat(words[1])
		switch {
		case err1 != nil || err2 != nil:
			return parseWordsErr("Trying to operate by a distribution, but distribution is not specified as two floats")
		case new_low <= 0.0 || new_high <= 0.0:
			return parseWordsErr("Trying to parse two floats as a lognormal, but the two floats must be greater than 0")
		case new_low == new_high:
			return parseWordsErr("Trying to parse two floats as a lognormal, but the two floats must be different. Try a single scalar instead?")
		case new_low > new_high:
			return parseWordsErr("Trying to parse two floats as a lognormal, but the first number is larger than the second number")
		}
		dist = Lognormal{low: new_low, high: new_high}
	case len(words) == 3:
		switch {
		case words[0] == "beta" || words[0] == "b":
			a, err1 := ParseFloat(words[1])
			b, err2 := ParseFloat(words[2])
			if err1 != nil || err2 != nil {
				return parseWordsErr("Trying to specify a beta distribution? Try beta 1 2")
			}
			dist = Beta{a: a, b: b}
		default:
			return parseWordsErr("Input not understood or not implemented yet")
		}
	case len(words) >= 4: // four or more words
		if words[0] == "mx" {
			dist, err := parseMixture(words, vars)
			return op, dist, err
		}
		return parseWordsErr("Input not understood or not implemented yet")
	}
	return op, dist, nil
}

/* Combine old dist and new line */
// We want this as a function (rather than just be in main)
// to be able to have parenthesis/recusion, possibly functions
func runRepl(stack Stack, reader *bufio.Reader, echo_flag *bool) Stack {
replForLoop:
	for {
		new_line, _ := reader.ReadString('\n')
		if *echo_flag {
			fmt.Print(new_line)
		}
		new_line_before_comments, _, _ := strings.Cut(new_line, "#")
		new_line_trimmed := strings.TrimSpace(new_line_before_comments)
		words := strings.Split(new_line_trimmed, " ")

		switch {

		/* Empty line */
		case strings.TrimSpace(new_line_trimmed) == "":
			continue replForLoop

		/* Special operations */
		case words[0] == "exit" || words[0] == "e":
			os.Exit(0)
		case words[0] == "help" || words[0] == "h":
			fmt.Println(HELP_MSG)
		case words[0] == "debug" || words[0] == "d":
			fmt.Printf("%v", stack)
		case words[0] == "clear" || words[0] == "c" || words[0] == ".":
			stack.old_dist = INIT_DIST
			fmt.Println()
		case words[0] == "stats" || words[0] == "s":
			PrettyPrintStats(stack.old_dist)

		/* Variable assignment */
		case words[0] == "=:" && len(words) == 2:
			stack.vars[words[1]] = stack.old_dist
			fmt.Printf("%s ", words[1])
		case words[0] == "=." && len(words) == 2:
			stack.vars[words[1]] = stack.old_dist
			fmt.Printf("%s ", words[1])
			PrettyPrintDist(stack.old_dist)
			stack.old_dist = INIT_DIST

		/* Parenthesis */
		case len(words) == 2 && (words[0] == "*" || words[0] == "+" || words[0] == "-" || words[0] == "/") && words[1] == "(":
			new_stack := runRepl(Stack{old_dist: INIT_DIST, vars: stack.vars}, reader, echo_flag)
			combined_dist, err := operateDists(stack.old_dist, new_stack.old_dist, words[0])
			if err != nil {
				PrintError(err)
			} else {
				stack.old_dist = combined_dist
			}
		case len(words) == 1 && words[0] == ")":
			return stack

		/* Bread and butter distribution operations */
		default:
			op, new_dist, err := parseWordsIntoOpAndDist(words, stack.vars)
			if err != nil {
				PrintError(err)
				PrettyPrintDist(stack.old_dist)
				continue replForLoop
			}
			combined_dist, err := operateDists(stack.old_dist, new_dist, op)
			if err != nil {
				PrintError(err)
				PrettyPrintDist(stack.old_dist)
				continue replForLoop
			}
			stack.old_dist = combined_dist
		}

		PrettyPrintDist(stack.old_dist)
	}
}

func main() {
	num_samples_flag := flag.Int("n", N_SAMPLES, "Specifies the number of samples to draw when using samples")
	filename := flag.String("f", "", "Specifies a file with a model to run. Sets the echo flag to true")
	echo_flag := flag.Bool("echo", false, "Specifies whether inputs should be echoed back. Useful if reading from a file.")
	help_flag := flag.Bool("h", false, "Shows help message")
	flag.Parse()
	N_SAMPLES = *num_samples_flag
	if *help_flag {
		fmt.Println(HELP_MSG)
	}

	var reader *bufio.Reader = nil
	if *filename != "" {
		file, err := os.Open(*filename)
		if err == nil {
			*echo_flag = true
			reader = bufio.NewReader(file)
		} else {
			fmt.Printf("Error opening filename; reading from stdin instead\n")
		}
	}
	if reader == nil {
		reader = bufio.NewReader(os.Stdin)
	}
	stack := Stack{old_dist: INIT_DIST, vars: make(map[string]Dist)}
	runRepl(stack, reader, echo_flag)

}
