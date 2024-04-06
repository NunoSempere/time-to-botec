use rand_core::SeedableRng;
use rand_distr::{Distribution, Normal, Uniform};
use rand_pcg::Pcg64Mcg;

// use rand::thread_rng;
// use rand::prelude::*

fn model(mut rng: &mut Pcg64Mcg) -> f64 {
    let p_a = 0.8;
    let p_b = 0.5;
    let p_c = p_a * p_b;

    let ws = [1.0 - p_c, p_c / 2.0, p_c / 4.0, p_c / 4.0];

    let uniform = Uniform::new(0.0, 1.0);
    let p = uniform.sample(&mut rng);
    if p < ws[0] {
        return 0.0;
    } else if p < (ws[0] + ws[1]) {
        return 1.0;
    } else if p < (ws[0] + ws[1] + ws[2]) {
        let normal = Normal::new(2.0, 3.0).unwrap();
        let v = normal.sample(&mut rng);
        return v;
    } else {
        return 3.0;
    }
}

fn main() {
    println!("Hello, world!");

    let mut rng = Pcg64Mcg::seed_from_u64(1);

    let a = model(&mut rng);
    println!("Sample is {}", a);
    let b = model(&mut rng);
    println!("Sample is {}", b);
}
