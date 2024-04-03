/// 2023 Day 7
use raoc_2023::{get_reader, print_part01_header, print_part02_header};
use std::cmp::Ordering;
use std::collections::HashMap;
use std::fmt;
use std::fs::File;
use std::io::BufRead;
use std::io::Write;
use std::path::Path;

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

type Number = u32;

#[derive(Clone, Copy, Debug, PartialEq, Eq)]
enum Rules {
    Part01,
    Part02,
}

#[derive(Clone, Debug, PartialEq, Eq)]
struct Hand {
    cards: [u8; 5],
    bid: Number,
    rules: Rules,
    rank: Number,
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
    pub fn new(line: &str, rules: Rules) -> Self {
        let no_rank = 0;
        Hand::new_with_rank(line, rules, no_rank)
    }

    pub fn new_with_rank(line: &str, rules: Rules, rank: Number) -> Self {
        let (cards_src, bid) = Self::parse(line);
        let cards: [u8; 5] = cards_src
            .chars()
            .map(|c| match c {
                'T' => 10u8,
                'J' => match rules {
                    Rules::Part01 => 11u8,
                    Rules::Part02 => 1u8,
                },
                'Q' => 12u8,
                'K' => 13u8,
                'A' => 14u8,
                num => num.to_digit(10).unwrap() as u8,
            })
            .collect::<Vec<u8>>()
            .try_into()
            .unwrap();
        Self {
            cards,
            bid,
            rules,
            rank,
        }
    }

    pub fn get_type(&self) -> Type {
        match self.rules {
            Rules::Part01 => self.get_type_part01(),
            Rules::Part02 => self.get_type_part02(),
        }
    }

    pub fn get_bid(&self) -> Number {
        self.bid
    }

    pub fn get_cards_as_str(&self) -> String {
        let mut result = String::new();
        for card_num in self.cards {
            let card_char = match card_num {
                10u8 => 'T',
                1u8 | 11u8 => 'J',
                12u8 => 'Q',
                13u8 => 'K',
                14u8 => 'A',
                num => (num + 48u8) as char,
            };
            result.push(card_char);
        }
        result
    }

    fn get_count_of_cards(&self) -> HashMap<u8, u8> {
        let mut count_of_cards: HashMap<u8, u8> = HashMap::new();
        for card in &self.cards {
            count_of_cards
                .entry(*card)
                .and_modify(|c| *c += 1)
                .or_insert(1);
        }
        count_of_cards
    }

