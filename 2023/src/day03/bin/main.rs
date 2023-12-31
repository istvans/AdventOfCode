/// 2023 Day 3
use std::collections::{HashMap, HashSet};
use std::io::BufRead;
use std::path::Path;

use utils::get_reader;

#[derive(Clone, Debug, PartialEq, Eq, Hash)]
struct Point {
    pub x: usize,
    pub y: usize,
}

impl Point {
    pub fn new(x: usize, y: usize) -> Self {
        Self { x, y }
    }

    pub fn set(&mut self, x: usize, y: usize) {
        self.x = x;
        self.y = y;
    }
}

#[derive(Debug)]
struct SchematicNumber {
    start: Point,
    end: Point,
    number: usize,
}

impl SchematicNumber {
    pub fn new(start: Point, end: Point, number: usize) -> Self {
        Self { start, end, number }
    }

    /// Return the adjacent points.
    /// Example:
    ///
    ///     * * * * *
    ///     * 1 2 3 *
    ///     * * * * *
    pub fn adjacent_points(&self) -> Vec<Point> {
        let mut result = Vec::new();

        // *
        // *
        // *
        if self.start.x > 0 {
            if self.start.y > 0 {
                result.push(Point::new(self.start.x - 1, self.start.y - 1));
            }
            result.push(Point::new(self.start.x - 1, self.start.y));
            result.push(Point::new(self.start.x - 1, self.start.y + 1));
        }

        // * * *
        // 1 2 3
        // * * *
        for x in self.start.x..(self.end.x + 1) {
            if self.start.y > 0 {
                result.push(Point::new(x, self.start.y - 1));
            }
            result.push(Point::new(x, self.start.y + 1));
        }

        // *
        // *
        // *
        if self.end.y > 0 {
            result.push(Point::new(self.end.x + 1, self.end.y - 1));
        }
        result.push(Point::new(self.end.x + 1, self.end.y));
        result.push(Point::new(self.end.x + 1, self.end.y + 1));

        result
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> usize {
    let mut symbol_of_coord = HashMap::new();
    let mut schematic_numbers = Vec::new();

    let reader = get_reader(&path);
    for (y, line) in reader.lines().enumerate() {
        let line = line.unwrap();
        let mut digits = String::new();
        let mut start = Point::new(0, 0);
        for (x, ch) in line.chars().enumerate() {
            if ch.is_ascii_digit() {
                if digits.is_empty() {
                    start.set(x, y);
                }
                digits.push(ch);
            } else if ch != '.' {
                symbol_of_coord.insert(Point::new(x, y), ch);
            } else {
                assert!(ch == '.'); // -> ignored
            }

            if !ch.is_ascii_digit() || (x == line.len() - 1) {
                if !digits.is_empty() {
                    let number = digits.parse::<usize>().unwrap();
                    let end = Point::new(x - 1, y);
                    let schematic_number = SchematicNumber::new(start, end, number);
                    schematic_numbers.push(schematic_number);

                    // reset the state for the next cycle
                    start = Point::new(0, 0);
                    digits.clear();
                }
            }
        }
    }

    let mut part_numbers = Vec::new();
    for schematic_number in schematic_numbers {
        for point in schematic_number.adjacent_points() {
            if symbol_of_coord.contains_key(&point) {
                part_numbers.push(schematic_number.number);
                break;
            }
        }
    }

    part_numbers.iter().sum()
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> usize {
    let mut asterisk_coordinates = HashSet::new();
    let mut schematic_numbers = Vec::new();

    let reader = get_reader(&path);
    for (y, line) in reader.lines().enumerate() {
        let line = line.unwrap();
        let mut digits = String::new();
        let mut start = Point::new(0, 0);
        for (x, ch) in line.chars().enumerate() {
            if ch.is_ascii_digit() {
                if digits.is_empty() {
                    start.set(x, y);
                }
                digits.push(ch);
            } else if ch == '*' {
                asterisk_coordinates.insert(Point::new(x, y));
            } else {
                // other special characters or periods
            }

            if !ch.is_ascii_digit() || (x == line.len() - 1) {
                if !digits.is_empty() {
                    let number = digits.parse::<usize>().unwrap();
                    let end = Point::new(x - 1, y);
                    let schematic_number = SchematicNumber::new(start, end, number);
                    schematic_numbers.push(schematic_number);

                    // reset the state for the next cycle
                    start = Point::new(0, 0);
                    digits.clear();
                }
            }
        }
    }

    let mut gear_candidates = HashMap::new();
    for schematic_number in schematic_numbers {
        for point in schematic_number.adjacent_points() {
            if asterisk_coordinates.contains(&point) {
                gear_candidates
                    .entry(point)
                    .or_insert(Vec::new())
                    .push(schematic_number.number);
            }
        }
    }

    gear_candidates
        .iter()
        .filter(|(_, v)| v.len() == 2)
        .map(|(_, v)| v[0] * v[1])
        .sum()
}

fn main() {
    let path = Path::new("./inputs/day03");

    let answer = part01(&path);
    println!(
        "What is the sum of all of the part numbers \
         in the engine schematic? {}",
        answer
    );

    let answer = part02(&path);
    println!(
        "What is the sum of all of the gear ratios in your engine schematic? {}",
        answer
    );
}
