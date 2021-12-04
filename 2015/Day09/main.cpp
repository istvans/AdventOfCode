/** --- Day 9: All in a Single Night ---

Every year, Santa manages to deliver all of his presents in a single night.

This year, however, he has some new locations to visit; his elves have provided him the distances
between every pair of locations. He can start and end at any two (different) locations he wants,
but he must visit each location exactly once. What is the shortest distance he can travel to
achieve this?

For example, given the following distances:

London to Dublin = 464
London to Belfast = 518
Dublin to Belfast = 141
The possible routes are therefore:

Dublin -> London -> Belfast = 982
London -> Dublin -> Belfast = 605
London -> Belfast -> Dublin = 659
Dublin -> Belfast -> London = 659
Belfast -> Dublin -> London = 605
Belfast -> London -> Dublin = 982
The shortest of these is London -> Dublin -> Belfast = 605, and so the answer is 605 in this
example.

What is the distance of the shortest route?
===========================================
Research:
In the mathematical field of graph theory, a Hamiltonian path (or traceable path)
is a path in an undirected or directed graph that visits each vertex exactly once.

*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/program_options.hpp>
CLANG_DIAG_POP()

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

namespace po = boost::program_options;


int main(int argc_, char** argv_)
{
    std::string input_file;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<std::string>(&input_file), "input file")
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
        if (auto file = std::fstream{input_file, std::ios_base::in}) {
            (void)file;
        } else {
            std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
            exit_code = 3;
        }
    }

    return exit_code;
}
