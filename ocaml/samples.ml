(* Constants *)
let pi = acos (-1.)
let normal_95_ci_length = 1.6448536269514722

(* Array manipulation helpers *)
let sumFloats xs = Array.fold_left(fun acc x -> acc +. x) 0.0 xs
let normalizeXs xs = 
  let sum_xs = sumFloats xs in 
  Array.map(fun x -> x /. sum_xs) xs
let cumsumXs xs = 
  let _, cum_sum = Array.fold_left(fun (sum, ys) x -> 
    let new_sum = sum +. x in
    new_sum, ys @ [new_sum]
  ) (0.0, []) xs in
  cum_sum

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

let mixture (samplers: (unit -> float) array) (weights: float array) = 
  match (Array.length samplers == Array.length weights) with
    | false -> None
    | true -> 
        let normalized_weights = normalizeXs weights in
        let cumsummed_normalized_weights = cumsumXs normalized_weights in
        let answer = 1.1 in
        Some(1.0)

let () =
  Random.init 1;
  Printf.printf "%f\n" (sampleZeroToOne());
  Printf.printf "%f\n" (sampleZeroToOne());