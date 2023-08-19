use crate::engine::Engine;

struct Solver<E>
where
    E: Engine,
{
    engine: E,
}

impl<E> Solver<E>
where
    E: Engine,
{
    fn solve(&self) -> Result<E::Solution, E::Error> {
        let puzzle = self.engine.get_puzzle()?;
        self.engine.solve(puzzle)
    }
}