    fn get_type_part01(&self) -> Type {
        use Type::*;

        let count_of_cards = self.get_count_of_cards();

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

        let count_of_cards = self.get_count_of_cards();

        let max_count = *(count_of_cards.values().max().unwrap());
        let num_jokers = count_of_cards.get(&1);
        let hand_type = if max_count == 5 {
            FiveOfAKind
        } else if max_count == 4 {
            if num_jokers.is_some() {
                FiveOfAKind
            } else {
                FourOfAKind
            }
        } else if max_count == 3 {
            if num_jokers.is_some_and(|&n| n <= 3) {
                let num_jokers = *num_jokers.unwrap();
                if num_jokers == 1 {
                    FourOfAKind
                } else if num_jokers == 2 {
                    FiveOfAKind
                } else if num_jokers == 3 {
                    if count_of_cards.values().any(|&c| c == 2) {
                        FiveOfAKind
                    } else {
                        FourOfAKind
                    }
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
                        FullHouse
                    } else if num_jokers == 2 {
                        FourOfAKind
                    } else {
                        unreachable!("joker counting error")
                    }
                } else {
                    TwoPair
                }
            } else if num_pairs == 1 {
                if num_jokers.is_some() {
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

impl fmt::Display for Hand {
    fn fmt(&self, fmt: &mut fmt::Formatter) -> fmt::Result {
        write!(
            fmt,
            "{} {} {} {:?}",
            self.rank,
            self.get_cards_as_str(),
            self.get_bid(),
            self.get_type(),
        )
    }
}

#[derive(Debug, PartialEq, Eq)]
struct Hands {
    hands: Vec<Hand>,
}

impl Hands {
    pub fn from_file(input: &Path, rules: Rules) -> Self {
        let reader = get_reader(&input);
        let mut hands = Vec::new();
        for line in reader.lines() {
            let line = line.unwrap();
            let hand = Hand::new(&line, rules);
            hands.push(hand);
        }
        Self { hands }
    }

    #[allow(dead_code)]
    pub fn from_slice(hands: &[Hand]) -> Self {
        let hands = hands.to_vec();
        Self { hands }
    }

    pub fn get_ranked(&self) -> Vec<Hand> {
        let mut hands = self.hands.clone();
        hands.sort();
        for (i, hand) in hands.iter_mut().enumerate() {
            hand.rank = (i as Number) + 1;
        }
        hands
    }

    pub fn get_total_winnings(&self, ranked_hands: &Vec<Hand>) -> Number {
        ranked_hands
            .iter()
            .map(|hand| hand.rank * hand.get_bid())
            .sum()
    }

    #[allow(dead_code)]
    pub fn write_to_file(&self, output: &Path) -> std::io::Result<()> {
        let mut out = File::create(output)?;
        for hand in &self.hands {
            writeln!(out, "{}", hand)?;
        }
        Ok(())
    }
}

fn solve(input: &Path, rules: Rules) -> Number {
    let hands = Hands::from_file(input, rules);
    let ranked_hands = hands.get_ranked();
    hands.get_total_winnings(&ranked_hands)
}

fn part01(input: &Path) -> Number {
    solve(input, Rules::Part01)
}

fn part02(input: &Path) -> Number {
    solve(input, Rules::Part02)
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
         What are the new total winnings?\n{}",
        answer
    );
}

//=============================================================================
//================================= Tests =====================================
//=============================================================================

//=============================================================================
// Part-independent tests
//=============================================================================

#[test]
fn test_type_ordering() {
    use Type::*;
    assert!(FiveOfAKind > FourOfAKind);
    assert!(FullHouse < FourOfAKind);
    assert!(FullHouse > HighCard);
    assert!(TwoPair < ThreeOfAKind);
    assert!(TwoPair <= FourOfAKind);
    assert!(FourOfAKind >= FourOfAKind);
    assert!(FiveOfAKind >= FullHouse);
    assert!(HighCard < OnePair);
    assert_eq!(OnePair, OnePair);
    assert_eq!(ThreeOfAKind, ThreeOfAKind);
    assert_eq!(FullHouse, FullHouse);
}

//=============================================================================
// Part 1 tests
//=============================================================================

#[test]
fn test_hand_type_and_bid_part01() {
    use Type::*;
    let test = Hand::new("A2T63 1234", Rules::Part01);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
}

#[test]
fn test_hand_type_kinds_part01() {
    let rules = Rules::Part01;
    use Type::*;
    assert_eq!(Hand::new("A2T63", rules).get_type(), HighCard);
    assert_eq!(Hand::new("4854J", rules).get_type(), OnePair);
    assert_eq!(Hand::new("ATT9A", rules).get_type(), TwoPair);
    assert_eq!(Hand::new("TJTT3", rules).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("666JJ", rules).get_type(), FullHouse);
    assert_eq!(Hand::new("AAAA4", rules).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQQQQ", rules).get_type(), FiveOfAKind);
}

#[test]
fn test_ranking_part01() {
    let rules = Rules::Part01;

    let expected_order: Vec<Hand> = vec!["32T3K", "KTJJT", "KK677", "T55J5", "QQQJA"]
        .iter()
        .enumerate()
        .map(|(i, line)| Hand::new_with_rank(line, rules, (i + 1) as Number))
        .collect();

    let unranked = vec![
        Hand::new("32T3K", rules),
        Hand::new("T55J5", rules),
        Hand::new("KK677", rules),
        Hand::new("KTJJT", rules),
        Hand::new("QQQJA", rules),
    ];
    let ranked = Hands::from_slice(&unranked[..]).get_ranked();

    assert_eq!(ranked, expected_order);
}

#[test]
fn test_example_part01() {
    // source: https://adventofcode.com/2023/day/7
    let expected_answer = 6440;

    let input = Path::new("./inputs/day07.example");
    let answer = part01(&input);

    assert_eq!(answer, expected_answer);
}

/// SPOILER ALERT: This test contains the secret solution for this part!!!
#[test]
fn test_solution_part01() {
    let input = Path::new("./inputs/day07");

    let expected_answer = 248559379;

    let answer = part01(&input);

    assert_eq!(answer, expected_answer);
}

//=============================================================================
// Part 2 tests
//=============================================================================

#[test]
fn test_get_count_of_cards_single_part02() {
    let test = Hand::new("A2T63", Rules::Part02);
    let expected_count_of_cards =
        HashMap::from([(2u8, 1u8), (14u8, 1u8), (10u8, 1u8), (3u8, 1u8), (6u8, 1u8)]);
    let actual_count_of_cards = test.get_count_of_cards();
    assert_eq!(actual_count_of_cards, expected_count_of_cards);
}

#[test]
fn test_get_count_of_cards_multi_part02() {
    let test = Hand::new("Q2JJQ", Rules::Part02);
    let expected_count_of_cards = HashMap::from([(1u8, 2u8), (12u8, 2u8), (2u8, 1u8)]);
    let actual_count_of_cards = test.get_count_of_cards();
    assert_eq!(actual_count_of_cards, expected_count_of_cards);
}

#[test]
fn test_hand_type_and_bid_part02() {
    use Type::*;
    let test = Hand::new("A2T63 1234", Rules::Part02);
    assert_eq!(test.get_type(), HighCard);
    assert_eq!(test.get_bid(), 1234u32);
}

#[test]
fn test_hand_type_kinds_part02_high_card() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("A2T63", rules).get_type(), HighCard);
    assert_eq!(Hand::new("Q4K65", rules).get_type(), HighCard);
}

#[test]
fn test_hand_type_kinds_part02_one_pair() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("234AJ", rules).get_type(), OnePair);
    assert_eq!(Hand::new("2344A", rules).get_type(), OnePair);
}

