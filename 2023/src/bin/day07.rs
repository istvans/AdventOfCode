/// 2023 Day 7
use std::cmp::Ordering;
use std::io::BufRead;
use std::path::Path;

use raoc_2023::{get_reader, print_part01_header, print_part02_header};

#[derive(Debug, PartialEq, Eq, PartialOrd, Ord)]
enum Type {
    HighCard,
    OnePair,
    TwoPair,
    ThreeOfAKind,
    FullHouse,
    FourOfAKind,
    FiveOfAKind,
}

#[test]
fn test_type_ordering() {
    use Type::*;
    assert!(FiveOfAKind > FourOfAKind);
    assert!(FullHouse > HighCard);
    assert!(TwoPair < ThreeOfAKind);
    assert!(TwoPair <= FourOfAKind);
    assert!(FourOfAKind >= FourOfAKind);
    assert!(FiveOfAKind >= FullHouse);
    assert_eq!(OnePair, OnePair);
    assert_eq!(ThreeOfAKind, ThreeOfAKind);
    assert_eq!(FullHouse, FullHouse);
}

#[derive(Debug, PartialEq, Eq)]
struct Hand {
    cards: [u8; 5],
    bid: u32,
}

impl Hand {
    fn new(line: &String) -> Self {
        let parts: Vec<&str> = line.split(' ').collect();
        let cards: Vec<u8> = parts[0]
            .chars()
            .map(|c| match c {
                'T' => 10u8,
                'J' => 11u8,
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect();
        let bid = parts[1].parse::<u32>().unwrap();
        Self { cards, bid }
    }

    fn get_type(&self) -> Type {
        use Type::*;
        FiveOfAKind
    }
}

impl Ord for Hand {
    fn cmp(&self, other: &Self) -> Ordering {
        use Ordering::*;
        let order = if self.get_type() == other.get_type() {
            let card_pairs = std::iter::zip(self.cards, other.cards);
            let mut o: Option<Ordering> = None;
            for (c1, c2) in card_pairs {
                if c1 == c2 {
                    continue;
                } else {
                    o = Some(c1.cmp(&c2));
                }
            }
            match o {
                Some(ord) => ord,
                None => Equal,
            }
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
        break; // TODO Remove
    }
    0 // TODO implement
}

fn main() {
    let input = Path::new("./inputs/day07");

    use Type::*;
    let x = FourOfAKind;
    let y = FiveOfAKind;
    println!("{}", x < y);

    print_part01_header();
    let answer = part01(&input);
    println!(
        "Find the rank of every hand in your set. What are the total winnings? {}",
        answer
    );
}
