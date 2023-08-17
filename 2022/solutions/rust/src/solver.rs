mod solver {
    struct Error;

    struct Solver<Engine> {
        engine: Engine,
    }

    impl<Engine> Solver<Engine> {
        fn solve(&self) -> Result<String, Error> {
            self.engine.solve(self.engine.get_puzzle())
        }
    }
}
