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

#[derive(Clone, Debug, PartialEq, Eq)]
struct Hand {
    cards: [u8; 5],
    bid: Number,
    rules: Part,
}

impl Ord for Hand {
    fn cmp(&self, other: &Hand) -> Ordering {
        use Ordering::*;
        let order = if self.get_type() == other.get_type() {
            let card_pairs = std::iter::zip(self.cards, other.cards);
            let mut card_based_order: Option<Ordering> = None;
            for (c1, c2) in card_pairs {
                if c1 == c2 {
                    continue;
                } else {
                    card_based_order = Some(c1.cmp(&c2));
                    break;
                }
            }
            match card_based_order {
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
    fn partial_cmp(&self, other: &Hand) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

impl Hand {
    pub fn new(line: &str, rules: Part) -> Self {
        let (cards_src, bid) = Self::parse(line);
        let cards: [u8; 5] = cards_src
            .chars()
            .map(|c| match c {
                'T' => 10u8,
                'J' => match rules {
                    Part::One => 11u8,
                    Part::Two => 1u8,
                },
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect::<Vec<u8>>()
            .try_into()
            .unwrap();
        Self { cards, bid, rules }
    }

    pub fn get_type(&self) -> Type {
        match self.rules {
            Part::One => self.get_type_part01(),
            Part::Two => self.get_type_part02(),
        }
    }

    pub fn get_bid(&self) -> Number {
        self.bid
    }

    fn get_type_part01(&self) -> Type {
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

    fn get_type_part02(&self) -> Type {
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

    fn parse(line: &str) -> (&str, Number) {
        let parts: Vec<&str> = line.split(' ').collect();

        let bid = if parts.len() == 1 {
            0 // if the bid isn't specified we just assume it's 0
        } else {
            match parts[1].parse::<Number>() {
                Ok(bid) => bid,
                Err(_) => 0,
            }
        };

        (parts[0], bid)
    }
}

#[test]
fn test_hand_type_and_bid_part01() {
    use Type::*;
    let test = Hand::new("A2T63 1234", Part::One);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
}

#[test]
fn test_hand_type_and_bid_part02() {
    use Type::*;
    let test = Hand::new("A2T63 1234", Part::Two);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
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
fn test_hand_type_kinds_part02() {
    let part = Part::Two;
    use Type::*;
    assert_eq!(Hand::new("A2T63", part).get_type(), HighCard);
    assert_eq!(Hand::new("4854J", part).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("ATT9A", part).get_type(), TwoPair);
    assert_eq!(Hand::new("KTT9J", part).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("TJTT3", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("JJ244", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("666JJ", part).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("AAAA4", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQJA", part).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQQQ", part).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("234AJ", part).get_type(), OnePair);
}

#[test]
fn test_ranking_part01() {
    let part = Part::One;
    let unranked = vec![
        Hand::new("32T3K", part),
        Hand::new("T55J5", part),
        Hand::new("KK677", part),
        Hand::new("KTJJT", part),
        Hand::new("QQQJA", part),
    ];
    let mut hands = Hands::from_slice(&unranked[..]);

    hands.rank();

    let ranked = vec![
        Hand::new("32T3K", part),
        Hand::new("KTJJT", part),
        Hand::new("KK677", part),
        Hand::new("T55J5", part),
        Hand::new("QQQJA", part),
    ];
    let expected_order = Hands::from_slice(&ranked[..]);

    assert_eq!(hands, expected_order);
}

#[test]
fn test_ranking_part02() {
    let part = Part::Two;
    let unranked = vec![
        Hand::new("32T3K", part),
        Hand::new("T55J5", part),
        Hand::new("KK677", part),
        Hand::new("KTJJT", part),
        Hand::new("QQQJA", part),
    ];
    let mut hands = Hands::from_slice(&unranked[..]);

    hands.rank();

    let ranked = vec![
        Hand::new("32T3K", part),
        Hand::new("KK677", part),
        Hand::new("T55J5", part),
        Hand::new("QQQJA", part),
        Hand::new("KTJJT", part),
    ];
    let expected_order = Hands::from_slice(&ranked[..]);

    assert_eq!(hands, expected_order);
}

#[derive(Debug, PartialEq, Eq)]
struct Hands {
    hands: Vec<Hand>,
}

impl Hands {
    pub fn from_file(input: &Path, part: Part) -> Self {
        let reader = get_reader(&input);
        let mut hands = Vec::new();
        for line in reader.lines() {
            let line = line.unwrap();
            let hand = Hand::new(&line, part);
            hands.push(hand);
        }
        Self { hands }
    }

    pub fn from_slice(hands: &[Hand]) -> Self {
        let hands = hands.to_vec();
        Self { hands }
    }

    pub fn rank(&mut self) {
        self.hands.sort();
    }

    pub fn get_total_winnings(&self) -> Number {
        self.hands
            .iter()
            .enumerate()
            .map(|(c, h)| ((c + 1) as Number) * h.get_bid())
            .sum()
    }
}

fn part01(input: &Path) -> Number {
    let mut hands = Hands::from_file(input, Part::One);
    hands.rank();
    hands.get_total_winnings()
}

fn part02(input: &Path) -> Number {
    let mut hands = Hands::from_file(input, Part::Two);
    hands.rank();
    hands.get_total_winnings()
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
