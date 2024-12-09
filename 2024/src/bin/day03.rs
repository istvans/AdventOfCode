use regex::Regex;
/// 2024 Day 3
use std::{io::BufRead, path::Path};

use raoc_2024::get_reader;

#[derive(Debug)]
struct Multiplication {
    first: u32,
    second: u32,
}

impl Multiplication {
    fn parse_from(value: &str) -> u32 {
        value.parse::<u32>().unwrap()
    }

    pub fn new(first: &str, second: &str) -> Self {
        let first = Multiplication::parse_from(first);
        let second = Multiplication::parse_from(second);
        Self { first, second }
    }

    pub fn execute(&self) -> u32 {
        self.first * self.second
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut result = 0;
    let mul_pattern = Regex::new(r"mul\((?<first>\d+),(?<second>\d+)\)").unwrap();
    for line in reader.lines() {
        let line = line.unwrap();
        let multiplications = mul_pattern
            .captures_iter(&line)
            .map(|m| Multiplication::new(&m["first"], &m["second"]))
            .collect::<Vec<Multiplication>>();
        // println!(
        //     "line {:?} contains these multiplications: {:?}",
        //     line, multiplications
        // );
        result += multiplications.iter().map(|m| m.execute()).sum::<u32>();
    }
    result
}

fn main() {
    let path = Path::new("./inputs/day03");

    let answer = part01(&path);
    println!(
        "Scan the corrupted memory for uncorrupted mul instructions. \
         What do you get if you add up all of the results of the \
         multiplications? {}",
        answer
    );
}
