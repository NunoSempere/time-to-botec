package main

import "fmt"
import rand "math/rand/v2"

func main() {
	r := rand.New(rand.NewPCG(1, 2))
	fmt.Println("Hello world!")
	fmt.Printf("%v\n", r.Float64())
}
