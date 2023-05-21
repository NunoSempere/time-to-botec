import std/math
import std/random

# randomize()

## Basic math functions
proc factorial(n: int): int = 
  if n == 0 or n < 0: 
    return 1
  else:
    return n * factorial(n - 1)

proc sine(x: float): float = 
  let n = 8
  # ^ Taylor will converge really quickly
  # notice that the factorial of 17 is 
  # already pretty gigantic
  var acc = 0.0
  for i in 0..n:
    var k = 2*i + 1
    var taylor = pow(-1, i.float) * pow(x, k.float) / factorial(k).float 
    acc = acc + taylor
  return acc 

# Helpers for calculating the log function

## Arithmetic-geomtric mean
proc ag(x: float, y: float): float = 
  let n = 100
  var a = (x + y)/2.0
  var b = sqrt(x * y)
  for i in 0..n:
    let temp = a
    a = (a+b)/2.0
    b = sqrt(b*temp)
  return a

## Find m such that x * 2^m > 2^100

proc log_slow(x: float): float = 
  # See: <https://en.wikipedia.org/wiki/Natural_logarithm#High_precision>
  var y = x - 1 
  let n = 100000000
  var acc = 0.0
  for i in 1..n:
    let taylor = pow(-1.0, float(i+1)) * pow(y, i.float) / i.float
    acc = acc + taylor
  return acc 

proc log(x: float): float = 
  return 1 

## Test these functions
echo factorial(5)
echo sine(1.0)
echo log(1.0)
echo log(2.0)

## Distribution functions
proc normal(): float = 
  let u1 = rand(1.0)
  let u2 = rand(1.0)
  let z = 1
  # see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form
