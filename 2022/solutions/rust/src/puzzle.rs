use crate::error::AocError;

use clap::ValueEnum;

/// Each puzzle has two parts.
#[derive(Clone, Debug, ValueEnum)]
pub enum Part {
    /// The first part; solving it unlocks the second part.
    First,
    /// The final, second part.
    Second,
}

pub struct Day(u8);

impl Day {
    pub fn new(day_number: u8) -> Result<Self, Error> {
        if day_number < 1 || day_number > 25 {
            result = Error("{day__number} isn't an advent calendar day");
        } else {
            result = Self(day_number);
        }
        result
    }
}
