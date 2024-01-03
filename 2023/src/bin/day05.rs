/// 2023 Day 5
use std::collections::BTreeMap;
use std::io::BufRead;
use std::path::Path;

use raoc_2023::get_reader;

#[derive(PartialEq, Eq)]
enum Mode {
    Seed,
    Skip,
    Map,
}

type Number = u64;
type Seeds = Vec<Number>;

struct Map {
    source_range_start: Number,
    destination_range_start: Number,
    range_length: Number,
}

struct Locator {
    map: Vec<BTreeMap<Number, Map>>,
}

impl Locator {
    pub fn new() -> Self {
        Self { map: Vec::new() }
    }

    pub fn new_layer(&mut self) {
        self.map.push(BTreeMap::new());
    }

    pub fn is_empty(&self) -> bool {
        self.map.is_empty()
    }

    pub fn add(&mut self, layer_mapping: Map) {
        assert!(!self.is_empty());
        let last_id = self.map.len() - 1;
        self.map[last_id]
            .entry(layer_mapping.source_range_start)
            .or_insert(layer_mapping);
    }

    pub fn location(&self, seed: Number) -> Number {
        let mut locator = seed;
        for mapping in &self.map {
            for (source_range_start, map) in mapping {
                if locator >= *source_range_start {
                    let end = source_range_start + map.range_length;
                    let range = *source_range_start..end;
                    if range.contains(&locator) {
                        let offset = locator - source_range_start;
                        locator = map.destination_range_start + offset;
                        break;
                    }
                }
            }
            // if it wasn't found then the source == destination
            // so the locator does not need any update
        }
        locator
    }
}

type SeedParserFn = fn(&String) -> Seeds;

fn part01_seed_parser(line: &String) -> Seeds {
    let split = line.split(' ');
    split
        .skip(1)
        .map(|e| e.parse::<Number>().unwrap())
        .collect()
}

fn get_seeds_and_locator(path: &Path, parse_seed_fn: SeedParserFn) -> (Seeds, Locator) {
    let reader = get_reader(path);
    let mut seeds = Vec::new();
    let mut mode = Mode::Seed;
    let mut locator = Locator::new();
    for line in reader.lines() {
        let line = line.unwrap();

        if mode == Mode::Seed {
            seeds = parse_seed_fn(&line);
            println!("Found {} seed(s).", seeds.len());
            mode = Mode::Skip;
        } else if line.is_empty() {
            mode = Mode::Skip;
        } else if line.find(" map:").is_some() {
            locator.new_layer();
            mode = Mode::Map;
        } else if mode == Mode::Map {
            let split = line.split(' ');
            let mapping: Vec<Number> = split.map(|e| e.parse::<Number>().unwrap()).collect();

            let destination_range_start = mapping[0];
            let source_range_start = mapping[1];
            let range_length = mapping[2];

            assert!(!locator.is_empty());
            locator.add(Map {
                source_range_start,
                destination_range_start,
                range_length,
            });
        } else {
            unreachable!();
        }
    }

    (seeds, locator)
}

fn get_lowest_location_number(seeds: &Seeds, locator: &Locator) -> Number {
    seeds
        .iter()
        .map(|seed| locator.location(*seed))
        .min()
        .expect("Cannot determine the minimum location number?!")
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> Number {
    let (seeds, locator) = get_seeds_and_locator(&path, part01_seed_parser);
    get_lowest_location_number(&seeds, &locator)
}

fn part02_seed_parser(line: &String) -> Seeds {
    let split = line.split(' ');
    println!("... Collecting all the seeds from the ranges. Please wait ...");
    split
        .skip(1)
        .map(|n| n.parse::<Number>().unwrap())
        .collect::<Vec<Number>>()
        .chunks_exact(2)
        .flat_map(|x| x[0]..(x[0] + x[1])) // x[0]=start x[1]=length
        .collect()
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> Number {
    let (seeds, locator) = get_seeds_and_locator(&path, part02_seed_parser);
    println!("... Looking for the lowest location number. Please be patient ...");
    get_lowest_location_number(&seeds, &locator)
}

fn main() {
    let path = Path::new("./inputs/day05");

    println!("Part 1:\n========================================================");
    let answer = part01(&path);
    println!(
        "What is the lowest location number that corresponds to any of the initial \
         seed numbers? {}",
        answer
    );

    println!("\nPart 2:\n========================================================");
    println!(
        "WARNING This is a working, but suboptimal, MVP solution! \
         It takes several minutes to complete using a \
         12th gen i7-1255U @3+GHz (with 25% single-core utilisation)! \
         You've been warned."
    );
    let answer = part02(&path);
    println!(
        "Consider all of the initial seed numbers listed in the ranges on the \
              first line of the almanac. What is the lowest location number that \
              corresponds to any of the initial seed numbers? {}",
        answer
    );
}
