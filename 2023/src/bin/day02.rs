/// 2023 Day 2
use std::cmp::Ordering;
use std::io::BufRead;
use std::path::Path;

use regex::Regex;

use raoc_2023::get_reader;

#[derive(Debug, PartialEq, Eq)]
struct CubeSet {
    red: u32,
    green: u32,
    blue: u32,
}

impl CubeSet {
    pub fn new(red: u32, green: u32, blue: u32) -> Self {
        Self { red, green, blue }
    }

    /// The power of a set of cubes is equal to the numbers of
    /// red, green, and blue cubes multiplied together.
    pub fn power(&self) -> u32 {
        self.red * self.green * self.blue
    }
}

impl Ord for CubeSet {
    fn cmp(&self, other: &Self) -> Ordering {
        let mut ordering = Ordering::Equal;
        if (self.red > other.red) || (self.green > other.green) || (self.blue > other.blue) {
            ordering = Ordering::Greater;
        }
        ordering
    }
}

impl PartialOrd for CubeSet {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug)]
struct Game {
    id: u32,
    sets: Vec<CubeSet>,
}

impl Game {
    fn parse_sets(sets: &str) -> Vec<CubeSet> {
        let red_re = Regex::new(r"^(?<count>\d+) red$").unwrap();
        let blue_re = Regex::new(r"^(?<count>\d+) blue$").unwrap();
        let green_re = Regex::new(r"^(?<count>\d+) green$").unwrap();

        let mut result = Vec::new();
        for cube_set in sets.split(';') {
            let mut set = CubeSet::new(0, 0, 0);
            for cubes in cube_set.split(',') {
                let cubes = cubes.trim();
                if let Some(red) = red_re.captures(&cubes) {
                    set.red = red.name("count").unwrap().as_str().parse::<u32>().unwrap();
                } else if let Some(blue) = blue_re.captures(&cubes) {
                    set.blue = blue.name("count").unwrap().as_str().parse::<u32>().unwrap();
                } else if let Some(green) = green_re.captures(&cubes) {
                    set.green = green
                        .name("count")
                        .unwrap()
                        .as_str()
                        .parse::<u32>()
                        .unwrap();
                } else {
                    panic!("WTF is this cubes:'{}'?!", cubes);
                }
            }
            result.push(set);
        }
        result
    }

    pub fn new(line: String) -> Self {
        let line_regex = Regex::new(r"Game (?<id>\d+): (?<sets>.*)$").unwrap();
        let captures = line_regex.captures(&line).unwrap();
        let id = captures
            .name("id")
            .unwrap()
            .as_str()
            .parse::<u32>()
            .unwrap();

        let sets = captures.name("sets").unwrap().as_str();
        let sets = Self::parse_sets(sets);
        Self { id, sets }
    }

    pub fn is_possible(&self, config: &CubeSet) -> bool {
        let mut possible = true;
        for set in &self.sets {
            if set > config {
                possible = false;
                break;
            }
        }
        possible
    }

    /// Return the minimal bag which contains the maximum red, blue and green
    /// cubes from the game sets.
    pub fn minimal_bag(&self) -> CubeSet {
        let mut bag = CubeSet::new(0, 0, 0);
        for set in &self.sets {
            if set.red > bag.red {
                bag.red = set.red;
            }
            if set.green > bag.green {
                bag.green = set.green;
            }
            if set.blue > bag.blue {
                bag.blue = set.blue;
            }
        }
        bag
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let bag = CubeSet::new(/*red*/ 12, /*green*/ 13, /*blue*/ 14);

    let reader = get_reader(path);
    let mut possible_games = Vec::new();
    for line in reader.lines() {
        let line = line.unwrap();
        let game = Game::new(line);
        if game.is_possible(&bag) {
            possible_games.push(game);
        }
    }

    possible_games.iter().map(|x| x.id).sum()
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> u32 {
    let reader = get_reader(path);
    let mut games = Vec::new();
    for line in reader.lines() {
        let line = line.unwrap();
        let game = Game::new(line);
        games.push(game);
    }
    games
        .iter()
        .map(|game| game.minimal_bag())
        .map(|bag| bag.power())
        .sum()
}

fn main() {
    let path = Path::new("./inputs/day02");

    let answer = part01(&path);
    println!(
        "Determine which games would have been possible if the bag \
         had been loaded with only 12 red cubes, 13 green cubes, and \
         14 blue cubes. What is the sum of the IDs of those games? {}",
        answer
    );

    let answer = part02(&path);
    println!(
        "For each game, find the minimum set of cubes that must have been present. \
         What is the sum of the power of these sets? {}",
        answer
    );
}
