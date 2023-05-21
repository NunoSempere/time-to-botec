import std/math

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

echo factorial(17)
echo sine(1.0)
