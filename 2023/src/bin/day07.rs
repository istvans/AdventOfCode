/// 2023 Day 7
use std::cmp::Ordering;
use std::io::BufRead;
use std::path::Path;

use raoc_2023::{get_reader, print_part01_header, print_part02_header};

#[derive(Debug, PartialEq, Eq)]
enum Type {
    FiveOfAKind,
    FourOfAKind,
    FullHouse,
    ThreeOfAKind,
    TwoPair,
    OnePair,
    HighCard,
}

impl Ord for Type {
    fn cmp(&self, other: &Self) -> Ordering {
        use Ordering::*;
        use Type::*;
        match (self, other) {
            (FiveOfAKind, FiveOfAKind) => Equal,
            (FiveOfAKind, _) => Greater,
            (FourOfAKind, FiveOfAKind) => Less,
            (FourOfAKind, FourOfAKind) => Equal,
            (FourOfAKind, _) => Greater,
            (_, FiveOfAKind) => Less,
            _ => unreachable!("Type ordering isn't correctly implemented"),
        }
    }
}

impl PartialOrd for Type {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug, PartialEq, Eq)]
struct Hand {
    cards: [u8; 5],
    bid: u32,
}

impl Hand {
    fn get_type(&self) -> Type {
        use Type::*;
        FiveOfAKind
    }
}

impl Ord for Hand {
    fn cmp(&self, other: &Self) -> Ordering {
        use Ordering::*;
        let order = if self.get_type() == other.get_type() {
            // TODO
            Equal
        } else {
            self.get_type().cmp(&other.get_type())
        };
        order
    }
}

impl PartialOrd for Hand {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

fn part01(input: &Path) -> u32 {
    let reader = get_reader(&input);
    for line in reader.lines() {
        let line = line.unwrap();
        println!("{}", line);
    }
    0 // TODO implement
}

fn main() {
    let input = Path::new("./inputs/day07");

    print_part01_header();
    let answer = part01(&input);
    println!(
        "Find the rank of every hand in your set. What are the total winnings? {}",
        answer
    );
}
