/// 2024 Day 7
use std::thread;
use std::{io::BufRead, path::Path};

use raoc_2024::{get_reader, print_part01_header, print_part02_header};

type Number = u64;

fn convert_to_number(string: &str) -> Number {
    string.parse::<Number>().unwrap()
}

fn parse_value_and_operands(line: String) -> (Number, Vec<Number>) {
    let value_and_operands: Vec<&str> = line.split(':').collect();

    let value = value_and_operands[0];
    let value = convert_to_number(value);

    let operands = value_and_operands[1].trim();
    let operands: Vec<Number> = operands.split(' ').map(|x| convert_to_number(x)).collect();

    (value, operands)
}

fn get_num_combinations(num_operator_kinds: usize, num_operators_in_equation: usize) -> usize {
    let mut num_combinations = num_operator_kinds;
    for _ in 0..num_operators_in_equation {
        num_combinations *= num_operator_kinds;
    }
    num_combinations
}

fn to_base3_vec(n: usize, len: usize) -> Vec<usize> {
    let mut n = n;
    let mut number = vec![];
    if n == 0 {
        number.push(0);
    } else {
        while n > 0 {
            let digit = n % 3;
            number.push(digit);
            n /= 3;
        }
        number.reverse();
    }
    while number.len() < len {
        number.insert(0, 0);
    }
    number
}

fn get_all_operation_symbols(
    num_combinations: usize,
    operator_kinds: &Vec<char>,
    num_operators_in_equation: usize,
) -> Vec<Vec<char>> {
    let mut all_operation_symbols = vec![];
    let num_operator_kinds = operator_kinds.len();

    if num_operator_kinds == 2 {
        // part 1
        for combination in 0..num_combinations {
            let mut op = vec!['.'; num_operators_in_equation];
            for i in 0..num_operators_in_equation {
                let ops_id = (combination >> i) & 1;
                op[i] = operator_kinds[ops_id];
            }
            all_operation_symbols.push(op);
        }
    } else if num_operator_kinds == 3 {
        // part 2
        for combination in 0..num_combinations {
            let mut op = vec!['.'; num_operators_in_equation];
            let base3 = to_base3_vec(combination, num_operators_in_equation);
            for i in 0..num_operators_in_equation {
                let ops_id = base3[i];
                op[i] = operator_kinds[ops_id];
            }
            all_operation_symbols.push(op);
        }
    } else {
        panic!(
            "unexpected number of operator kinds! {}",
            num_operator_kinds,
        );
    }

    all_operation_symbols
}

fn is_valid(
    operands: &Vec<Number>,
    all_operation_symbols: Vec<Vec<char>>,
    expected_result: Number,
) -> bool {
    let mut valid = false;
    for operation_symbols in all_operation_symbols {
        let mut result = 0;
        for (i, operand) in operands.iter().enumerate() {
            let operand = *operand;
            if i == 0 {
                result = operand;
            } else {
                let operation_symbol = operation_symbols[i - 1];
                result = match operation_symbol {
                    '+' => result + operand,
                    '*' => result * operand,
                    '|' => {
                        // we do not use this operation in part 1, but it does not cause any
                        // problem when calculating part 1 values. the only down side would
                        // be if part 1 could contain these symbols, but it can't.
                        // so we keep it simple here and just re-use the part 2 calculation as is.
                        let concatenated = result.to_string() + &operand.to_string();
                        convert_to_number(&concatenated)
                    }
                    wtf => panic!("{} WTF?!", wtf),
                };
            }
        }
        if result == expected_result {
            valid = true;
            break;
        }
    }
    valid
}

/// Return the answer to the first part of the puzzle.
fn part01(path: &Path) -> Number {
    let reader = get_reader(path);

    let ops = vec!['+', '*'];
    let num_operator_kinds = ops.len();

    let mut total_calibration_result = 0;
    for line in reader.lines() {
        let line = line.unwrap();

        let (value, operands) = parse_value_and_operands(line);

        let num_operators_in_equation = operands.len() - 1;
        let num_combinations = get_num_combinations(num_operator_kinds, num_operators_in_equation);

        let all_operation_symbols =
            get_all_operation_symbols(num_combinations, &ops, num_operators_in_equation);

        let valid = is_valid(&operands, all_operation_symbols, value);

        if valid {
            total_calibration_result += value;
        }
    }

    total_calibration_result
}

/// Return the answer to the second part of the puzzle.
fn part02(path: &Path) -> Number {
    let reader = get_reader(path);

    let ops = vec!['+', '*', '|'];
    let num_operator_kinds = ops.len();
    let mut total_calibration_result = 0;

    let mut threads = vec![];
    for line in reader.lines() {
        let line = line.unwrap();

        let (value, operands) = parse_value_and_operands(line);

        let num_operators_in_equation = operands.len() - 1;
        let num_combinations = get_num_combinations(num_operator_kinds, num_operators_in_equation);

        let all_operation_symbols =
            get_all_operation_symbols(num_combinations, &ops, num_operators_in_equation);

        let handle = thread::spawn(move || {
            let valid = is_valid(&operands, all_operation_symbols, value);
            (valid, value)
        });
        threads.push(handle);
    }

    for handle in threads {
        let (valid, caliration_result) = handle.join().unwrap();
        if valid {
            total_calibration_result += caliration_result;
        }
    }

    total_calibration_result
}

fn main() {
    let path = Path::new("./inputs/day07");

    print_part01_header();
    let answer = part01(&path);
    println!(
        "Determine which equations could possibly be true. \
         What is their total calibration result?\n{}",
        answer,
    );

    print_part02_header();
    println!(
        "Using your new knowledge of elephant hiding spots, \
         determine which equations could possibly be true. \
         What is their total calibration result?"
    );
    let answer = part02(&path);
    println!("{}", answer);
}
