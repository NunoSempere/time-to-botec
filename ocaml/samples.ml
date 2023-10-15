(* Constants *)
let pi = acos (-1.)
let normal_95_ci_length = 1.6448536269514722

(* List manipulation helpers *)
let sumFloats xs = List.fold_left(fun acc x -> acc +. x) 0.0 xs

let normalizeXs xs = 
  let sum_xs = sumFloats xs in 
  List.map(fun x -> x /. sum_xs) xs

let cumsumXs xs = 
  let _, cum_sum = List.fold_left(fun (sum, ys) x -> 
    let new_sum = sum +. x in
    new_sum, ys @ [new_sum]
  ) (0.0, []) xs in
  cum_sum

let rec nth xs (n: int) = 
  match xs with
  | [] -> Error "nth function finds no match"
  | y :: ys -> if n = 0 then Ok(y) else nth ys (n-1)
  (* 
     Note that this is O(n) access.
     That is the cost of using the nice match syntax,
     which is not possible with OCaml arrays
  *)

let findIndex xs test = 
  let rec recursiveHelper ys i = 
    match ys with
      | [] -> Error "findIndex doesn't find an index"
      | z :: zs -> if test z then Ok(i) else recursiveHelper zs (i+1)
    in
  recursiveHelper xs 0

let unwind xs = 
  let rec tailRecursiveHelper ys acc =
    match ys with
    | [] -> Ok(acc)
    | Error e :: _ -> Error e
    | Ok(y) :: ys -> tailRecursiveHelper ys (y :: acc) 
  in 
  tailRecursiveHelper xs []
  
let unwindSum xs = 
  let rec tailRecursiveHelper ys sum =
    match ys with
    | [] -> Ok(sum)
    | Error e :: _ -> Error e
    | Ok(y) :: ys -> tailRecursiveHelper ys (y +. sum) 
  in 
  tailRecursiveHelper xs 0.0

(* Array helpers *)
let unwindSumArray xs = 
  Array.fold_left(fun acc x -> 
    (
      match acc, x with
      | Error e, _ -> Error e
      | _, Error e -> Error e
      | Ok(sum), Ok(y) -> Ok(sum +. y)
    )
  ) (Ok 0.0) xs

let sumFloats xs = List.fold_left(fun acc x -> acc +. x) 0.0 xs

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
  let logstd = (loghigh -. loglow) /. (2.0 *. normal_95_ci_length ) in
  sampleLognormal logmean logstd

let mixture (samplers: (unit -> float) list) (weights: float list): (float, string) result = 
   if (List.length samplers <> List.length weights) 
    then Error "in mixture function, List.length samplers != List.length weights"
  else
    let normalized_weights = normalizeXs weights in
    let cumsummed_normalized_weights = cumsumXs normalized_weights in
    let p = sampleZeroToOne () in
    let chosenSamplerIndex = findIndex cumsummed_normalized_weights (fun x -> p < x) in
    let sampler = match chosenSamplerIndex with
      | Error e -> Error e
      | Ok(i) -> nth samplers i
    in
    let sample = match sampler with
      | Error e -> Error e
      | Ok(f) -> Ok(f ())
    in 
    sample

let () =
  let sample0 () = 0. in 
  let sample1 () = 1. in
  let sampleFew () = sampleTo 1. 3. in
  let sampleMany () = sampleTo 2. 10. in 
  let p1 = 0.8 in 
  let p2 = 0.5 in 
  let p3 = p1 *. p2 in 
  let weights = [ 1. -. p3; p3 /. 2.; p3 /. 4.; p3/. 4. ] in
  let sampler () = mixture [ sample0; sample1; sampleFew; sampleMany ] weights in
  let n = 1_000_000 in 
  let samples = Array.init n (fun _ -> sampler ()) in 
  match unwindSumArray samples with
    | Error err -> Printf.printf "Error %s\n" err
    | Ok(sum) -> (
        let mean = sum /. float_of_int(n) in 
        Printf.printf "Mean: %f\n" mean
      )
