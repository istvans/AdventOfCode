/// 2024 Day 1
use std::fs::File;
use std::io::BufRead;
use std::io::BufReader;
use std::iter::zip;
use std::num::ParseIntError;
use std::path::Path;

use raoc_2024::get_reader;

fn parse(src: Option<&str>) -> Result<u32, ParseIntError> {
    src.unwrap().parse::<u32>()
}

fn get_sorted_left_and_right_numbers(reader: &mut BufReader<File>) -> (Vec<u32>, Vec<u32>) {
    let mut left = vec![];
    let mut right = vec![];
    for line in reader.lines() {
        let line = line.unwrap();
        let mut iter = line.split_ascii_whitespace();

        let left_val = parse(iter.next()).unwrap();
        left.push(left_val);

        let right_val = parse(iter.next()).unwrap();
        right.push(right_val);
    }

    left.sort();
    right.sort();

    (left, right)
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let mut reader = get_reader(path);
    let (left, right) = get_sorted_left_and_right_numbers(&mut reader);

    let mut sum = 0;
    for (left_val, right_val) in zip(left, right) {
        let diff = left_val.abs_diff(right_val);
        sum += diff;
    }

    sum
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let mut reader = get_reader(path);
    let (left, right) = get_sorted_left_and_right_numbers(&mut reader);

    let mut sum = 0;
    for left_val in left {
        let count = right
            .iter()
            .filter(|&right_val| left_val == *right_val)
            .count();
        sum += left_val * u32::try_from(count).unwrap();
    }

    sum
}

fn main() {
    let path = Path::new("./inputs/day01");

    let answer = part01(&path);
    println!("What is the total distance between your lists? {}", answer);

    let answer = part02(&path);
    println!("What is their similarity score? {}", answer);
}
