/// 2024 Day 4
use std::iter::zip;
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

#[derive(Debug)]
struct Puzzle {
    horizontals: Vec<String>,
    verticals: Vec<String>,
    left_diagonals: Vec<String>, // start from top left
    left_diagonal_coordinates: Vec<Vec<(usize, usize)>>,
    right_diagonals: Vec<String>, // start from top right
    right_diagonal_coordinates: Vec<Vec<(usize, usize)>>,
}

impl Puzzle {
    pub fn new(path: &Path) -> Puzzle {
        let reader = get_reader(path);

        let mut horizontals = Vec::new();
        for line in reader.lines() {
            let line = line.unwrap();
            horizontals.push(line);
        }
        let num_lines = horizontals.len();

        let mut verticals = vec![String::new(); num_lines];
        for line in &horizontals {
            for (i, ch) in line.chars().enumerate() {
                verticals[i].push(ch);
            }
        }

        let num_diagonals = (verticals.len() * 2) - 1;

        let left_diagonal_coordinates: Vec<Vec<(usize, usize)>> =
            Self::get_left_diagonal_coordinates(num_lines).collect();
        let mut left_diagonals = vec![String::new(); num_diagonals];
        for (cindex, coordinates) in left_diagonal_coordinates.iter().enumerate() {
            for (x, y) in coordinates {
                left_diagonals[cindex].push(horizontals[*x].chars().nth(*y).unwrap());
            }
        }

        let right_diagonal_coordinates: Vec<Vec<(usize, usize)>> =
            Self::get_right_diagonal_coordinates(num_lines).collect();
        let mut right_diagonals = vec![String::new(); num_diagonals];
        for (dindex, diagonal) in right_diagonal_coordinates.iter().enumerate() {
            for (x, y) in diagonal {
                right_diagonals[dindex].push(horizontals[*x].chars().nth(*y).unwrap());
            }
        }

        Puzzle {
            horizontals,
            verticals,
            left_diagonals,
            left_diagonal_coordinates,
            right_diagonals,
            right_diagonal_coordinates,
        }
    }

    /* Return the left diagonal coordinates of a size x size matrix.
     *
     * Example:
     * 0,0 1,0 2,0 3,0 4,0
     * 0,1 1,1 2,1 3,1 4,1
     * 0,2 1,2 2,2 3,2 4,2
     * 0,3 1,3 2,3 3,3 4,3
     * 0,4 1,4 2,4 3,4 4,4
     *
     * Left diagonal indices:
     * 0,0
     * 0,1 1,0
     * 0,2 1,1 2,0
     * 0,3 1,2 2,1 3,0
     * 0,4 1,3 2,2 3,1 4,0
     * 1,4 2,3 3,2 4,1
     * 2,4 3,3 4,2
     * 3,4 4,3
     * 4,4
     * * * * * * * * * * */
    fn get_left_diagonal_coordinates(size: usize) -> std::vec::IntoIter<Vec<(usize, usize)>> {
        let mut indices = Vec::new();
        let num_lines = size;
        for max_len in 1..num_lines {
            let mut line = Vec::new();
            let mut len = 0;
            let mut j = max_len - 1;
            for i in 0..num_lines {
                line.push((i, j));
                len += 1;
                if len == max_len {
                    break;
                }
                j -= 1;
            }
            indices.push(line);
        }

        let mut line = Vec::new();
        let mut i = 0;
        for j in (0..num_lines).rev() {
            line.push((i, j));
            i += 1;
        }
        indices.push(line);

        let mut k = 1;
        for max_len in (1..num_lines).rev() {
            let mut len = 0;
            let mut j = num_lines - 1;
            let mut line = Vec::new();
            for i in k..num_lines {
                line.push((i, j));
                len += 1;
                if len == max_len {
                    break;
                }
                j -= 1;
            }
            k += 1;
            indices.push(line);
        }

        indices.into_iter()
    }

    /* Return the right diagonal indices of a size x size matrix.
     *
     * Example:
     * 0,0 1,0 2,0 3,0 4,0
     * 0,1 1,1 2,1 3,1 4,1
     * 0,2 1,2 2,2 3,2 4,2
     * 0,3 1,3 2,3 3,3 4,3
     * 0,4 1,4 2,4 3,4 4,4
     *
     * Right diagonal indices:
     * 4,0
     * 3,0 4,1
     * 2,0 3,1 4,2
     * 1,0 2,1 3,2 4,3
     * 0,0 1,1 2,2 3,3 4,4
     * 0,1 1,2 2,3 3,4
     * 0,2 1,3 2,4
     * 0,3 1,4
     * 0,4
     * * * * * * * * * * */
    fn get_right_diagonal_coordinates(size: usize) -> std::vec::IntoIter<Vec<(usize, usize)>> {
        let mut indices = Vec::new();
        let num_lines = size;

        for max_len in 1..num_lines {
            let mut line = Vec::new();
            for j in 0..max_len {
                let i = num_lines - max_len + j;
                line.push((i, j));
            }
            indices.push(line);
        }

        let mut line = Vec::new();
        let mut i = 0;
        for j in 0..num_lines {
            line.push((i, j));
            i += 1;
        }
        indices.push(line);

        for max_len in (1..num_lines).rev() {
            let mut line = Vec::new();
            for i in 0..max_len {
                let j = num_lines - max_len + i;
                line.push((i, j));
            }
            indices.push(line);
        }

        indices.into_iter()
    }

