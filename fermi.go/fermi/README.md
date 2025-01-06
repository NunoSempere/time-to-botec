# Fermi: A terminal DSL for Fermi estimation with distributions

This project is a minimalist, calculator-style DSL for fermi estimation. It can multiply, divide, add and subtract scalars, lognormals and beta distributions, and supports variables and mixtures. 

## Usage

```
$ fermi
5M 12M           # number of people living in Chicago
beta 1 200       # fraction of people that have a piano
30 180           # minutes it takes to tune a piano, including travel time
/ 48 52          # weeks a year that piano tuners work for
/ 5 6            # days a week in which piano tuners work
/ 6 8            # hours a day in which piano tuners work
/ 60             # minutes to an hour
=: piano_tuners
```

Here are some real-life examples: [Chance for a Russian male of fighting age of being drafted](https://x.com/NunoSempere/status/1829525844169248912), [did the startup Friend burn too much cash](https://x.com/NunoSempere/status/1818810770932568308), [how much did Nikita Bier make mentoring?](https://x.com/NunoSempere/status/1815169781907042504), [what fraction of North Korea's caloric intake is Russia supporting?](https://x.com/NunoSempere/status/1855666428835140078). In general, as a terminal guy, I've found that having zero startup cost makes creating small fermi models much cheaper, and thus happen more often.

## Build instructions 

Install the [go toolchain](https://go.dev/dl/), then:

```
git clone https://git.nunosempere.com/NunoSempere/fermi
cd fermi
make build
./fermi
# sudo make install
# fermi 
```


## Tips & tricks

- It's conceptually clearer to have all the multiplications first and then all the divisions
- For distributions between 0 and 1, consider using a beta distribution
- The default operation is multiplication

If you type "help" (or run fermi -h), you can see a small grammar and some optional command flags:

```
$ fermi -h

1. Grammar:
  Operation | Variable assignment | Special
    Operation:                             operator operand
          operator:                        (empty) | * | / | + | -
          operand:                         scalar | lognormal | beta | variable
            lognormal:                     low high
            beta:                          beta alpha beta
    Variable assignment:                   =: variable_name
    Variable assignment and clear stack:   =. variable_name
    Special commands:
         Comment:                          # this is a comment
         Summary stats:                    stats
         Clear stack:                      clear | c | .
         Print debug info:                 debug | d
         Print help message:               help  | h
         Start additional stack:           operator (
         Return from additional stack      )
         Exit:                             exit  | e
  Examples:
    + 2
    / 2.5
    * 1 10 (interpreted as lognormal)
    + 1 10
    * beta 1 10
    1 10 (multiplication taken as default operation)
    =: x
    .
    1 100
    + x
    # this is a comment
    * 1 12 # this is an operation followed by a comment
    * (
    1 10
    + beta 1 100
    )
    / 1% 
    =. y
    mx x 1 y 2.33
    + mx x 30% y 70%
    exit

2. Command flags:
  -echo
        Specifies whether inputs should be echoed back. Useful if reading from a file
.  -f string
        Specifies a file with a model to run
  -n int
        Specifies the number of samples to draw when using samples (default 100000)
  -h    Shows help message

```

### Integrations with Linux utilities

Because the model reads from standard input, you can pipe a model to it:

```
$ cat more/piano-tuners.fermi | fermi
```

In that case, you will probably want to use the echo flag as well

```
$ cat more/piano-tuners-commented.fermi | fermi -echo
```

You can make a model an executable file by running `$ chmod -x model.fermi` and then adding the following at the top, XD.

```
#!/bin/usr/fermi -f
```

You can save a session to a logfile with tee:

```
fermi | tee -a fermi.log
```

## Different levels of complexity

The mainline code has a bunch of complexity: variables, parenthesis, samples, beta distributions, number of samples, mixtures etc. In the simple/ folder:

- f_simple.go (370 lines) strips variables and parenthesis, but keeps beta distributions, samples, and addition and subtraction
- f_minimal.go (140 lines) strips everything that isn't lognormal and scalar multiplication and addition, plus a few debug options.

## Roadmap 

Done: 

- [x] Write README
- [x] Add division?
- [x] Read from file?
- [x] Save to file?
- [x] Allow comments?
  - [x] Use a sed filter? 
  - [x] Add proper comment processing
- [x] Add show more info version
- [x] Scalar multiplication and division
- [x] Think how to integrate with squiggle.c to draw samples
  - [x] Copy the time to botec go code
  - [x] Define samplers
  - [x] Call those samplers when operating on distributions that can't be operated on algebraically
- [x] Display output more nicely, with K/M/B/T
- [x] Consider the following: make this into a stack-based DSL, with:
  - [x] Variables that can be saved to and then displayed
  - [x] Other types of distributions, particularly beta distributions? => But then this requires moving to bags of samples. It could still be ~instantaneous though.
  - [x] Added bags of samples to support addition and multiplication of betas and lognormals
- [x] Figure out go syntax for
  - Maps
  - Joint types
  - Enums
- [x] Fix correlation problem, by spinning up a new randomness thing every time some serial computation is done.
- [x] Clean up error code. Right now only needed for division
- [x] Maintain *both* a more complex thing that's more featureful *and* the more simple multiplication of lognormals thing.
- [x] Allow input with K/M/T
- [x] Document parenthesis syntax
- [x] Specify number of samples as a command line option
- [x] Figure out how to make models executable, by adding a #!/bin/bash-style command at the top?
- [x] Make -n flag work
- [x] Add flag to repeat input lines (useful when reading from files)
- [x] Add percentages
- [x] Consider adding an understanding of percentages
- [x] Improve and rationalize error messages a bit
- [x] Add, then document mixture distributions

To (possibly) do:

- [ ] Consider implications of sampling strategy for operating variables.
- [ ] Fix lognormal multiplication and division by 0 or < 0
- [ ] With the -f command line option, the program doesn't read from stdin after finishing reading the file
- [ ] Add functions. Now easier to do with an explicit representation of the stack
- [ ] Think about how to draw a histogram from samples
- [ ] Dump samples to file
- [ ] Represent samples/statistics in some other way
- [ ] Perhaps use qsort rather than full sorting
- [ ] Program into a small device, like a calculator?
- [ ] Units?

Discarded: 

- [ ] ~~Think of some way of calling bc~~
