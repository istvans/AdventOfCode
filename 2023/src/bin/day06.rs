/// 2023 Day 6
use std::io::BufRead;
use std::path::Path;

use raoc_2023::{get_reader, print_part01_header, print_part02_header};

type Number = u64;

fn get_num_ways_to_win(time_ms: Number, best_distance: Number) -> Number {
    let mut num_ways_to_win = 0;
    for button_press_ms in 0..(time_ms + 1) {
        let time_to_move_ms = time_ms - button_press_ms;
        let speed = button_press_ms; // mm/ms
        let distance = speed * time_to_move_ms;
        if distance > best_distance {
            num_ways_to_win += 1;
        }
    }
    num_ways_to_win
}

fn parse_part01(line: &str) -> Vec<Number> {
    line.split(' ')
        .skip(1)
        .filter(|x| !x.is_empty())
        .map(|x| x.parse::<Number>().unwrap())
        .collect()
}

fn part01(input: &Path) -> Number {
    let reader = get_reader(&input);
    let lines: Vec<_> = reader.lines().collect::<Result<_, _>>().unwrap();
    assert!(lines.len() == 2);

    let times = parse_part01(&lines[0]);
    let best_distances = parse_part01(&lines[1]);

    let times_and_best_distances: Vec<(&Number, &Number)> =
        times.iter().zip(best_distances.iter()).collect();

    let mut num_ways_to_win_per_race = Vec::new();
    for (time_ms, best_distance) in times_and_best_distances {
        let num_ways_to_win = get_num_ways_to_win(*time_ms, *best_distance);
        num_ways_to_win_per_race.push(num_ways_to_win);
    }

    num_ways_to_win_per_race.iter().product()
}

fn parse_part02(line: &str) -> Number {
    line.split(':')
        .skip(1)
        .map(|x| x.replace(' ', ""))
        .map(|x| x.parse::<Number>().unwrap())
        .reduce(|_, e| e)
        .unwrap()
}

fn part02(input: &Path) -> Number {
    let reader = get_reader(&input);
    let lines: Vec<_> = reader.lines().collect::<Result<_, _>>().unwrap();
    assert!(lines.len() == 2);

    let time_ms = parse_part02(&lines[0]);
    let best_distance = parse_part02(&lines[1]);

    get_num_ways_to_win(time_ms, best_distance)
}

fn main() {
    let input = Path::new("./inputs/day06");

    print_part01_header();
    let answer = part01(&input);
    println!(
        "Determine the number of ways you could beat the record in each race.\n\
              What do you get if you multiply these numbers together? {}",
        answer
    );

    print_part02_header();
    let answer = part02(&input);
    println!(
        "How many ways can you beat the record in this one much longer race? {}",
        answer
    );
}