#[test]
fn test_hand_type_kinds_part02_two_pair() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("ATT9A", rules).get_type(), TwoPair);
}

#[test]
fn test_hand_type_kinds_part02_three_of_a_kind() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("48544", rules).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("KTT9J", rules).get_type(), ThreeOfAKind);
    assert_eq!(Hand::new("JJA59", rules).get_type(), ThreeOfAKind);
}

#[test]
fn test_hand_type_kinds_part02_full_house() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("T3TT3", rules).get_type(), FullHouse);
    assert_eq!(Hand::new("77JTT", rules).get_type(), FullHouse);
    assert_eq!(Hand::new("23J23", rules).get_type(), FullHouse);
}

#[test]
fn test_hand_type_kinds_part02_four_of_a_kind() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("TJTT3", rules).get_type(), FourOfAKind);
    assert_eq!(Hand::new("QQJJA", rules).get_type(), FourOfAKind);
    assert_eq!(Hand::new("JJ2J4", rules).get_type(), FourOfAKind);
    assert_eq!(Hand::new("AAAA4", rules).get_type(), FourOfAKind);
}

#[test]
fn test_hand_type_kinds_part02_five_of_a_kind() {
    let rules = Rules::Part02;
    use Type::*;
    assert_eq!(Hand::new("AAJAA", rules).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("6JJJJ", rules).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("66JJJ", rules).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("666JJ", rules).get_type(), FiveOfAKind);
    assert_eq!(Hand::new("QQQQQ", rules).get_type(), FiveOfAKind);
}

