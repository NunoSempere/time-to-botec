import std/random
import std/math
# randomize()

proc pow2(x:float, y:int): float = 
  return pow(x, float(y))

proc sine(x: float): float = 
  let n = 100
  var result = 0.0
  for i in 0..n:
    let k = 2*n + 1
    let taylor = pow2(x, k)/ float(k)
    result = result + taylor
  return result

proc log(x: float): float = 
  var y = x - 1 
  var result = 0.0
  let n = 1000
  for i in 1..n:
    let taylor = pow2(-1, n+1) * pow2(y, n) / float(n)
    let n = 1000
    result = result + taylor
  return result

proc normal(): float = 
  let u1 = rand(1.0)
  let u2 = rand(1.0)
  let z = 1
  # see https://en.wikipedia.org/wiki/Box%E2%80%93Muller_transform#Basic_form


echo log(1.0)
