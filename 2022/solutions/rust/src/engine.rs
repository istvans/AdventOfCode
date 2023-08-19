use crate::puzzle::{Day, Part};

pub trait Engine {
    type Puzzle;
    type Error;
    type Solution;

    fn get_puzzle(&self) -> Result<Self::Puzzle, Self::Error>;
    fn solve(&self, puzzle: Self::Puzzle) -> Result<Self::Solution, Self::Error>;
}

pub fn new(day: Day, part: Part) {}