    fn count_xmas(line: &str) -> u32 {
        let forward = "XMAS";
        let backward = "SAMX";

        let word_len = forward.len();
        let mut count = 0;
        for i in 0..line.len() {
            let j = i + word_len;
            if j <= line.len() {
                if (*forward == line[i..j]) || (*backward == line[i..j]) {
                    count += 1;
                }
            } else {
                break;
            }
        }
        count
    }

    /* Return the X coordinates i.e. the central coordinates of X-MAS's.
     *
     * Example:
     *   0 1 2
     * 0 M . S
     * 1 . A .
     * 2 M . S
     * -> [(1,1)]
     */
    fn get_x_coordinates(
        line: &str,
        coordinates: &Vec<(usize, usize)>,
    ) -> std::vec::IntoIter<(usize, usize)> {
        let forward = "MAS";
        let backward = "SAM";
        let word_len = forward.len();
        let mut x_coordinates = vec![];
        for i in 0..line.len() {
            let j = i + word_len;
            if j <= line.len() {
                if (*forward == line[i..j]) || (*backward == line[i..j]) {
                    x_coordinates.push(coordinates[i + 1]);
                }
            } else {
                break;
            }
        }
        x_coordinates.into_iter()
    }

    pub fn solve_part01(&self) -> u32 {
        let mut count = 0;

        for line in &self.horizontals {
            count += Self::count_xmas(line);
        }

        for line in &self.verticals {
            count += Self::count_xmas(line);
        }

        for line in &self.left_diagonals {
            count += Self::count_xmas(line);
        }

        for line in &self.right_diagonals {
            count += Self::count_xmas(line);
        }

        count
    }

    pub fn solve_part02(&self) -> usize {
        let mut count = 0;

        let mut left_x_coordinates = vec![];
        for (line, coordinates) in zip(&self.left_diagonals, &self.left_diagonal_coordinates) {
            for x_coordinates in Self::get_x_coordinates(line, coordinates) {
                left_x_coordinates.push(x_coordinates);
            }
        }

        let mut right_x_coordinates = vec![];
        for (line, coordinates) in zip(&self.right_diagonals, &self.right_diagonal_coordinates) {
            for x_coordinates in Self::get_x_coordinates(line, coordinates) {
                right_x_coordinates.push(x_coordinates);
            }
        }

        for lx in &left_x_coordinates {
            count += right_x_coordinates.iter().filter(|rx| *lx == **rx).count();
        }

        count
    }
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> u32 {
    let puzzle = Puzzle::new(&path);
    puzzle.solve_part01()
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> usize {
    let puzzle = Puzzle::new(&path);
    puzzle.solve_part02()
}

fn main() {
    let path = Path::new("./inputs/day04");

    print_part01_header();
    let answer = part01(&path);
    println!("How many times does XMAS appear? {}", answer);

    print_part02_header();
    let answer = part02(&path);
    println!("How many times does an X-MAS appear? {}", answer);
}

// TODO testing private static methods...? is this really a good idea?

#[test]
fn test_left_diagonal_indices() {
    let actual_coordinates = Puzzle::get_left_diagonal_coordinates(5);
    let expected_coordinates = vec![
        vec![(0, 0)],
        vec![(0, 1), (1, 0)],
        vec![(0, 2), (1, 1), (2, 0)],
        vec![(0, 3), (1, 2), (2, 1), (3, 0)],
        vec![(0, 4), (1, 3), (2, 2), (3, 1), (4, 0)],
        vec![(1, 4), (2, 3), (3, 2), (4, 1)],
        vec![(2, 4), (3, 3), (4, 2)],
        vec![(3, 4), (4, 3)],
        vec![(4, 4)],
    ];
    for (actual, expected) in zip(actual_coordinates, expected_coordinates) {
        assert_eq!(actual, expected);
    }
}

#[test]
fn test_right_diagonal_indices() {
    let actual_coordinates = Puzzle::get_right_diagonal_coordinates(5);
    let expected_coordinates = vec![
        vec![(4, 0)],
        vec![(3, 0), (4, 1)],
        vec![(2, 0), (3, 1), (4, 2)],
        vec![(1, 0), (2, 1), (3, 2), (4, 3)],
        vec![(0, 0), (1, 1), (2, 2), (3, 3), (4, 4)],
        vec![(0, 1), (1, 2), (2, 3), (3, 4)],
        vec![(0, 2), (1, 3), (2, 4)],
        vec![(0, 3), (1, 4)],
        vec![(0, 4)],
    ];
    for (actual, expected) in zip(actual_coordinates, expected_coordinates) {
        assert_eq!(actual, expected);
    }
}

#[test]
fn test_count_xmas_forward() {
    let actual = Puzzle::count_xmas("..XMAS.XMAS");
    let expected = 2;
    assert_eq!(actual, expected);
}

#[test]
fn test_count_xmas_backward() {
    let actual = Puzzle::count_xmas("SAMX...SAMX");
    let expected = 2;
    assert_eq!(actual, expected);
}

#[test]
fn test_count_xmas_example1() {
    let actual = Puzzle::count_xmas("MMMSXXMASM");
    let expected = 1;
    assert_eq!(actual, expected);
}

#[test]
fn test_count_xmas_example2() {
    let actual = Puzzle::count_xmas("MSAMXMSMSA");
    let expected = 1;
    assert_eq!(actual, expected);
}

#[test]
fn test_solve_small_part01() {
    let path = Path::new("./inputs/day04.small");
    let actual = part01(path);
    let expected = 4;
    assert_eq!(actual, expected);
}

#[test]
fn test_solve_example_part02() {
    let path = Path::new("./inputs/day04.x-mas_example");
    let actual = part02(path);
    let expected = 9;
    assert_eq!(actual, expected);
}
