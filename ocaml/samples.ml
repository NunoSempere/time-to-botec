(* Constants *)
let pi = acos (-1.)

(* Basic samplers *)
let sampleZeroToOne () : float = Random.float 1.0
let sampleStandardNormal (): float = 
  let u1 = sampleZeroToOne () in 
  let u2 = sampleZeroToOne () in 
  let z = sqrt(-2.0 *. log(u1)) *. sin(2.0 *. pi *. u2) in 
  z

let () =
  Random.init 1;
  Printf.printf "%f\n" (sampleZeroToOne());
  Printf.printf "%f\n" (sampleZeroToOne());
