/// 2024 Day 2
use std::{io::BufRead, path::Path};

use raoc_2024::get_reader;

#[derive(Debug)]
struct Report {
    levels: Vec<u32>,
}

impl Report {
    pub fn new(line: String) -> Self {
        let levels = line
            .split_whitespace()
            .map(|x| x.parse::<u32>().unwrap())
            .collect::<Vec<u32>>();
        Self { levels }
    }

    fn is_strictly_monotonic(levels: &Vec<u32>) -> bool {
        if levels.is_empty() {
            return false;
        }
        if levels.len() == 1 {
            return true;
        }

        let mut is_increasing = false;
        let mut is_decreasing = false;
        let mut is_stagnating = false;
        let mut previous = &levels[0];
        for level in &levels[1..] {
            if level > previous {
                is_increasing = true;
            } else if level < previous {
                is_decreasing = true;
            } else {
                is_stagnating = true;
            }

            if (is_increasing && is_decreasing) || is_stagnating {
                return false;
            }

            previous = level;
        }

        true
    }

    fn is_smooth(levels: &Vec<u32>) -> bool {
        if levels.is_empty() {
            return true;
        }
        if levels.len() == 1 {
            return true;
        }

        let mut previous = &levels[0];
        for level in &levels[1..] {
            let diff = level.abs_diff(*previous);
            if (diff < 1) || (diff > 3) {
                return false;
            }
            previous = level;
        }

        true
    }

    pub fn is_strictly_safe(&self) -> bool {
        Self::is_strictly_monotonic(&self.levels) && Self::is_smooth(&self.levels)
    }

    pub fn is_dampened_safe(&self) -> bool {
        if self.is_strictly_safe() {
            return true;
        }

        for i in 0..self.levels.len() {
            let mut c = self.levels.clone();
            c.remove(i);
            if Self::is_strictly_monotonic(&c) && Self::is_smooth(&c) {
                return true;
            }
        }
        false
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut safe_count = 0;
    for line in reader.lines() {
        let line = line.unwrap();
        let report = Report::new(line);
        if report.is_strictly_safe() {
            safe_count += 1;
        }
    }
    safe_count
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut dampened_safe_count = 0;
    for line in reader.lines() {
        let line = line.unwrap();
        let report = Report::new(line);
        if report.is_dampened_safe() {
            dampened_safe_count += 1;
        }
    }
    dampened_safe_count
}

fn main() {
    let path = Path::new("./inputs/day02");

    let answer = part01(&path);
    println!("How many reports are safe? {}", answer);

    let answer = part02(&path);
    println!(
        "Update your analysis by handling situations where the \
         Problem Dampener can remove a single level from unsafe \
         reports. How many reports are now safe? {}",
        answer
    );
}
