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

#[derive(Debug, PartialEq, Eq)]
struct Hand {
    cards: [u8; 5],
    bid: Number,
}

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Mode {
    Part01,
    Part02,
}

impl Hand {
    fn new(line: &str, mode: Mode) -> Self {
        let parts: Vec<&str> = line.split(' ').collect();

        use Mode::*;
        let cards: [u8; 5] = parts[0]
            .chars()
            .map(|c| match c {
                'T' => 10u8,
                'J' => {
                    if mode == Part01 {
                        11u8
                    } else if mode == Part02 {
                        1u8
                    } else {
                        unreachable!("unexpected mode: {:?}!", mode);
                    }
                }
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect::<Vec<u8>>()
            .try_into()
            .unwrap();

        let bid = if parts.len() == 1 {
            0 // if the bid isn't specified we just assume it's 0
        } else {
            match parts[1].parse::<Number>() {
                Ok(bid) => bid,
                Err(_) => 0,
            }
        };

        Self { cards, bid }
    }

    pub fn get_type(&self) -> Type {
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

    pub fn get_bid(&self) -> Number {
        self.bid
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
}

#[test]
fn test_hand_type_kinds_part01() {
    let mode = Mode::Part01;
    use Type::*;
    assert_eq!(Hand::new("A2T63", mode).get_type(), HighCard);
    assert_eq!(Hand::new("4854J", mode).get_type(), OnePair);
    assert_eq!(Hand::new("ATT9A", mode).get_type(), TwoPair);
    assert_eq!(Hand::new("TJTT3", mode).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("666JJ", mode).get_type(), FullHouse);
    assert_eq!(Hand::new("AAAA4", mode).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQQQ", mode).get_type(), FiveOfAKind);
}

#[test]
fn test_hand_type_and_bid_part01() {
    use Mode::*;
    use Type::*;
    let test = Hand::new("A2T63 1234", Part01);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
}

impl PartialOrd for Hand {
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        Some(self.cmp(other))
    }
}

#[test]
fn test_sorting_part01() {
    let mode = Mode::Part01;
    let mut hands = vec![
        Hand::new("32T3K", mode),
        Hand::new("T55J5", mode),
        Hand::new("KK677", mode),
        Hand::new("KTJJT", mode),
        Hand::new("QQQJA", mode),
    ];
    hands.sort();
    let expected_order = vec![
        Hand::new("32T3K", mode),
        Hand::new("KTJJT", mode),
        Hand::new("KK677", mode),
        Hand::new("T55J5", mode),
        Hand::new("QQQJA", mode),
    ];

    assert_eq!(hands, expected_order);
}

type Hands = Vec<Hand>;

fn parse_hands(input: &Path, mode: Mode) -> Hands {
    let reader = get_reader(&input);
    let mut hands = Vec::new();
    for line in reader.lines() {
        let line = line.unwrap();
        let hand = Hand::new(&line, mode);
        hands.push(hand);
    }
    hands
}

fn rank_hands(hands: &mut Hands) -> () {
    hands.sort();
}

fn get_total_winnings(ranked_hands: &Hands) -> Number {
    ranked_hands
        .iter()
        .enumerate()
        .map(|(c, h)| ((c + 1) as Number) * h.get_bid())
        .sum()
}

fn part01(input: &Path) -> Number {
    let mut hands = parse_hands(input, Mode::Part01);
    rank_hands(&mut hands);
    get_total_winnings(&hands)
}

fn part02(input: &Path) -> Number {
    let mut hands = parse_hands(input, Mode::Part02);
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
