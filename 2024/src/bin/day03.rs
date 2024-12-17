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

fn calculate_line_part02(line: &str) -> u32 {
    let mut dos = line
        .match_indices("do()")
        .map(|x| x.0)
        .collect::<Vec<usize>>();
    // At the beginning of the program, mul instructions are enabled.
    dos.insert(0, 0);
    let mut donts = line
        .match_indices("don't()")
        .map(|x| x.0)
        .collect::<Vec<usize>>();
    // Make our algo work with an endstop.
    donts.push(line.len());

    println!("{:?}", &line[0..line.len()]);

    let mut result = 0;
    if donts.is_empty() {
        result += multiply(&line);
    } else {
        let mut donts_index = 0;
        let mut dos_index = 0;
        while dos_index < dos.len() {
            let mut do_index = dos[dos_index];
            let dont_index = donts[donts_index];
            println!("do:{:?} dont:{:?}", do_index, dont_index);

            if do_index < dont_index {
                result += multiply(&line[do_index..dont_index]);

                while do_index <= dont_index {
                    dos_index += 1;
                    if dos_index >= dos.len() {
                        break;
                    }
                    do_index = dos[dos_index];
                    println!("after mul => do:{:?} dont:{:?}", do_index, dont_index);
                }
                donts_index += 1;
                donts_index %= donts.len();
            } else {
                dos_index += 1;
            }
        }
    }

    result
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut result = 0;
    for line in reader.lines() {
        let line = line.unwrap();
        result += calculate_line_part02(&line);
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

//===================================================================
//================================= Tests ===========================
//===================================================================

// Part 2 tests

#[test]
fn test_no_dos_or_donts() {
    let result = calculate_line_part02("xyzmul(2,3)kdskdskdsmul(4,5)");
    assert_eq!(result, (2 * 3) + (4 * 5));
}

#[test]
fn test_dont_all() {
    let result = calculate_line_part02("don't()xyzmul(2,3)kdskdskdsmul(4,5)");
    assert_eq!(result, 0);
}

#[test]
fn test_dont_in_the_middle() {
    let result = calculate_line_part02("xyzmul(2,3)kdskdon't()skdsmul(4,5)");
    assert_eq!(result, 2 * 3);
}

#[test]
fn test_dont_do_dont() {
    let result = calculate_line_part02(
        "xmul(1,2)somegarbage^don't()_mul(5,6)+mul(32,64](mul(11,8)undo()?mul(3,2))\
        someothergarbagedon't()foobarmul(6, 4)donnodo()xyzmul(4, 5)",
    );
    assert_eq!(result, (1 * 2) + (3 * 2) + (4 * 5));
}

#[test]
fn test_example() {
    let result = calculate_line_part02(
        "xmul(2,4)&mul[3,7]!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))",
    );
    assert_eq!(result, (2 * 4) + (8 * 5));
}

#[test]
fn test_multi_dont() {
    let result = calculate_line_part02(
        "do()xmul(2,4)&mul[3,7]don't()!^don't()_mul(5,5)+mul(32,64](mul(11,8)undo()?mul(8,5))",
    );
    assert_eq!(result, (2 * 4) + (8 * 5));
}

#[test]
fn test_multi_do() {
    let result =
        calculate_line_part02("do()xmul(2,4)&mul[3,7]do()_mul(3,2)+don't()mul(32,64](mul(11,8)");
    assert_eq!(result, (2 * 4) + (3 * 2));
}
