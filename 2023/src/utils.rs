/// Common tools to solve the puzzles.
use std::fs::File;
use std::io;
use std::path::Path;

/// Return the file reader instance for the specified `path`.
pub fn get_reader(path: &Path) -> io::BufReader<File> {
    io::BufReader::new(File::open(path).unwrap())
}