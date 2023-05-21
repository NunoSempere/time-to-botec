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

## Log function

## Old implementation using Taylor expansion
proc log_slow(x: float): float = 
  var y = x - 1 
  let n = 100000000
  var acc = 0.0
  for i in 1..n:
    let taylor = pow(-1.0, float(i+1)) * pow(y, i.float) / i.float
    acc = acc + taylor
  return acc 

## New implementation
## <https://en.wikipedia.org/wiki/Natural_logarithm#High_precision>

## Arithmetic-geomtric mean
proc ag(x: float, y: float): float = 
  let n = 128 # just some high number
  var a = (x + y)/2.0
  var b = sqrt(x * y)
  for i in 0..n:
    let temp = a
    a = (a+b)/2.0
    b = sqrt(b*temp)
  return a

## Find m such that x * 2^m > 2^precision/2
proc find_m(x:float): float = 
  var m = 0.0;
  let precision = 64 # bits
  let c = pow(2.0, precision.float / 2.0)
  while x * pow(2.0, m) < c:
    m = m + 1
  return m

proc log(x: float): float = 
  let m = find_m(x)
  let s = x * pow(2.0, m)
  let ln2 = 0.6931471805599453
  return ( PI / (2.0 * ag(1, 4.0/s)) ) - m * ln2

## Test these functions
echo factorial(5)
echo sine(1.0)
echo log(1.0)
echo log(2.0)
echo log(3.0)
echo pow(2.0, 32.float)

## Distribution functions
proc normal(): float = 
  let u1 = rand(1.0)
  let u2 = rand(1.0)
  let z = 1
  # see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form
