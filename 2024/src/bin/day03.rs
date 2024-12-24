/// 2024 Day 3
use regex::Regex;
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

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
    let mut enabler_buffer = String::new();
    // At the beginning of the program, mul instructions are enabled.
    let mut enabled = true;
    let mut enabled_switched = false;

    let pattern = Multiplication::pattern();
    let mut mul_buffer = String::new();
    let mut result = 0;
    for ch in line.chars() {
        enabler_buffer.push(ch);
        if enabler_buffer.contains("don't()") {
            if enabled {
                enabled_switched = true;
            }
            enabled = false;
            enabler_buffer.clear();
        } else if enabler_buffer.contains("do()") {
            if !enabled {
                enabled_switched = true;
            }
            enabled = true;
            enabler_buffer.clear();
        } else {
            // the enabled flag remains the same
        }

        if enabled_switched {
            mul_buffer.clear();
            enabled_switched = false;
        }

        if enabled {
            mul_buffer.push(ch);
            if let Some(m) = pattern.captures(&mul_buffer) {
                let mul = Multiplication::new(&m["first"], &m["second"]);
                result += mul.execute();
                mul_buffer.clear();
            }
        }
    }

    result
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut complete_content = String::new();
    for line in reader.lines() {
        let line = line.unwrap();
        complete_content.push_str(&line);
    }
    calculate_line_part02(&complete_content)
}

fn main() {
    let path = Path::new("./inputs/day03");

    print_part01_header();
    let answer = part01(&path);
    println!(
        "Scan the corrupted memory for uncorrupted mul instructions. \
         What do you get if you add up all of the results of the \
         multiplications? {}",
        answer
    );

    print_part02_header();
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
        "mul(1,2)somegarbage^don't()_mul(5,6)+mul(32,64](mul(11,8)undo()?mul(3,2))\
        someothergarbagedon't()foobarmul(6, 4)donnodo()xyzmul(4,5)",
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

#[test]
fn test_dont_then_immediate_do() {
    let result =
        calculate_line_part02("xmul(2,4)&mul[3,7]don't()do()_mul(3,2)+don't()mul(32,64](mul(11,8)");
    assert_eq!(result, (2 * 4) + (3 * 2));
}

#[test]
fn test_complex_zero() {
    let result =
        calculate_line_part02(".....don't()do()don't()....mul(3,2)....don't()....mul(2,5)");
    assert_eq!(result, 0);
}

#[test]
fn test_another_made_up_example() {
    let result = calculate_line_part02(
        "...don't()do()don't()don't()..do()..mul(3,2)do()....don't()...do()...don't()mul(1,2)...do()",
    );
    assert_eq!(result, 3 * 2);
}

#[test]
fn test_amazing_zero() {
    let result = calculate_line_part02(
        "do()shsuwje8ilr[[]don't()mul(1,2)mult(3,5)do()mul[123,456]don't()mul(1,2)do()do()don't()mul(123,456)do()mul(0,1000)",
    );
    assert_eq!(result, 0);
}

#[test]
fn test_multiline_file() {
    let path = Path::new("./inputs/day03.multiline_test");
    let result = part02(&path);
    assert_eq!(result, 2);
}
