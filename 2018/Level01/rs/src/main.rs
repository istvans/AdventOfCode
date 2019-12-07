use std::env;
use std::fs::File;
use std::io::{BufRead, BufReader, Error, ErrorKind};
use std::collections::HashSet;


fn main() {
    let args: Vec<String> = env::args().collect();
    if args.len() == 2 {
        let file_name = &args[1];
        process(&file_name);
    } else {
        eprintln!("Usage: {} [file_name]", args[0]);
    }
}

//==============================================================================

fn get_numbers(file_name: &String) -> Result<Vec<i32>, Error> {
    let input = File::open(file_name).unwrap();
    let reader = BufReader::new(input);
    reader.lines()
          .map(|line_read| line_read.and_then(|line| line.parse().map_err(|e| Error::new(ErrorKind::InvalidData, e))))
          .collect()
}

fn process(file_name: &String) {
    println!("Processing {}...", file_name);
    let numbers = get_numbers(file_name).unwrap();

    let mut frequency = 0;
    let mut frequency_history: HashSet<i32> = HashSet::new();
    frequency_history.insert(frequency);

    let mut first_repeating_frequency_found = false;
    let mut first_iteration_is_over = false;
    while !first_repeating_frequency_found {
        for change in &numbers {
            frequency += change;

            let new_frequency = frequency_history.insert(frequency);
            if !(new_frequency || first_repeating_frequency_found) {
                println!("The first repeating frequency is {}.", frequency);
                first_repeating_frequency_found = true;
            }
        }

        if !first_iteration_is_over {
            println!("The resulting frequency is {} after {} changes (1 iteration).", frequency
                , (frequency_history.len() - 1));
            first_iteration_is_over = true;
        }
    }
}
