/// 2023 Day 8
use raoc_2023::{get_reader, print_part01_header, print_part02_header};
use regex::Regex;
use std::collections::HashMap;
use std::io::BufRead;
use std::io::Write;
use std::path::Path;

type NumSteps = u32;
type Directions = Vec<usize>;

fn parse(input: &Path) -> (Directions, HashMap<String, [String; 2]>) {
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
                    _ => unreachable!(),
                })
                .collect::<Directions>()
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

    (directions, map)
}

fn part01(input: &Path) -> NumSteps {
    let (directions, map) = parse(&input);

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

fn part02(input: &Path) -> NumSteps {
    let (directions, map) = parse(&input);

    let mut positions: Vec<String> = map
        .keys()
        .filter(|node| node.ends_with("A"))
        .cloned()
        .collect();

    // debug-only
    /*println!("found {} start node(s)", positions.len());
    positions
        .iter()
        .for_each(|node| println!("start node: {}", node));*/

    let mut step = 0;
    let mut dir_index = 0;
    let mut arrived = false;
    while !arrived {
        let dir = directions[dir_index];

        /*positions
        .iter_mut()
        .for_each(|node| *node = map[node][dir].clone());*/
        for node in &mut positions {
            *node = map[node][dir].clone();
        }

        dir_index += 1;
        dir_index %= directions.len();
        step += 1;

        print!("\r{}    ", step);
        let _ = std::io::stdout().flush();

        // arrived = positions.iter().all(|node| node.ends_with("Z"));

        /*println!(
            "Step {} {} {}\n--------------------------",
            step, dir, arrived
        );*/

        arrived = true;
        for node in &positions {
            // println!("    '{}'", node);
            if !node.ends_with("Z") {
                arrived = false;
                break;
            }
        }

        /*positions.iter().for_each(|node| println!("    {}", node));*/

        /*if step == 1000000 {
            break;
        }*/
    }

    step
}

fn main() {
    let input = Path::new("./inputs/day08");

    print_part01_header();
    let answer = part01(&input);
    println!("Starting at AAA, follow the left/right instructions. How many steps are required to reach ZZZ? {}", answer);

    // let input = Path::new("./inputs/day08.example");

    print_part02_header();
    println!("Simultaneously start on every node that ends with A. How many steps does it take before you're only on nodes that end with Z?");
    let answer = part02(&input);
    println!("{}", answer);
}
