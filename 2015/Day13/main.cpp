/** --- Day 13: Knights of the Dinner Table ---

In years past, the holiday feast with your family hasn't gone so well.
Not everyone gets along! This year, you resolve, will be different.
You're going to find the optimal seating arrangement and avoid all those awkward
conversations.

You start by writing up a list of everyone invited and the amount their happiness
would increase or decrease if they were to find themselves sitting next to each
other person. You have a circular table that will be just big enough to fit
everyone comfortably, and so each person will have exactly two neighbors.

For example, suppose you have only four attendees planned, and you calculate their
potential happiness as follows:

Alice would gain 54 happiness units by sitting next to Bob.
Alice would lose 79 happiness units by sitting next to Carol.
Alice would lose 2 happiness units by sitting next to David.
Bob would gain 83 happiness units by sitting next to Alice.
Bob would lose 7 happiness units by sitting next to Carol.
Bob would lose 63 happiness units by sitting next to David.
Carol would lose 62 happiness units by sitting next to Alice.
Carol would gain 60 happiness units by sitting next to Bob.
Carol would gain 55 happiness units by sitting next to David.
David would gain 46 happiness units by sitting next to Alice.
David would lose 7 happiness units by sitting next to Bob.
David would gain 41 happiness units by sitting next to Carol.

Then, if you seat Alice next to David, Alice would lose 2 happiness units
(because David talks so much), but David would gain 46 happiness units
(because Alice is such a good listener), for a total change of 44.

If you continue around the table, you could then seat Bob next to Alice
(Bob gains 83, Alice gains 54). Finally, seat Carol, who sits next to Bob
(Carol gains 60, Bob loses 7) and David (Carol gains 55, David gains 41).

The arrangement looks like this:
     +41 +46
+55   David    -2
Carol       Alice
+60    Bob    +54
     -7  +83

After trying every other seating arrangement in this hypothetical scenario,
you find that this one is the most optimal, with a total change in happiness of 330.

What is the total change in happiness for the optimal seating arrangement of the actual guest list?

https://adventofcode.com/2015/day/13
*/
#include <fstream>
#include <iostream>
#include <regex>

#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/program_options.hpp>
CLANG_DIAG_POP()


int main(int argc_, char** argv_)
{
    namespace po = boost::program_options;

    using puzzle_input_type = std::string;
    puzzle_input_type input_filename;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<puzzle_input_type>(&input_filename), "input file name")
        ("help,h", "show this help message")
        ;

    po::variables_map vm;
    int exit_code = 0;
    try {
        po::store(po::parse_command_line(argc_, argv_, description), vm);
    } catch (const std::exception& ex) {
        std::cout << description << " - " << ex.what() << std::endl;
        exit_code = 1;
    }

    if (exit_code == 0) {
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << description << std::endl;
        } else if (!vm.count("input")) {
            std::cout << description << std::endl;
            exit_code = 2;
        }
    }

    if (exit_code == 0) {
        if (auto file = std::fstream{input_filename, std::ios_base::in}) {
            std::string line;
            while (std::getline(file, line)) {
                std::cout << line << std::endl;

                std::smatch match;
                const auto regex = std::regex("^");
                if(std::regex_search(line, match, regex)) {
                    std::cout << "matches for '" << line << "'\n";
                    std::cout << "Prefix: '" << color_match.prefix() << "'\n";
                    for (size_t i = 0; i < color_match.size(); ++i)
                        std::cout << i << ": " << color_match[i] << '\n';
                    std::cout << "Suffix: '" << color_match.suffix() << "\'\n\n";
                } else {
                    throw std::invalid_argument(input_filename + " does not contain a valid input: '" + line + '\'');
                }
            }
        } else {
            std::cerr << "cannot open input file \'" << input_filename << '\'' << std::endl;
            exit_code = 3;
        }
    }

    return exit_code;
}
