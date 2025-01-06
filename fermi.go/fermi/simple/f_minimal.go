package main

import (
	"bufio"
	"errors"
	"fmt"
	"git.nunosempere.com/NunoSempere/fermi/pretty"
	"math"
	"os"
	"strconv"
	"strings"
)

type Lognormal struct {
	low  float64
	high float64
}

const HELP_MSG = "  Operation | Variable assignment | Special\n" +
	"    Operation:                             operator operand\n" +
	"          operator:                        (empty) | * | / | + | -\n" +
	"          operand:                         scalar | lognormal\n" +
	"            lognormal:                     low high\n" +
	"    Clear stack:                           . | c | clear\n" +
	"    Other special operations:              help | debug | exit\n" +
	"  Examples: \n" +
	"    / 2.5\n" +
	"    * 1 10 (interpreted as lognormal)\n" +
	"    / 1 10\n" +
	"    1 10 (multiplication taken as default operation)\n" +
	"    .\n" +
	"    exit\n"
const NORMAL90CONFIDENCE = 1.6448536269514727
const N_SAMPLES = 100_000

func prettyPrintLognormal(l Lognormal) {
	fmt.Printf("=> ")
	pretty.PrettyPrint2Floats(l.low, l.high)
	fmt.Println()
}

func printAndReturnErr(err_msg string) error {
	fmt.Println(err_msg)
	fmt.Println(HELP_MSG)
	return errors.New(err_msg)
}

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

func divideLogDists(l1 Lognormal, l2 Lognormal) (Lognormal, error) {
	if l2.high == 0 || l2.low == 0 {
		fmt.Println("Error: Can't divide by 0.0")
		return Lognormal{}, errors.New("Error: division by zero")
	}
	return multiplyLogDists(l1, Lognormal{low: 1.0 / l2.high, high: 1.0 / l2.low}), nil
}

func parseWordsErr(err_msg string) (string, Lognormal, error) {
	return "", Lognormal{}, printAndReturnErr(err_msg)
}
func parseWordsIntoOpAndDist(words []string) (string, Lognormal, error) {
	op := ""
	var dist Lognormal

	switch words[0] {
	case "*", "/":
		op = words[0]
		words = words[1:]
	default:
		op = "*"
	}

	switch len(words) {
	case 0:
		return parseWordsErr("Operator must have operand; can't operate on nothing")
	case 1:
		single_float, err := strconv.ParseFloat(words[0], 64) // abstract this away to search for K/M/B/T/etc.
		if err != nil {
			return parseWordsErr("Trying to operate on a scalar, but scalar is neither a float nor an assigned variable")
		}
		dist = Lognormal{low: single_float, high: single_float}
	case 2:
		new_low, err1 := strconv.ParseFloat(words[0], 64)
		new_high, err2 := strconv.ParseFloat(words[1], 64)
		if err1 != nil || err2 != nil {
			return parseWordsErr("Trying to operate by a distribution, but distribution is not specified as two floats")
		}
		dist = Lognormal{low: new_low, high: new_high}
	default:
		return parseWordsErr("Input not understood or not implemented yet")
	}
	return op, dist, nil
}

func main() {
	reader := bufio.NewReader(os.Stdin)
	old_dist := Lognormal{low: 1, high: 1}

replForLoop:
	for {

		new_line, _ := reader.ReadString('\n')
		words := strings.Split(strings.TrimSpace(new_line), " ")

		if strings.TrimSpace(new_line) == "" {
			continue replForLoop
		}

		op, new_dist, err := parseWordsIntoOpAndDist(words)
		if err != nil {
			continue replForLoop
		}
		switch op {
		case "*":
			old_dist = multiplyLogDists(old_dist, new_dist)
		case "/":
			result_dist, err := divideLogDists(old_dist, new_dist)
			if err != nil {
				continue replForLoop
			}
			old_dist = result_dist

		}
		prettyPrintLognormal(old_dist)
	}
}
