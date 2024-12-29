/// 2024 Day 6
use std::iter::IntoIterator;
use std::thread;
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

#[derive(Debug, Copy, Clone)]
struct Position {
    pub x: i64,
    pub y: i64,
}

impl Position {
    pub fn new(x: i64, y: i64) -> Position {
        Position { x, y }
    }

    pub fn from_indices(x: usize, y: usize) -> Position {
        let x = i64::try_from(x).ok().unwrap();
        let y = i64::try_from(y).ok().unwrap();
        Position { x, y }
    }
}

#[derive(Debug, Clone)]
struct Map {
    map: Vec<Vec<char>>,
    guard: Position,
}

impl Map {
    pub fn new(path: &Path) -> Map {
        let reader = get_reader(path);

        let mut map = vec![];
        let mut guard = Position::new(-1, -1);
        for (y, line) in reader.lines().enumerate() {
            map.push(vec![]);
            let line = line.unwrap();
            for (x, element) in line.chars().enumerate() {
                if element == '^' {
                    guard.x = i64::try_from(x).ok().unwrap();
                    guard.y = i64::try_from(y).ok().unwrap();
                }
                map.last_mut().unwrap().push(element);
            }
        }
        Map { map, guard }
    }

    fn indices_from(position: &Position) -> (usize, usize) {
        let x = usize::try_from(position.x).ok().unwrap();
        let y = usize::try_from(position.y).ok().unwrap();
        (x, y)
    }

    fn get_map_elem(&self, position: &Position) -> char {
        let (x, y) = Self::indices_from(position);
        self.map[y][x]
    }

    pub fn set_map_elem(&mut self, position: Position, value: char) {
        let (x, y) = Self::indices_from(&position);
        self.map[y][x] = value;
    }

    fn set_guard_direction(&mut self, direction: char) {
        self.set_map_elem(self.guard.clone(), direction);
    }

    fn mark_guard_path(&mut self) {
        self.set_map_elem(self.guard.clone(), 'X');
    }

    fn get_target_guard_position(&self, guard: &Position, direction: char) -> Position {
        let mut target_guard_position = guard.clone();
        match direction {
            '^' => target_guard_position.y -= 1,
            'v' => target_guard_position.y += 1,
            '>' => target_guard_position.x += 1,
            '<' => target_guard_position.x -= 1,
            unexpected_direction => panic!(
                "the guard looks to an unexpected direction: {}",
                unexpected_direction
            ),
        }
        target_guard_position
    }

    fn guard_escaped(&self, guard: &Position) -> bool {
        assert!(!self.map.is_empty());
        (guard.x < 0)
            || ((guard.x as usize) >= self.map[0].len())
            || (guard.y < 0)
            || ((guard.y as usize) >= self.map.len())
    }

    fn _show_guard(&self) {
        for y in -1..2 {
            for x in -1..2 {
                let mut position = self.guard;
                position.x += x;
                position.y += y;
                let mut elem = '@';
                if !self.guard_escaped(&position) {
                    elem = self.get_map_elem(&position);
                }
                print!("{} ", elem);
            }
            println!();
        }
        println!("{:?}\n======", self.guard);
    }

    fn detect_loop(index: usize) -> bool {
        index > 10000
    }

    pub fn move_guard(&mut self) -> bool {
        let mut guard_on_map = true;
        let mut index = 0;
        let mut loop_detected = false;
        while guard_on_map {
            if self.guard_escaped(&self.guard) {
                guard_on_map = false;
            } else if Self::detect_loop(index) {
                loop_detected = true;
                break;
            } else {
                let mut direction = self.get_map_elem(&self.guard);
                let mut target_guard_position =
                    self.get_target_guard_position(&self.guard, direction);
                if self.guard_escaped(&target_guard_position) {
                    self.mark_guard_path();
                    guard_on_map = false;
                } else {
                    let mut target_map_elem = self.get_map_elem(&target_guard_position);
                    while target_map_elem == '#' {
                        direction = match direction {
                            '^' => '>',
                            'v' => '<',
                            '>' => 'v',
                            '<' => '^',
                            unexpected_direction => panic!(
                                "the guard looks to an unexpected direction: {}",
                                unexpected_direction
                            ),
                        };
                        target_guard_position =
                            self.get_target_guard_position(&self.guard, direction);
                        if self.guard_escaped(&target_guard_position) {
                            guard_on_map = false;
                            break;
                        }
                        target_map_elem = self.get_map_elem(&target_guard_position);
                    }
                    self.mark_guard_path();
                    self.guard = target_guard_position;
                    self.set_guard_direction(direction);
                }
                index += 1;
            }
        }
        loop_detected
    }

    pub fn count_visited_positions(&self) -> usize {
        let mut count = 0;
        for row in &self.map {
            count += row.iter().filter(|elem| **elem == 'X').count()
        }
        count
    }
}

impl IntoIterator for Map {
    type Item = Vec<char>;
    type IntoIter = std::vec::IntoIter<Self::Item>;

    fn into_iter(self) -> Self::IntoIter {
        self.map.into_iter()
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> usize {
    let mut map = Map::new(path);
    map.move_guard();
    map.count_visited_positions()
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> usize {
    let blueprint_map = Map::new(path);
    let loop_map = blueprint_map.clone();
    let mut threads = vec![];
    for (y, row) in loop_map.into_iter().enumerate() {
        for (x, elem) in row.iter().enumerate() {
            let elem = *elem;
            if elem == '.' {
                let mut alternative_map = blueprint_map.clone();
                alternative_map.set_map_elem(Position::from_indices(x, y), '#');
                let thread = thread::spawn(move || alternative_map.move_guard());
                threads.push(thread);
            }
        }
    }

    let mut loop_counter = 0;
    for thread in threads {
        let looped = thread.join().unwrap();
        if looped {
            loop_counter += 1;
        }
    }
    loop_counter
}

fn main() {
    let path = Path::new("./inputs/day06");

    print_part01_header();
    let answer = part01(&path);
    println!(
        "Determine which updates are already in the correct \
         order. What do you get if you add up the middle page \
         number from those correctly-ordered updates?\n{}",
        answer,
    );

    print_part02_header();
    let answer = part02(&path);
    println!(
        "You need to get the guard stuck in a loop by adding \
         a single new obstruction. How many different positions could \
         you choose for this obstruction?\n{}",
        answer
    );
}

#[test]
fn test_example_part01() {
    let path = Path::new("./inputs/day06.example");
    let actual = part01(&path);
    let expected = 41;
    assert_eq!(actual, expected);
}

#[test]
fn test_example_part02() {
    let path = Path::new("./inputs/day06.example");
    let actual = part02(&path);
    let expected = 6;
    assert_eq!(actual, expected);
}
