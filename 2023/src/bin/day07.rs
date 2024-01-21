/// 2023 Day 7
use std::cmp::Ordering;
use std::collections::HashMap;
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

type Number = u32;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Part {
    One,
    Two,
}

trait Hand: PartialOrd + Ord {
    fn new(line: &str, part: Part) -> Self {
        let parts: Vec<&str> = line.split(' ').collect();

        let bid = if parts.len() == 1 {
            0 // if the bid isn't specified we just assume it's 0
        } else {
            match parts[1].parse::<Number>() {
                Ok(bid) => bid,
                Err(_) => 0,
            }
        };

        match part {
            Part::One => HandPart01::new(parts[0], bid),
            Part::Two => HandPart02::new(parts[0], bid),
            _ => unreachable!("what kind of hand '{}' should be?", part),
        }
    }

    fn get_type(&self) -> Type;

    fn get_bid(&self) -> Number {
        self.bid
    }

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
                    break;
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

    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[derive(Debug, PartialEq, Eq)]
struct HandPart01 {
    cards: [u8; 5],
    bid: Number,
}

impl HandPart01 {
    pub fn new(cards_src: &str, bid: Number) -> Self {
        let cards: [u8; 5] = cards_src
            .chars()
            .map(|c| match c {
                'T' => 10u8,
                'J' => 11u8,
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect::<Vec<u8>>()
            .try_into()
            .unwrap();
        Self { cards, bid }
    }
}

impl Hand for HandPart01 {
    fn get_type(&self) -> Type {
        use Type::*;
        let mut count_of_cards: HashMap<u8, u8> = HashMap::new();
        for card in &self.cards {
            count_of_cards
                .entry(*card)
                .and_modify(|c| *c += 1)
                .or_insert(1);
        }

        let max_count = *(count_of_cards.values().max().unwrap());
        let hand_type = if max_count == 5 {
            FiveOfAKind
        } else if max_count == 4 {
            FourOfAKind
        } else if max_count == 3 {
            if count_of_cards.values().any(|&c| c == 2) {
                FullHouse
            } else {
                ThreeOfAKind
            }
        } else {
            let num_pairs = count_of_cards.values().filter(|&c| *c == 2).count();
            if num_pairs == 2 {
                TwoPair
            } else if num_pairs == 1 {
                OnePair
            } else {
                HighCard
            }
        };

        hand_type
    }
}

#[derive(Debug, PartialEq, Eq)]
struct HandPart02 {
    cards: [u8; 5],
    bid: Number,
}

impl HandPart02 {
    pub fn new(cards_src: &str, bid: Number) -> Self {
        let cards: [u8; 5] = cards_src
            .chars()
            .map(|c| match c {
                'J' => 1u8,
                'T' => 10u8,
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect::<Vec<u8>>()
            .try_into()
            .unwrap();
        Self { cards, bid }
    }
}

impl Hand for HandPart02 {
    fn get_type(&self) -> Type {
        use Type::*;
        let mut count_of_cards: HashMap<u8, u8> = HashMap::new();
        for card in &self.cards {
            count_of_cards
                .entry(*card)
                .and_modify(|c| *c += 1)
                .or_insert(1);
        }

        let max_count = *(count_of_cards.values().max().unwrap());
        let num_jokers = count_of_cards.get(&1);
        let hand_type = if max_count == 5 {
            FiveOfAKind
        } else if max_count == 4 {
            if num_jokers.is_some_and(|n| *n == 1) {
                FiveOfAKind
            } else {
                FourOfAKind
            }
        } else if max_count == 3 {
            if num_jokers.is_some_and(|n| *n < 3) {
                let num_jokers = *num_jokers.unwrap();
                if num_jokers == 1 {
                    FourOfAKind
                } else if num_jokers == 2 {
                    FiveOfAKind
                } else {
                    unreachable!("joker counting error");
                }
            } else {
                if count_of_cards.values().any(|&c| c == 2) {
                    FullHouse
                } else {
                    ThreeOfAKind
                }
            }
        } else {
            let num_pairs = count_of_cards.values().filter(|&c| *c == 2).count();
            if num_pairs == 2 {
                if num_jokers.is_some() {
                    let num_jokers = *num_jokers.unwrap();
                    if num_jokers == 1 {
                        ThreeOfAKind
                    } else if num_jokers == 2 {
                        FourOfAKind
                    } else {
                        unreachable!("joker counting error")
                    }
                } else {
                    TwoPair
                }
            } else if num_pairs == 1 {
                if num_jokers.is_some_and(|n| *n == 1) {
                    ThreeOfAKind
                } else {
                    OnePair
                }
            } else {
                if num_jokers.is_some() {
                    OnePair
                } else {
                    HighCard
                }
            }
        };

        hand_type
    }
}

#[test]
fn test_hand_type_kinds_part01() {
    let part = Part::One;
    use Type::*;
    assert_eq!(Hand::new("A2T63", part).get_type(), HighCard);
    assert_eq!(Hand::new("4854J", part).get_type(), OnePair);
    assert_eq!(Hand::new("ATT9A", part).get_type(), TwoPair);
    assert_eq!(Hand::new("TJTT3", part).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("666JJ", part).get_type(), FullHouse);
    assert_eq!(Hand::new("AAAA4", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQQQ", part).get_type(), FiveOfAKind);
}

#[test]
fn test_hand_type_and_bid_part01() {
    use Type::*;
    let test = Hand::new("A2T63 1234", Part::One);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
}

#[test]
fn test_sorting_part01() {
    let part = Part::One;
    let mut hands = vec![
        Hand::new("32T3K", part),
        Hand::new("T55J5", part),
        Hand::new("KK677", part),
        Hand::new("KTJJT", part),
        Hand::new("QQQJA", part),
    ];
    hands.sort();
    let expected_order = vec![
        Hand::new("32T3K", part),
        Hand::new("KTJJT", part),
        Hand::new("KK677", part),
        Hand::new("T55J5", part),
        Hand::new("QQQJA", part),
    ];

    assert_eq!(hands, expected_order);
}

#[test]
fn test_hand_type_kinds_part02() {
    let part = Part::Two;
    use Type::*;
    assert_eq!(Hand::new("A2T63", part).get_type(), HighCard);
    assert_eq!(Hand::new("4854J", part).get_type(), OnePair);
    assert_eq!(Hand::new("ATT9A", part).get_type(), TwoPair);
    assert_eq!(Hand::new("TJTT3", part).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("666JJ", part).get_type(), FullHouse);
    assert_eq!(Hand::new("AAAA4", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQQQ", part).get_type(), FiveOfAKind);
}

fn parse_hands(input: &Path, part: Part) -> Vec<Box<dyn Hand>> {
    let reader = get_reader(&input);
    let mut hands = Vec::new();
    for line in reader.lines() {
        let line = line.unwrap();
        let hand = Hand::new(&line, part);
        hands.push(hand);
    }
    hands
}

fn rank_hands(hands: &mut Vec<Box<dyn Hand>>) -> () {
    hands.sort();
}

fn get_total_winnings(ranked_hands: &Vec<Box<dyn Hand>>) -> Number {
    ranked_hands
        .iter()
        .enumerate()
        .map(|(c, h)| ((c + 1) as Number) * h.get_bid())
        .sum()
}

fn part01(input: &Path) -> Number {
    let mut hands = parse_hands(input, Part::One);
    rank_hands(&mut hands);
    get_total_winnings(&hands)
}

fn part02(input: &Path) -> Number {
    let mut hands = parse_hands(input, Part::Two);
    rank_hands(&mut hands);
    get_total_winnings(&hands)
}

fn main() {
    let input = Path::new("./inputs/day07");

    print_part01_header();
    let answer = part01(&input);
    println!(
        "Find the rank of every hand in your set. What are the total winnings? {}",
        answer
    );

    print_part02_header();
    let answer = part02(&input);
    println!(
        "Using the new joker rule, find the rank of every hand in your set. \
         What are the new total winnings? {}",
        answer
    );
}
