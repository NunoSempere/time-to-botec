(* Constants *)
let pi = acos (-1.)
let normal_95_ci_length = 1.6448536269514722

(* Basic samplers *)
let sampleZeroToOne () : float = Random.float 1.0
let sampleStandardNormal (): float = 
  let u1 = sampleZeroToOne () in 
  let u2 = sampleZeroToOne () in 
  let z = sqrt(-2.0 *. log(u1)) *. sin(2.0 *. pi *. u2) in 
  z
let sampleNormal mean std = mean +. std *. (sampleStandardNormal ())
let sampleLognormal logmean logstd = exp(sampleNormal logmean logstd)
let sampleTo low high = 
  let loglow = log(low) in
  let loghigh = log(high) in
  let logmean = (loglow +. loghigh) /. 2.0 in
  let logstd = (loghigh -. loglow) /. (2.0 -. normal_95_ci_length ) in
  sampleLognormal logmean logstd

let () =
  Random.init 1;
  Printf.printf "%f\n" (sampleZeroToOne());
  Printf.printf "%f\n" (sampleZeroToOne());
