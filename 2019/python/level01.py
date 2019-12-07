"""
A solution for this level.
"""

__author__ = "István Siroki"
__version__ = "0.1.0"
__license__ = "MIT"

import argparse


def calculate_fuel(mass):
    """Calculate the fuel requirement for the specified `mass`"""
    return int(mass / 3) - 2


def open_input(args):
    """Open the input file and return its handle"""
    return open(args.input, "r")


def solve_part1(args):
    """Solve part 1 of the puzzle"""
    input_file = open_input(args)
    required_total_fuel = 0
    for line in input_file:
        mass_of_module = int(line.rstrip())
        required_total_fuel += calculate_fuel(mass_of_module)
    print("What is the sum of the fuel requirements for all "
          "of the modules on your spacecraft?\n{}".format(required_total_fuel))


def solve_part2(args):
    """Solve part 2 of the puzzle"""
    input_file = open_input(args)
    required_total_fuel = 0
    for line in input_file:
        mass_of_module = int(line.rstrip())

        required_fuel = calculate_fuel(mass_of_module)
        if required_fuel > 0:
            required_total_fuel += required_fuel

            required_fuel_for_fuel = required_fuel
            while True:
                required_fuel_for_fuel = calculate_fuel(required_fuel_for_fuel)
                if required_fuel_for_fuel > 0:
                    required_total_fuel += required_fuel_for_fuel
                else:
                    break

    print("What is the sum of the fuel requirements for all of the modules on"
          " your spacecraft when also taking into account the mass of the added fuel?\n"
          "(Calculate the fuel requirements for each module separately, then add"
          " them all up at the end.)\n{}".format(required_total_fuel))


def main(args):
    """ Main entry point of the app """
    if args.part1:
        solve_part1(args)
    if args.part2:
        solve_part2(args)
    if not args.part1 and not args.part2:
        print("Please specify --part1 and/or --part2 to solve one or both parts of the puzzle")


if __name__ == "__main__":
    """ This is executed when run from the command line """
    parser = argparse.ArgumentParser()

    # Mandatory argument which requires a parameter
    parser.add_argument("-i", "--input", required=True, action="store", dest="input",
                        help="The input file")

    # Optional argument flags which defaults to False
    parser.add_argument("-1", "--part1", action="store_true", dest="part1", default=False)
    parser.add_argument("-2", "--part2", action="store_true", dest="part2", default=False)

    # Optional verbosity counter (eg. -v, -vv, -vvv, etc.)
    parser.add_argument(
        "-v",
        "--verbose",
        action="count",
        default=0,
        help="Verbosity (-v, -vv, etc)")

    # Specify output of "--version"
    parser.add_argument(
        "--version",
        action="version",
        version="%(prog)s (version {version})".format(version=__version__))

    args = parser.parse_args()
    main(args)