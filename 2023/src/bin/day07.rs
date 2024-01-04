/// 2023 Day 7
use std::io::BufRead;
use std::path::Path;

use raoc_2023::{get_reader, print_part01_header, print_part02_header};

fn part01(input: &Path) -> u32 {
    let reader = get_reader(&input);
    for line in reader.lines() {
        let line = line.unwrap();
        println!("{}", line);
    }
    0 // TODO implement
}

fn main() {
    let input = Path::new("./inputs/day07");

    print_part01_header();
    let answer = part01(&input);
    println!(
        "Find the rank of every hand in your set. What are the total winnings? {}",
        answer
    );
}
