use rand::thread_rng;
use rand_distr::{Normal, Distribution};

fn main() {
    println!("Hello, world!");

    // mean 2, standard deviation 3
    let normal = Normal::new(2.0, 3.0).unwrap();
    let v = normal.sample(&mut thread_rng());
    println!("{} is from a N(2, 9) distribution", v)
}


