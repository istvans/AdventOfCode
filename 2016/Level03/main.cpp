#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <boost/program_options.hpp>

#include <array>

namespace po = boost::program_options;

#define KEYPAD_SIZE 5

namespace
{

int generic_exit(const int error_code_)
{
#ifdef _WIN32
    std::cout << "Press any key to continue...";
    while (!_kbhit()) {}
#endif

    return error_code_;
}

}

int main(int argc_, char** argv_)
{
    std::string input_file;

    po::options_description desc("Options");
    desc.add_options()
        ("input,i", po::value<std::string>(&input_file), "input file")
        ("help,h", "show this help message")
    ;

    po::variables_map vm;
    try {
        po::store(po::parse_command_line(argc_, argv_, desc), vm);
    } catch (const std::exception& ex) {
        std::cout << desc << " - " << ex.what() << std::endl;
        return generic_exit(1);
    }
    po::notify(vm);

    if (vm.count("help") || !vm.count("input")) {
        std::cout << desc << std::endl;
        return generic_exit(2);
    }

    if (auto file = std::fstream{input_file, std::ios_base::in}) {
        size_t possible_triangles{0};
        
        /*
        std::string line;
        while (std::getline(file, line)) {
            std::istringstream iss{line};
            size_t a, b, c;
            if (iss >> a >> b >> c) {
                // std::cout << a << ' ' << b << ' ' << c;
                if (((a + b) > c) && ((a + c) > b) && ((b + c) > a)) {
                    ++possible_triangles;
                }
            } else {
                throw "Unexpected input format! " + line;
            }
            std::cout << std::endl;
        }

        std::cout << "In your puzzle input, how many of the listed triangles are possible? " << possible_triangles << std::endl;
        */

        std::string line;
        std::array<std::array<size_t, 3>, 3>    triangles;
        size_t col_id{0};
        while (std::getline(file, line)) {
            std::istringstream iss{line};
            if (iss >> triangles[0][col_id] >> triangles[1][col_id] >> triangles[2][col_id]) {
                ++col_id;
                if (3 == col_id) {
                    for (const auto& row : triangles) {
                        const size_t& a = row[0];
                        const size_t& b = row[1];
                        const size_t& c = row[2];
                        if (((a + b) > c) && ((a + c) > b) && ((b + c) > a)) {
                            ++possible_triangles;
                        }
                    }
                    col_id = 0;
                }
            } else {
                throw "Unexpected input format! " + line;
            }
            std::cout << std::endl;
        }

        std::cout << "In your puzzle input, and instead reading by columns, "
            "how many of the listed triangles are possible? " << possible_triangles << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
