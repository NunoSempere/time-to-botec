use rand_core::SeedableRng;
use rand_pcg::Pcg64Mcg;
use rand_distr::{Normal, Distribution};

// use rand::thread_rng;
// use rand::prelude::*

fn model(rng: Pcg64Mcg) -> f64 {
    let x = 1.0;
    let normal = Normal::new(2.0, 3.0).unwrap();
    let v = normal.sample(&mut rng);
    v * x
}

fn main() {
    println!("Hello, world!");

    let mut rng = Pcg64Mcg::seed_from_u64(1);
    // mean 2, standard deviation 3
    let v = model(rng);
    println!("{} is from a N(2, 9) distribution", v);
    let z = model(rng);
    println!("{} is from a N(2, 9) distribution", z);
}


