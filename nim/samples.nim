import std/math
import std/sugar
import std/random
import std/sequtils

randomize()

## Distribution functions

## Normal
## <https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form>
proc ur_normal(): float = 
  let u1 = rand(1.0)
  let u2 = rand(1.0)
  let z = sqrt(-2.0 * ln(u1)) * sin(2 * PI * u2)
  return z

proc normal(mean: float, sigma: float): float = 
  return (mean + sigma * ur_normal())

proc lognormal(logmean: float, logsigma: float): float =
  let answer = pow(E, normal(logmean, logsigma))
  return answer

proc to(low: float, high: float): float = 
  let normal95confidencePoint = 1.6448536269514722
  let loglow = ln(low)
  let loghigh = ln(high)
  let logmean = (loglow + loghigh)/2
  let logsigma = (loghigh - loglow) / (2.0 * normal95confidencePoint);
  return lognormal(logmean, logsigma)

## echo ur_normal()
## echo normal(10, 20)
## echo lognormal(2, 4)
## echo to(10, 90)

## Manipulate samples

proc mixture(fs: seq[proc (): float{.nimcall.}], ps: seq[float], n: int): seq[float] = 
  
  assert fs.len == ps.len

  var ws: seq[float]
  var sum = 0.0
  for i, p in ps:
    sum = sum + p
    ws.add(sum)
  ws = ws.map(w => w/sum)
  
  var samples: seq[float]
  let rs = toSeq(1..n).map(_=>rand(1.0))
  for i in 0..(n-1):
    let r = rs[i]
    var j = ws.len - 1
    for k, w in ws:
      if r < w:
        j = k
        break
    ## only occasion when ^ doesn't assign j
    ## is when r is exactly 1
    ## which would correspond to choosing the last item in ws
    ## which is why j is initialized to ws.len - 1
    let f = fs[j]
    samples.add(f())
  return samples


## Actual model

let n = 1000000

let p_a = 0.8
let p_b = 0.5
let p_c = p_a * p_b

let weights = @[ 1.0 - p_c, p_c/2.0, p_c/4.0, p_c/4.0 ]

let fs = @[ proc (): float = 0.0, proc (): float = 1.0, proc (): float = to(1.0, 3.0), proc (): float = to(2.0, 10.0)]
let result = mixture(fs, weights, n)
let mean_result = foldl(result, a + b, 0.0) / float(result.len)

# echo result
echo mean_result
