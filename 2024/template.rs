/// 2024 Day TODO
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

type Number = usize;

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> Number {
    let reader = get_reader(path);

    for line in reader.lines() {}
    0
}

fn main() {
    let path = Path::new("./inputs/dayTODO");

    print_part01_header();
    println!("TODO");
    let answer = part01(&path);
    println!("{}", answer);

    print_part02_header();
    println!("LOCKED");
}
