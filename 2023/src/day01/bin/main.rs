/// 2023 Day 1
use std::collections::HashMap;
use std::io::BufRead;
use std::path::Path;

use utils::get_reader;

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut sum = 0;
    for line in reader.lines() {
        let line = line.unwrap();
        let numbers = line
            .chars()
            .filter(char::is_ascii_digit)
            .collect::<String>();
        let first_num = numbers.chars().nth(0).unwrap();
        let last_num = numbers.chars().last().unwrap();
        let calibration_value = format!("{}{}", first_num, last_num).parse::<u32>().unwrap();
        sum += calibration_value;
    }
    sum
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let digit_of_digit_word: HashMap<_, u32> = HashMap::from([
        ("one", 1),
        ("two", 2),
        ("three", 3),
        ("four", 4),
        ("five", 5),
        ("six", 6),
        ("seven", 7),
        ("eight", 8),
        ("nine", 9),
    ]);
    let mapped = digit_of_digit_word.keys().map(|d| d.chars().count());
    let longest_digit_word = mapped.clone().max().unwrap();
    let shortest_digit_word = mapped.min().unwrap();

    let mut sum = 0;
    let reader = get_reader(path);
    for line in reader.lines() {
        let line = line.unwrap();
        let mut nums: Vec<u32> = Vec::new();
        for (i, ch) in line.chars().enumerate() {
            if ch.is_ascii_digit() {
                // simple digit
                let radix = 10;
                let num = ch.to_digit(radix).unwrap();
                nums.push(num);
            } else {
                // there might be a digit_word (e.g. "seven") hiding in `line`
                let suffix_length = line.chars().count() - i;
                let lookup_length = std::cmp::min(suffix_length, longest_digit_word);
                let lookup_end_index = lookup_length + i;

                if lookup_length >= shortest_digit_word {
                    let lookup_window = &line[i..lookup_end_index];
                    for (digit_word, digit) in &digit_of_digit_word {
                        if lookup_window.starts_with(digit_word) {
                            nums.push(digit.clone());
                            break;
                        }
                    }
                } else {
                    // not a digit or a digit_word -> next
                }
            }
        }

        let first_num = nums.first().unwrap();
        let last_num = nums.last().unwrap();
        let calibration_value = format!("{}{}", first_num, last_num).parse::<u32>().unwrap();

        sum += calibration_value;
    }
    sum
}

fn main() {
    let path = Path::new("./inputs/day01");

    let answer = part01(&path);
    println!(
        "Consider your entire calibration document. \
         What is the sum of all of the calibration values? {}",
        answer
    );

    let answer = part02(&path);
    println!(
        "What is the sum of all of the calibration values? {}",
        answer
    );
}
