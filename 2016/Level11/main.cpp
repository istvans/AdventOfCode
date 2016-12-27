#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <unordered_map>
#include <set>
#include <iterator>
#include <regex>
#include <algorithm>
#include <cctype>

#include <pdcurses\curses.h>

namespace po = boost::program_options;

#ifndef KEY_ESC
# define KEY_ESC 0x1b
#endif
#ifndef KEY_SPACE
# define KEY_SPACE 0x20
#endif

namespace
{

int generic_exit(const int error_code_)
{
#ifdef _WIN32
    ::printw("Press any key to continue...");
    ::getch();
#endif

    return error_code_;
}

struct RadioisotopeTestingFacility
{
    RadioisotopeTestingFacility(std::fstream&& notes_)
    {
        if (!::initscr()) {
            throw "failed to initialize curses!!!";
        }

        for (std::string line; std::getline(notes_, line); ++_floors) {
            _generators.push_back(floor_things_list_t::value_type{});
            for (auto gti = std::sregex_token_iterator{line.begin(), line.end(), _genX};
                gti != std::sregex_token_iterator{}; ++gti) {
                auto gen = gti->str().substr(0, 2);
                std::transform(gen.begin(), gen.end(), gen.begin(), [](unsigned char c) { return std::toupper(c); });
                std::cout << "found " << gen << " generator on floor " << _floors << std::endl;
                if (!_allGenerators.emplace(gen).second) {
                    throw "duplicated generator " + gen + "!!!";
                }
                if (!_generators[_floors].emplace(gen).second) {
                    throw "duplicated generator on floor " + _floors + ' ' + gen + "!!!";
                }
                
            }
            _microchips.push_back(floor_things_list_t::value_type{});
            for (auto mti = std::sregex_token_iterator{line.begin(), line.end(), _micX};
                mti != std::sregex_token_iterator{}; ++mti) {
                auto mic = mti->str().substr(0, 2);
                std::transform(mic.begin(), mic.end(), mic.begin(), [](unsigned char c) { return std::toupper(c); });
                std::cout << "found " << mic << "-compatible microchip on floor " << _floors << std::endl;
                if (!_microchips[_floors].emplace(mic).second) {
                    throw "duplicated microchip on floor " + _floors + ' ' + mic + "!!!";
                }
                if (!_allMicrochips.emplace(mic).second) {
                    throw "duplicated microchip " + mic + "!!!";
                }
            }
        }
        std::cout << "found floors " << _floors << std::endl;
        
        // disable line buffering
        ::raw();
        // do not print what the user types
        ::noecho();
        // enable arrow keys (among others...)
        ::keypad(stdscr, TRUE);

        // init
        ::printw("steps: ");
        getyx(stdscr, _stepsXY.second, _stepsXY.first);
        ::printw("%d", _steps);

        const int width = 5;
        // |[TUG,TUM]|
        // |[...,...]|
        // |.........|
        const int elevatorWidth = 11;

        position_t pos{0, (_stepsXY.second + 1)};
        int origX, origY, x, y;
        int firstGenX, firstMicX;

        _thingsFloorPositions.resize(_floors);
        _cursorFloorPositions.resize(_floors);
        for (size_t floor{_floors}; floor > 0; pos.second += _floorHeight, --floor) {
            ::move(pos.second, pos.first);
            getyx(stdscr, origY, origX);
            ::printw("%d", floor);
            getyx(stdscr, y, x);
            ::move(pos.second, x + (width - (x - origX)));

            const auto floorIndex = (floor - 1);
            _thingsFloorPositions[floorIndex] = pos;

            // print the elevator
            if (_currentFloor == floorIndex) {
                ::printw("|[...,...]|");
            } else {
                ::printw("|.........|");
            }
            getyx(stdscr, y, x);
            
            firstGenX = x + 1;
            ::move(y, firstGenX);

            _cursorFloorPositions[floorIndex] = position_t{firstGenX, (y + 1)};

            // print generators
            const auto& genFloor = _generators[floorIndex];
            auto git = genFloor.begin();
            for (const auto& gen : _allGenerators) {
                getyx(stdscr, y, x);
                gen._x = x;
                if ((git != genFloor.end()) && (*git == gen._name)) {
                    ::printw("  %s  ", gen._name.c_str());
                    ++git;
                } else {
                    ::printw("  .  ");
                }
            }

            getyx(stdscr, y, x);
            firstMicX = x;
            // print microchips
            const auto& micFloor = _microchips[floorIndex];
            auto mit = micFloor.begin();
            for (const auto& mic : _allMicrochips) {
                getyx(stdscr, y, x);
                mic._x = x;
                if ((mit != micFloor.end()) && (*mit == mic._name)) {
                    ::printw(" %s  ", mic._name.c_str());
                    ++mit;
                } else {
                    ::printw("  .  ");
                }
            }
        }
        // info
        ::move(pos.second, pos.first);
        ::printw("F    E");
        ::mvaddch(pos.second, firstGenX, 'G');
        ::mvaddch(pos.second, firstMicX, 'M');

        // set start position
        _cursor = _cursorFloorPositions[0];
        ::mvaddch(_cursor.second, _cursor.first, _cursorSymbol);

        //::refresh();
    }
    ~RadioisotopeTestingFacility(void)
    {
        ::endwin();
    }

    void run(void)
    {
        int ch = 0;
        while ((ch = getch()) != KEY_ESC) {
            switch (ch) {
                case KEY_LEFT:
                {
                    ::mvaddch(_cursor.second, _cursor.first, ' ');
                    _cursor.first -= _stepWidth;
                    ::mvaddch(_cursor.second, _cursor.first, _cursorSymbol);
                    break;
                }
                case KEY_RIGHT:
                {
                    ::mvaddch(_cursor.second, _cursor.first, ' ');
                    _cursor.first += _stepWidth;
                    ::mvaddch(_cursor.second, _cursor.first, _cursorSymbol);
                    break;
                }
                case KEY_SPACE:
                {
                    break;
                }
            }
        }
    }

private:
    struct Elevator
    {
        std::string _thing1;
        std::string _thing2;
    };

    struct Thing
    {
        explicit Thing(const std::string& name_)
            : _name{name_}
        {}

        std::string     _name;
        mutable int     _x;

        friend bool operator==(const Thing& lhs_, const Thing& rhs_)
        {
            return lhs_._name == rhs_._name;
        }

        friend bool operator<(const Thing& lhs_, const Thing& rhs_)
        {
            return lhs_._name < rhs_._name;
        }
    };

private:
    using position_t            = std::pair<size_t, size_t>;
    using map_t                 = std::vector<std::map<position_t, Thing>>;
    using all_things_set_t      = std::set<Thing>;
    
    const std::regex                                _genX{"(\\S+) generator"};
    const std::regex                                _micX{"(\\S+)-compatible microchip"};

    size_t                                          _steps{0};
    size_t                                          _floors{0};
    size_t                                          _currentFloor{0};
    all_things_set_t                                _allGenerators;
    all_things_set_t                                _allMicrochips;
    floor_things_list_t                             _generators;
    floor_things_list_t                             _microchips;
    Elevator                                        _elevator;

    position_t                                      _stepsXY{0,0};
    /// the current position on the screen
    position_t                                      _cursor{0,0};
    char                                            _cursorSymbol{'^'};

    const size_t                                    _floorHeight{2};
    const size_t                                    _stepWidth{5};
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
        RadioisotopeTestingFacility rtf{std::move(file)};
        rtf.run();
        
        std::cout << "In your situation, what is "
            "the minimum number of steps required "
            "to bring all of the objects to the "
            "fourth floor?" << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
