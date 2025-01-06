package main

import (
	"fmt"
)

type FermiError struct {
	Code string
	Msg  string
}

func (e FermiError) Error() string {
	return e.Msg
}

func PrintError(e error) {
	if fermi_error, ok := e.(FermiError); ok {
		fmt.Printf("Error: %s: %s\n", fermi_error.Code, fermi_error.Msg)
		fmt.Printf("Type \"help\" or \"h\" to see grammar and command flags\n")
	} else {
		fmt.Printf("Error: %s\n", e.Error())
	}
}
