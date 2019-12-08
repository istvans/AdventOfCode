"""
A solution for this level.
"""

from base import PuzzleSolver


def _calculate_fuel(mass):
    """Calculate the fuel requirement for the specified `mass`"""
    return int(mass / 3) - 2


class Level01(PuzzleSolver):
    """The class to solve this level (both parts)"""

    def solve_part1(self, args):
        """Override to solve part 1 of the puzzle"""
        input_file = self.open_input_file(args)
        required_total_fuel = 0
        for line in input_file:
            mass_of_module = int(line.rstrip())
            required_total_fuel += _calculate_fuel(mass_of_module)
        print("What is the sum of the fuel requirements for all "
              "of the modules on your spacecraft?\n{}".format(required_total_fuel))

    def solve_part2(self, args):
        """Override to solve part 2 of the puzzle"""
        input_file = self.open_input_file(args)
        required_total_fuel = 0
        for line in input_file:
            mass_of_module = int(line.rstrip())

            required_fuel = _calculate_fuel(mass_of_module)
            if required_fuel > 0:
                required_total_fuel += required_fuel

                required_fuel_for_fuel = required_fuel
                while True:
                    required_fuel_for_fuel = _calculate_fuel(required_fuel_for_fuel)
                    if required_fuel_for_fuel > 0:
                        required_total_fuel += required_fuel_for_fuel
                    else:
                        break

        print("What is the sum of the fuel requirements for all of the modules on"
              " your spacecraft when also taking into account the mass of the added fuel?\n"
              "(Calculate the fuel requirements for each module separately, then add"
              " them all up at the end.)\n{}".format(required_total_fuel))


if __name__ == "__main__":
    """ This is executed when run from the command line """
    Level01().run()
