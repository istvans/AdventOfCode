#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <array>

namespace po = boost::program_options;

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

struct Screen
{
    Screen(void)
    {
        for (auto& row : _screen) {
            for (auto& col : row) {
                col = false;
            }
        }
    }

    void operator()(std::string&& command_)
    {
        if (command_.find("rect ") != std::string::npos) {
            auto npos = command_.find_first_of("0123456789");
            if (npos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            
            auto xpos = command_.find('x', npos);
            if (xpos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }

            const auto width = std::stoull(command_.substr(npos, (xpos - npos)));
            ++xpos;
            if (xpos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            const auto height = std::stoull(command_.substr(xpos));

            for (size_t row{0}; (row < height) && (row < _screen.size()); ++row) {
                for (size_t col{0}; (col < width) && (col < _screen[row].size()); ++col) {
                    _screen[row][col] = true;
                }
            }
        } else if (command_.find("rotate row y=") != std::string::npos) {
            auto npos = command_.find_first_of("0123456789");
            if (npos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            auto ppos = command_.find(" by ", npos);
            if (ppos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            const auto row_id = std::stoull(command_.substr(npos, (ppos - npos)));
            
            ppos += 4;
            if (ppos >= command_.size()) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            const auto pixelsRight = std::stoull(command_.substr(ppos));
                
            std::array<bool, width> row;
            for (size_t src{0}, tgt{pixelsRight % width}; src < width; ++src, tgt = ((tgt + 1) % width) ) {
                row[tgt] = _screen[row_id][src];
            }
            for (size_t col{0}; col < width; ++col) {
                _screen[row_id][col] = row[col];
            }
        } else if (command_.find("rotate column x=") != std::string::npos) {
            auto npos = command_.find_first_of("0123456789");
            if (npos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            auto ppos = command_.find(" by ", npos);
            if (ppos == std::string::npos) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            const auto col_id = std::stoull(command_.substr(npos, (ppos - npos)));

            ppos += 4;
            if (ppos >= command_.size()) {
                throw std::string{"wrong command! "} +std::move(command_);
            }
            const auto pixelsDown = std::stoull(command_.substr(ppos));

            std::array<bool, height> column;
            for (size_t src{0}, tgt{pixelsDown % height}; src < height; ++src, tgt = ((tgt + 1) % height)) {
                column[tgt] = _screen[src][col_id];
            }
            for (size_t row{0}; row < height; ++row) {
                _screen[row][col_id] = column[row];
            }
        } else {
            throw std::string{"unknown command! "} +std::move(command_);
        }
    }

    size_t lit_pixels(void) const
    {
        size_t sum{0};
        for (const auto& row : _screen) {
            for (const auto& col : row) {
                if (col) {
                    ++sum;
                }
            }
        }

        return sum;
    }

    template <class OStream>
    friend OStream& operator<< (OStream& lhs_, const Screen& rhs_)
    {
        for (const auto& row : rhs_._screen) {
            for (const auto& col : row) {
                lhs_ << (col ? '+' : ' ');
            }
            lhs_ << "\n";
        }

        return lhs_;
    }

private:
    static const size_t width{50};
    static const size_t height{6};

    std::array<std::array<bool, width>, height>   _screen;
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
        std::string line;
        Screen scr;
        for (std::string line; std::getline(file, line); ) {
            scr(std::move(line));
        }
        std::cout << "There seems to be an intermediate check of the voltage used by the display:\n"
            "after you swipe your card, if the screen did work,\n"
            "how many pixels should be lit? " << scr.lit_pixels() << std::endl;

        std::cout << "After you swipe your card, what code is the screen trying to display?\n"
            << scr << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
