mod engine;
mod error;
mod puzzle;
mod solver;

use clap::Parser;

/// Solve the selected day's puzzle.
#[derive(Parser)]
struct Cli {
    /// The day in the calendar to solve.
    #[arg(short, long)]
    day: Day,
    /// Which part of the puzzle we want to solve.
    #[arg(short, long, value_enum, default_value_t=Part::First)]
    part: Part,
}

fn main() {
    let args = Cli::parse();
    println!("{:?} part of day {}", args.part, args.day);
    let mut solver = engine::new(args.day, args.part);
}
