/// Utilities for day 4
use std::collections::HashSet;

pub fn slice_up(line: &String) -> Vec<&str> {
    let slices: Vec<&str> = line.split(&[':', '|']).collect();
    assert!(slices.len() == 3);
    slices
}

#[test]
/// Return the expected slices.
fn test_slice_up() {
    let line = String::from("Card  59: 44 80 | 44 33 91");
    let slices = slice_up(&line);
    let expected = Vec::from(["Card  59", " 44 80 ", " 44 33 91"]);
    assert!(slices == expected);
}

pub fn get_winner_numbers(raw_win_nums: &str) -> Vec<usize> {
    raw_win_nums
        .trim()
        .split(' ')
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<usize>().unwrap())
        .collect()
}

pub fn get_my_numbers(raw_my_nums: &str) -> HashSet<usize> {
    raw_my_nums
        .trim()
        .split(' ')
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<usize>().unwrap())
        .collect()
}

pub fn get_num_matches(winner_numbers: &Vec<usize>, my_numbers: &HashSet<usize>) -> u32 {
    winner_numbers
        .iter()
        .filter(|w| my_numbers.contains(w))
        .fold(/*init*/ 0, |acc, _| acc + 1)
}
