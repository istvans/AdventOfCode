/// 2024 Day 2
use std::path::Path;

use raoc_2024::get_reader;

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let reader = get_reader(path);

    0
}

fn main() {
    let path = Path::new("./inputs/day02");

    let answer = part01(&path);
    // TODO
    println!("??? {}", answer);
}
