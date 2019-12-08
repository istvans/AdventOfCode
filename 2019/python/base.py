"""
The base module that supports solving each level.
"""

__author__ = "István Siroki"
__version__ = "0.1.0"
__license__ = "MIT"


import abc
import argparse


class PuzzleSolver(abc.ABC):
    """The abstract puzzle solver base class with all the common functionality
    and boilerplate codes to ease puzzle solving.
    """

    def __init__(self):
        """Create the argument parser that can be modified by derived classes"""
        self.parser = argparse.ArgumentParser()

    def open_input_file(self, args):
        """Open the input file and return its handle"""
        return open(args.input, "r")

    def solve_part1(self, args):
        """This function is expected to solve the first part of the puzzle"""
        print("Not yet implemented. Override {}".format(self.solve_part1.__name__))

    def solve_part2(self, args):
        """This function is expected to solve the second part of the puzzle"""
        print("Not yet implemented. Override {}".format(self.solve_part2.__name__))

    def __main(self, args):
        """The simple backbone of each level's puzzle-solver"""
        if args.part1:
            self.solve_part1(args)
        if args.part2:
            self.solve_part2(args)
        if not args.part1 and not args.part2:
            print("Please specify --part1/-1 and/or --part2/-2 to solve one or"
                  " both parts of the puzzle")

    def run(self):
        """Parse the command-line arguments and run `main`"""
        # Mandatory arguments which requires a parameter
        self.parser.add_argument("-i", "--input", required=True, action="store", dest="input",
                            help="The input file")

        # Optional argument flags which defaults to False
        self.parser.add_argument("-1", "--part1", action="store_true", dest="part1", default=False)
        self.parser.add_argument("-2", "--part2", action="store_true", dest="part2", default=False)

        # Optional verbosity counter
        self.parser.add_argument("-v", "--verbose", action="count", default=0,
                            help="Verbosity (-v, -vv, etc)")

        # Specify output of "--version"
        self.parser.add_argument("--version", action="version",
                            version="%(prog)s (version {version})".format(version=__version__))

        args = self.parser.parse_args()
        self.__main(args)
