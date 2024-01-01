/// 2023 Day 4
use std::collections::HashSet;
use std::io::BufRead;
use std::path::Path;

use utils::get_reader;

/// Return the 3 slices of a line from the input.
/// # Example
/// ```
/// let slices = slice_up("Card  59: 44 80 | 44 33 91");
/// let expected = Vec::from(["Card  59", " 44 80 ", " 44 33 91"]);
/// assert!(slices == expected);
/// ```
/// TODO refactor into a library so the above example can be executed
/// See https://github.com/rust-lang/cargo/issues/5477
fn slice_up(line: &String) -> Vec<&str> {
    let slices: Vec<&str> = line.split(&[':', '|']).collect();
    assert!(slices.len() == 3);
    slices
}

fn get_winner_numbers(raw_win_nums: &str) -> Vec<usize> {
    raw_win_nums
        .trim()
        .split(' ')
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<usize>().unwrap())
        .collect()
}

fn get_my_numbers(raw_my_nums: &str) -> HashSet<usize> {
    raw_my_nums
        .trim()
        .split(' ')
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<usize>().unwrap())
        .collect()
}

fn get_num_matches(winner_numbers: &Vec<usize>, my_numbers: &HashSet<usize>) -> u32 {
    winner_numbers
        .iter()
        .filter(|w| my_numbers.contains(w))
        .fold(/*init*/ 0, |acc, _| acc + 1)
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> usize {
    let reader = get_reader(&path);
    let mut total_points = 0;
    for line in reader.lines() {
        let line = line.unwrap();

        let slices = slice_up(&line);

        let winner_numbers = get_winner_numbers(slices[1]);
        let my_numbers = get_my_numbers(slices[2]);

        let num_matches = get_num_matches(&winner_numbers, &my_numbers);

        if num_matches > 0 {
            let base: usize = 2;
            let points = base.pow(num_matches - 1);
            total_points += points;
        } else {
            // this is a loser card
        }
    }
    total_points
}

#[derive(Clone, Debug)]
struct Card {
    pub count: usize,
}

impl Card {
    pub fn new() -> Self {
        Self { count: 1 }
    }
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> usize {
    let reader = get_reader(&path);
    let num_cards = reader.lines().count();
    let mut cards: Vec<Card> = vec![Card::new(); num_cards];

    let reader = get_reader(&path);
    for (card_id, line) in reader.lines().enumerate() {
        let line = line.unwrap();

        let slices = slice_up(&line);

        let winner_numbers = get_winner_numbers(slices[1]);
        let my_numbers = get_my_numbers(slices[2]);

        let num_matches = get_num_matches(&winner_numbers, &my_numbers);

        let num_matches = usize::try_from(num_matches).unwrap();
        let next_card_id = card_id + 1;
        let extermal_card_id = card_id + num_matches + 1;
        for copy_card_id in next_card_id..extermal_card_id {
            cards[copy_card_id].count += cards[card_id].count;
        }
    }

    cards.iter().map(|c| c.count).sum()
}

fn main() {
    let path = Path::new("./inputs/day04");

    let answer = part01(&path);
    println!(
        "Take a seat in the large pile of colorful cards. \
         How many points are they worth in total? {}",
        answer
    );

    let answer = part02(&path);
    println!(
        "Process all of the original and copied scratchcards until no \
              more scratchcards are won. Including the original set of \
              scratchcards, how many total scratchcards do you end up with? \
              {}",
        answer
    );
}
