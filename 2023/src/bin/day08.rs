/// 2023 Day 8
use raoc_2023::{get_reader, print_part01_header, print_part02_header};
use regex::Regex;
use std::collections::HashMap;
use std::io::BufRead;
use std::path::Path;

type NumSteps = u32;

fn part01(input: &Path) -> NumSteps {
    let reader = get_reader(&input);

    let mut directions = Vec::new();
    let mut map = HashMap::new();
    let mut parse_dir = true;
    let re = Regex::new(r"(?<node>\S+)\s+=\s+\((?<left>[^,]+),\s+(?<right>[^)]+)\)").unwrap();
    for line in reader.lines() {
        let line = line.unwrap();
        if parse_dir {
            directions = line
                .chars()
                .map(|d| match d {
                    'L' => 0,
                    'R' => 1,
                    _ => 2,
                })
                .collect::<Vec<usize>>()
                .try_into()
                .unwrap();
            parse_dir = false;
        } else {
            let caps = re.captures(&line);
            if let Some(x) = caps {
                let node = x["node"].to_string();
                let left = x["left"].to_string();
                let right = x["right"].to_string();
                map.insert(node, [left, right]);
            }
        }
    }

    let goal = "ZZZ";
    let mut i = String::from("AAA");
    let mut step = 0;
    let mut dir = 0;
    while i != goal {
        i = map[&i][directions[dir]].to_string();
        dir += 1;
        dir %= directions.len();
        step += 1;
    }

    step
}

fn main() {
    let input = Path::new("./inputs/day08");

    print_part01_header();
    let answer = part01(&input);
    println!("Starting at AAA, follow the left/right instructions. How many steps are required to reach ZZZ? {}", answer);
}
