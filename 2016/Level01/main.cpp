#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <utility>
#include <unordered_map>

#include <conio.h>

#include <boost/program_options.hpp>
#include <boost/functional/hash.hpp>

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

// Only for pairs of std::hash-able types for simplicity.
// You can of course template this struct to allow other hash functions
struct pair_hash
{
    template <class T1, class T2>
    std::size_t operator () (const std::pair<T1, T2>& p_) const
    {
        std::size_t seed = 0;
        boost::hash_combine(seed, p_.first);
        boost::hash_combine(seed, p_.second);
        return  seed;
    }
};

class Position
{
    enum Direction
    {
        North = 0,
        East = 1,
        South = 2,
        West = 3,
        Size,
    };


    void move(unsigned long blocks_, bool left_)
    {
        _dir = (_dir + (left_ ? -1 : 1)) % Size;

        while (blocks_ > 0) {
            switch (_dir) {
                case North:
                {
                    _y -= 1;
                    break;
                }
                case East:
                {
                    _x += 1;
                    break;
                }
                case South:
                {
                    _y += 1;
                    break;
                }
                case West:
                {
                    _x -= 1;
                    break;
                }
                default: throw "Wrong direction! " + _dir;
            }

            auto& visit_counter = _history[std::make_pair(_x, _y)];
            ++visit_counter;

            if (_stop_at_first_point_visited_twice && (2 == visit_counter)) {
                return;
            }

            --blocks_;
        }
    }

    long long                                                                   _x{0};
    long long                                                                   _y{0};
    size_t                                                                      _dir{North};
    
    std::unordered_map<std::pair<long long, long long>, size_t, pair_hash>      _history{{{0, 0}, 0}};
    bool                                                                        _stop_at_first_point_visited_twice;

public:
    explicit Position(const bool stop_at_first_point_visited_twice_ = false)
        : _stop_at_first_point_visited_twice{stop_at_first_point_visited_twice_}
    {}

    void move(std::string&& command_)
    {
        if (command_.empty()) {
            throw "Empty command: " + command_;
        }
        switch (command_[0]) {
            case 'R':
                this->move(std::stoul(std::string{command_.begin()+1, command_.end()}), false);
                break;
            case 'L':
                this->move(std::stoul(std::string{command_.begin() + 1, command_.end()}), true);
                break;
            default:
                throw "Wrong command: " + command_;
        }
    }

    size_t distance(void) const
    {
        return std::abs(_x) + std::abs(_y);
    }

    bool second_visit(void) const
    {
        return 2 == _history.at(std::make_pair(_x, _y));
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
        Position pos{true};
        std::string command;
        while (std::getline(file, command, ' ')) {
            std::cout << command << std::endl;
            pos.move(std::move(command));
            if (pos.second_visit()) {
                break;
            }
        }
        //std::cout << "How many blocks away is Easter Bunny HQ? " << pos.distance() << std::endl;
        std::cout << "How many blocks away is the first location you visit twice? " << pos.distance() << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
