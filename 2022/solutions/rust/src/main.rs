use clap::{Parser, ValueEnum};

/// Each puzzle has two parts.
#[derive(Clone, Debug, ValueEnum)]
enum Part {
    /// The first part; solving it unlocks the second part.
    First,
    /// The final, second part.
    Second,
}

/// Solve the selected day's puzzle.
#[derive(Parser)]
struct Cli {
    /// The day in the calendar to solve.
    #[arg(short, long)]
    day: u8,
    /// Which part of the puzzle we want to solve.
    #[arg(short, long, value_enum, default_value_t=Part::First)]
    part: Part,
}

fn main() {
    let args = Cli::parse();
    println!("{:?} part of day {}", args.part, args.day);
}