#[test]
fn test_ranking_part02() {
    let rules = Rules::Part02;

    let expected_order: Vec<Hand> = vec!["32T3K", "KK677", "T55J5", "QQQJA", "KTJJT"]
        .iter()
        .enumerate()
        .map(|(i, line)| Hand::new_with_rank(line, rules, (i + 1) as Number))
        .collect();

    let unranked = vec![
        Hand::new("32T3K", rules),
        Hand::new("T55J5", rules),
        Hand::new("KK677", rules),
        Hand::new("KTJJT", rules),
        Hand::new("QQQJA", rules),
    ];
    let ranked = Hands::from_slice(&unranked[..]).get_ranked();

    assert_eq!(ranked, expected_order);
}

#[test]
fn test_individual_hand_cmp_part02() {
    let rules = Rules::Part02;
    assert!(Hand::new("A2T63", rules) < Hand::new("4854J", rules));
    assert!(Hand::new("TJTT3", rules) > Hand::new("69664", rules));
    assert!(Hand::new("66266", rules) > Hand::new("46626", rules));
}

#[test]
fn test_joker_hand_cmp() {
    let rules = Rules::Part02;
    assert!(Hand::new("23J45", rules) < Hand::new("23245", rules));
    assert!(Hand::new("234J5", rules) > Hand::new("23245", rules));
    assert!(Hand::new("TJ9QQ", rules) < Hand::new("Q59QQ", rules));
}

#[test]
fn test_example_part02() {
    // source: https://adventofcode.com/2023/day/7
    let expected_answer = 5905;

    let input = Path::new("./inputs/day07.example");
    let answer = part02(&input);

    assert_eq!(answer, expected_answer);
}

#[test]
fn test_get_cards_as_str_part02() {
    let rules = Rules::Part02;
    assert_eq!(Hand::new("23J45", rules).get_cards_as_str(), "23J45");
    assert_eq!(Hand::new("JAJ6Q", rules).get_cards_as_str(), "JAJ6Q");
    assert_eq!(Hand::new("KAJJA", rules).get_cards_as_str(), "KAJJA");
    assert_eq!(Hand::new("T9876", rules).get_cards_as_str(), "T9876");
}

#[test]
fn test_equal_hands_part02() {
    let rules = Rules::Part02;
    assert!(Hand::new("23J45", rules) == Hand::new("23J45", rules));
}

#[test]
fn test_input_0_part02() {
    // ranked by hand
    let expected_answer = 1 * 467
        + 2 * 537
        + 3 * 897
        + 4 * 27
        + 5 * 340
        + 6 * 528
        + 7 * 98
        + 8 * 4
        + 9 * 948
        + 10 * 572
        + 11 * 839
        + 12 * 997
        + 13 * 610
        + 14 * 71
        + 15 * 608
        + 16 * 504
        + 17 * 574
        + 18 * 229
        + 19 * 447
        + 20 * 50;

    let input = Path::new("./inputs/day07.0");
    let answer = part02(&input);

    assert_eq!(answer, expected_answer);
}

#[test]
fn test_input_1_part02() {
    // ranked by hand
    let expected_answer = 1 * 811
        + 2 * 958
        + 3 * 183
        + 4 * 744
        + 5 * 510
        + 6 * 746
        + 7 * 941
        + 8 * 814
        + 9 * 494
        + 10 * 586
        + 11 * 536
        + 12 * 272
        + 13 * 107
        + 14 * 198
        + 15 * 101
        + 16 * 874
        + 17 * 500
        + 18 * 353
        + 19 * 928
        + 20 * 283;

    let input = Path::new("./inputs/day07.1");
    let answer = part02(&input);

    assert_eq!(answer, expected_answer);
}

#[test]
fn test_input_2_part02() {
    // ranked by hand
    let expected_answer = 1 * 484
        + 2 * 917
        + 3 * 33
        + 4 * 468
        + 5 * 83
        + 6 * 751
        + 7 * 738
        + 8 * 73
        + 9 * 249
        + 10 * 118
        + 11 * 174
        + 12 * 228
        + 13 * 568
        + 14 * 603
        + 15 * 439
        + 16 * 685
        + 17 * 927
        + 18 * 684
        + 19 * 378
        + 20 * 633;

    let input = Path::new("./inputs/day07.2");
    let answer = part02(&input);

    assert_eq!(answer, expected_answer);
}
