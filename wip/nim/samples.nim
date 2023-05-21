import std/math
import std/sugar
import std/random
import std/sequtils

randomize()

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
## echo factorial(5)
## echo sine(1.0)
## echo log(0.1)
## echo log(2.0)
## echo log(3.0)
## echo pow(2.0, 32.float)
   
## Distribution functions

## Normal
## <https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form>
proc ur_normal(): float = 
  let u1 = rand(1.0)
  let u2 = rand(1.0)
  let z = sqrt(-2.0 * log(u1)) * sine(2 * PI * u2)
  return z

proc normal(mean: float, sigma: float): float = 
  return (mean + sigma * ur_normal())

proc lognormal(logmean: float, logsigma: float): float =
  let answer = pow(E, normal(logmean, logsigma))
  return answer

proc to(low: float, high: float): float = 
  let normal95confidencePoint = 1.6448536269514722
  let loglow = log(low)
  let loghigh = log(high)
  let logmean = (loglow + loghigh)/2
  let logsigma = (loghigh - loglow) / (2.0 * normal95confidencePoint);
  return lognormal(logmean, logsigma)

## echo ur_normal()
## echo normal(10, 20)
## echo lognormal(2, 4)
## echo to(10, 90)

## Manipulate samples

proc make_samples(f: () -> float, n: int): seq[float] = 
  result = toSeq(1..n).map(_ => f())
  return result

proc mixture(sxs: seq[seq[float]], ps: seq[float], n: int): seq[float] = 
  
  assert sxs.len == ps.len

  var ws: seq[float]
  var sum = 0.0
  for i, p in ps:
    sum = sum + p
    ws.add(sum)
  ws = ws.map(w => w/sum)
  
  proc get_mixture_sample(): float = 
    let r = rand(1.0)
    var i = ws.len - 1
    for j, w in ws:
      if r < w:
        i = j
        break
    ## only occasion when ^ doesn't assign i
    ## is when r is exactly 1
    ## which would correspond to choosing the last item in ws
    ## which is why i is initialized to ws.len
    let xs = sxs[i]
    let l = xs.len-1
    let k = rand(0..l)
    return xs[k]
  
  return toSeq(1..n).map(_ => get_mixture_sample())

## Actual model

let n = 1000000

let p_a = 0.8
let p_b = 0.5
let p_c = p_a * p_b

let weights = @[ 1.0 - p_c, p_c/2.0, p_c/4.0, p_c/4.0 ]

let fs = [ () => 0.0, () => 1.0, () => to(1.0, 3.0), () => to(2.0, 10.0)  ]
let dists = fs.map(f => make_samples(f, n))
let result = mixture(dists, weights, n)
let mean_result = foldl(result, a + b, 0.0) / float(result.len)

# echo result
echo mean_result
