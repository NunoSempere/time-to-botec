package main

import "fmt"
import rand "math/rand/v2"

var r = rand.New(rand.NewPCG(1, 2))

func sample_unit_normal() float64 {
	return 1.0
}

func main() {
	fmt.Println("Hello world!")
	fmt.Printf("%v\n", r.Float64())
	fmt.Printf("%v\n", r.NormFloat64())
}
