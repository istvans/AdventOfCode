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

class BathroomCodeFinder
{
    using keypad_line_type          = std::array<unsigned, KEYPAD_SIZE>;

    std::array<keypad_line_type, KEYPAD_SIZE>                 _keypad{
        keypad_line_type{'X',   'X',   1 ,   'X',   'X'},
        keypad_line_type{'X',    2 ,   3 ,    4 ,   'X'},
        keypad_line_type{ 5 ,    6 ,   7 ,    8 ,    9 },
        keypad_line_type{'X',   'A',  'B',   'C',   'X'},
        keypad_line_type{'X',   'X',  'D',   'X',   'X'}
    };

    std::string                                 _code;
    std::pair<int, int>                         _pos{2, 0};

public:
    void process_instructions(std::string&& commands_)
    {
        for (const char& command : commands_) {
            auto old_pos = _pos;
            try {
                switch(command)
                {
                    case 'U':
                    {
                        _pos.first -= 1;
                        break;
                    }
                    case 'L':
                    {
                        _pos.second -= 1;
                        break;
                    }
                    case 'R':
                    {
                        _pos.second += 1;
                        break;
                    }
                    case 'D':
                    {
                        _pos.first += 1;
                        break;
                    }
                    default: throw "Unknown command! " + command;
                }
                auto& key = _keypad.at(_pos.first).at(_pos.second);
                if ('X' == key) {
                    _pos = old_pos;
                }
            } catch (std::out_of_range&) {
                _pos = old_pos;
            }
        }
        
        auto& key = _keypad[_pos.first][_pos.second];
        if (key < 'A') {
            _code += '0' + key;
        } else {
            _code += static_cast<char>(key);
        }
    }

    const std::string& bathroom_code(void) const
    {
        return _code;
    }
};

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
        BathroomCodeFinder finder;
        std::string commands;
        while (std::getline(file, commands)) {
            finder.process_instructions(std::move(commands));
        }

        // std::cout << "What is the bathroom code? " << finder.bathroom_code() << std::endl;
        std::cout << "Using the same instructions in your puzzle input, "
            "what is the correct bathroom code? " << finder.bathroom_code() << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
