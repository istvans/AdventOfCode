/// 2024 Day 3
use regex::Regex;
use std::iter::zip;
use std::{io::BufRead, path::Path};

use raoc_2024::get_reader;

#[derive(Debug)]
struct Multiplication {
    first: u32,
    second: u32,
}

impl Multiplication {
    pub fn pattern() -> Regex {
        Regex::new(r"mul\((?<first>\d+),(?<second>\d+)\)").unwrap()
    }

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
    let mul_pattern = Multiplication::pattern();
    for line in reader.lines() {
        let line = line.unwrap();
        let multiplications = mul_pattern
            .captures_iter(&line)
            .map(|m| Multiplication::new(&m["first"], &m["second"]))
            .collect::<Vec<Multiplication>>();
        result += multiplications.iter().map(|m| m.execute()).sum::<u32>();
    }
    result
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut result = 0;
    let mul_pattern = Multiplication::pattern();
    for line in reader.lines() {
        let line = line.unwrap();
        let mut dos = line
            .match_indices("do()")
            .map(|x| x.0)
            .collect::<Vec<usize>>();
        // At the beginning of the program, mul instructions are enabled.
        dos.insert(0, 0);
        let donts = line
            .match_indices("don't()")
            .map(|x| x.0)
            .collect::<Vec<usize>>();
        println!("{}", line);
        for (i, (do_index, dont_index)) in zip(dos, donts).enumerate() {
            println!("{} {}", i, &line[do_index..dont_index])
        }
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

    let answer = part02(&path);
    println!(
        "Handle the new instructions; \
         what do you get if you add up all of the results of \
         just the enabled multiplications? {}",
        answer
    );
}
