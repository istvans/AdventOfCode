/// 2024 Day 5
use std::collections::{HashMap, HashSet};
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

enum Mode {
    Rule,
    Update,
}

fn parse_updates_and_rules(path: &Path) -> (Vec<Vec<u32>>, HashMap<u32, HashSet<u32>>) {
    let reader = get_reader(path);

    let mut mode = Mode::Rule;
    let mut must_come_before: HashMap<u32, HashSet<u32>> = HashMap::new();
    let mut updates = vec![];

    for line in reader.lines() {
        let line = line.unwrap();

        if line.is_empty() {
            mode = Mode::Update;
        } else {
            match mode {
                Mode::Rule => {
                    /* The notation X|Y means that if both page number X and page number Y are
                     * to be produced as part of an update, page number X must be printed at
                     * some point before page number Y. */
                    let rule: Vec<u32> =
                        line.split('|').map(|x| x.parse::<u32>().unwrap()).collect();
                    let x = rule[0];
                    let y = rule[1];
                    must_come_before.entry(y).or_default().insert(x);
                }
                Mode::Update => {
                    let update: Vec<u32> =
                        line.split(',').map(|x| x.parse::<u32>().unwrap()).collect();
                    updates.push(update);
                }
            }
        }
    }

    (updates, must_come_before)
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let (updates, must_come_before) = parse_updates_and_rules(path);

    let mut sum = 0;
    for update in updates {
        let mut correct = true;
        for (current_index, value) in update.iter().enumerate() {
            let befores = &must_come_before[value];
            for after_index in (current_index + 1)..update.len() {
                let after = update[after_index];
                if befores.contains(&after) {
                    correct = false;
                    break;
                }
            }
        }

        if correct {
            let middle_index = update.len() / 2;
            let middle = update[middle_index];
            sum += middle;
        }
    }
    sum
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let (updates, must_come_before) = parse_updates_and_rules(path);

    let mut sum = 0;
    for update in updates {
        let mut needed_correction = false;
        let mut corrected_update = update.clone();
        let mut correct = false;
        while !correct {
            correct = true;
            let previous_correction = corrected_update.clone();
            for (current_index, value) in previous_correction.iter().enumerate() {
                let befores = &must_come_before[value];
                for after_index in (current_index + 1)..previous_correction.len() {
                    let after = previous_correction[after_index];
                    if befores.contains(&after) {
                        needed_correction = true;
                        correct = false;
                        corrected_update.swap(after_index, current_index);
                    }
                }
            }
        }

        if needed_correction {
            let middle_index = corrected_update.len() / 2;
            let middle = corrected_update[middle_index];
            sum += middle;
        }
    }
    sum
}

fn main() {
    let path = Path::new("./inputs/day05");

    print_part01_header();
    let answer = part01(&path);
    println!(
        "Determine which updates are already in the correct \
         order. What do you get if you add up the middle page \
         number from those correctly-ordered updates?\n{}",
        answer,
    );

    print_part02_header();
    let answer = part02(&path);
    println!(
        "Find the updates which are not in the correct order. \
         What do you get if you add up the middle page numbers after \
         correctly ordering just those updates?\n{}",
        answer
    );
}
