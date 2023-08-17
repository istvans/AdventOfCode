pub trait Engine {
    fn get_puzzle(&self) -> Result<Puzzle, Error>;
    fn solve(&self, puzzle: Puzzle) -> Result<String, Error>;
}
