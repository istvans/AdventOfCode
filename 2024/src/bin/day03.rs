/// 2024 Day 3
use regex::Regex;
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

fn multiply(range: &str) -> u32 {
    let mul_pattern = Multiplication::pattern();
    let multiplications = mul_pattern
        .captures_iter(&range)
        .map(|m| Multiplication::new(&m["first"], &m["second"]))
        .collect::<Vec<Multiplication>>();
    multiplications.iter().map(|m| m.execute()).sum::<u32>()
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut result = 0;
    for line in reader.lines() {
        let line = line.unwrap();
        result += multiply(&line);
    }
    result
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut result = 0;
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

        if donts.is_empty() {
            result += multiply(&line);
        } else {
            let mut donts_index = 0;
            let mut dos_index = 0;
            while (donts_index < donts.len()) && (dos_index < dos.len()) {
                let mut do_index = dos[dos_index];
                let dont_index = donts[donts_index];

                if do_index < dont_index {
                    result += multiply(&line[do_index..dont_index]);

                    while do_index <= dont_index {
                        dos_index += 1;
                        if dos_index >= dos.len() {
                            break;
                        }
                        do_index = dos[dos_index];
                    }
                }

                donts_index += 1;
            }
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
