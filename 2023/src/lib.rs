/// Common tools to solve the puzzles.
use std::fs::File;
use std::io;
use std::path::Path;

/// Return the file reader instance for the specified `path`.
pub fn get_reader(path: &Path) -> io::BufReader<File> {
    io::BufReader::new(File::open(path).unwrap())
}

fn print_header(title: &str) {
    println!(
        "{}\n========================================================",
        title
    );
}

pub fn print_part01_header() {
    print_header("Part 1:");
}

pub fn print_part02_header() {
    print_header("\nPart 2:");
}