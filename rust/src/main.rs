use rand_core::SeedableRng;
use rand_distr::{Distribution, LogNormal, Uniform};
use rand_pcg::Pcg64Mcg;

fn sample_to(low: f64, high: f64, mut rng: &mut Pcg64Mcg) -> f64 {
    let normal90 = 1.6448536269514727; // change to global const later
    let loglow = low.ln();
    let loghigh = high.ln();
    let normal_mean = (loghigh + loglow)/2.0;
    let normal_std = (loghigh - loglow) / (2.0 * normal90);

    let lognormal = LogNormal::new(normal_mean, normal_std).unwrap(); 
    let x = lognormal.sample(&mut rng);
    // https://docs.rs/rand_distr/latest/src/rand_distr/normal.rs.html#232-236
    return x; 
}

fn model(mut rng: &mut Pcg64Mcg) -> f64 {
    let p_a = 0.8;
    let p_b = 0.5;
    let p_c = p_a * p_b;

    let ws = [1.0 - p_c, p_c / 2.0, p_c / 4.0, p_c / 4.0];

    let uniform = Uniform::new(0.0, 1.0); /* I don't understand why this doesn't need to be unwrapped, unlike normal below */
    let p: f64  = uniform.sample(&mut rng);
    if p < ws[0] {
        return 0.0;
    } else if p < (ws[0] + ws[1]) {
        return 1.0;
    } else if p < (ws[0] + ws[1] + ws[2]) {
        let x = sample_to(1.0, 3.0, rng);
        return x;
    } else {
        let x = sample_to(2.0, 10.0, rng);
        return x;
    }
}

fn main() {
    let mut rng = Pcg64Mcg::seed_from_u64(1);

    let mut mean = 0.0;
    let n_samples = 1_000_000;
    for _ in 0..n_samples { 
        let x = model(&mut rng);
        // println!("Sample: {}", x);
        mean += x;
    }
    mean = mean/(n_samples as f64);
    println!{"Mean: {}", mean};
}
