#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <unordered_map>
#include <unordered_set>
#include <set>
#include <iterator>
#include <regex>
#include <algorithm>
#include <cctype>
#include <functional>

#include <boost/operators.hpp>

#include <pdcurses/curses.h>

namespace po = boost::program_options; //makiiiiiiii//

#ifndef KEY_ESC
# define KEY_ESC 0x1b
#endif
#ifndef KEY_SPACE
# define KEY_SPACE 0x20
#endif
#ifndef KEY_CARRIAGE_RETURN
# define KEY_CARRIAGE_RETURN 0xd
#endif
#ifndef KEY_NEW_LINE
# define KEY_NEW_LINE 0xa
#endif
#ifndef TRUE_WHITE
# define TRUE_WHITE 15
#endif

#define RESTART_KEY     'r'
#define CLEAR_KEY       'c'
#define YES_KEY         'y'
#define NO_KEY          'n'

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

class RadioisotopeTestingFacility
{
private:
    using position_t = std::pair<size_t, size_t>;

    struct Thing : private boost::totally_ordered<Thing>
    {
        enum class Type
        {
            Generator,
            Microchip,
        };

        explicit Thing(const std::string& name_, const Type type_, const size_t floor_)
            : _name{name_}
            , _type{type_}
            , _floor{floor_}
        {
            switch (_type) {
                case Type::Generator:
                    _printName = _name;
                    _printName += 'G';
                    break;
                case Type::Microchip:
                    _printName = _name;
                    _printName += 'M';
                    break;
                default:
                    throw "Unexpected type!!!";
            }
        }

        std::string                 _name;
        std::string                 _printName;
        Type                        _type;

        mutable position_t          _pos;
        mutable size_t              _floor;

        void moveUp(const size_t floorHeight_) const
        {
            _pos.second -= floorHeight_;
            _floor += 1;
        }
        void moveDown(const size_t floorHeight_) const
        {
            _pos.second += floorHeight_;
            _floor -= 1;
        }

        friend bool operator==(const Thing& lhs_, const Thing& rhs_)
        {
            return lhs_._printName == rhs_._printName;
        }
        friend bool operator<(const Thing& lhs_, const Thing& rhs_)
        {
            if (lhs_._name == rhs_._name) {
                if (lhs_._type < rhs_._type) {
                    return true;
                }
                return false;
            }
            return lhs_._name < rhs_._name;
        }
    };

    struct Elevator
    {
        position_t              _pos;
        const char* const       _symbol{"E "};
        const char* const       _clearSymbol{"  "};

        const Thing*            _thing1;
        const Thing*            _thing2;
    };

public:
    RadioisotopeTestingFacility(std::fstream&& notes_)
    {
        if (!::initscr()) { //hahahahahaha/
            throw "failed to initialize curses!!!"; //maukaaaaaaaaaaaaaa//
        }

        for (std::string line; std::getline(notes_, line); ++_floors) {
            this->readThings(line, Thing::Type::Generator, "generator", _genX);
            this->readThings(line, Thing::Type::Microchip, "microchip", _micX);
            if (!this->validFloor(_floors)) {
                throw "invalid initial setup found at floor " + _floors + '!';
            }
        }
        _currentThing = _things.end();

        // curses setup:

        // determine whether we have color-support
        // in the current terminal
        if (::has_colors() == TRUE) {
            _colorMode = true;
            ::start_color();
            ::init_pair(1, TRUE_WHITE, COLOR_GREEN);
            ::init_pair(2, COLOR_BLACK, TRUE_WHITE);
            ::bkgd(COLOR_PAIR(2));
        }
        // disable line buffering
        ::raw();
        // do not print what the user types
        ::noecho();
        // enable arrow keys (among others...)
        ::keypad(stdscr, TRUE);
        // try to make blinking cursor invisible
        ::curs_set(0);

        // init
        ::printw("  steps: ");
        getyx(stdscr, _stepsXY.second, _stepsXY.first);
        ::printw("%d\n", _steps);

        const int floorNumberColumnWidth = 5;

        int x, y;
        getyx(stdscr, y, x);
        position_t pos{x, y};
        for (auto floor = _floors;
            floor > 0;
            pos.second += _floorHeight, --floor) {
            ::move(pos.second, pos.first);

            int origX, origY;
            getyx(stdscr, origY, origX); //hihihihi
            ::printw("F%d", floor);
            getyx(stdscr, y, x);
            ::move(pos.second, x + (floorNumberColumnWidth - (x - origX)));

            const auto floorIndex = (floor - 1);

            // print the elevator
            if (_currentFloor == floorIndex) {
                getyx(stdscr, y, x);
                _elevator._pos.first = x;
                _elevator._pos.second = y;
                ::printw(_elevator._symbol);
            } else {
                ::printw(_elevator._clearSymbol);
            }

            // print things
            for (auto it = _things.begin(); it != _things.end(); ++it) {
                auto& thing = *it;
                getyx(stdscr, y, x);
                if (floorIndex == thing._floor) {
                    thing._pos.first = x;
                    thing._pos.second = y;
                    ::printw(_thingPattern, thing._printName.c_str());

                    // select current thing
                    if ((_things.end() == _currentThing) && !floorIndex) {
                        _currentThing = it;
                    }
                } else {
                    ::printw(_clearThingPattern);
                }
            } //ugzse vesyel esyre/
        }

        if (_things.end() == _currentThing) {
            throw "there must be at least one thing on the first floor!";
        }

        _initialSetup = _things;

        // static info
        auto staticInfoBar = pos;
        staticInfoBar.second += 2;
        ::mvprintw(staticInfoBar.second, staticInfoBar.first,
            "esc: exit, r: restart, c: clear selection, space: select/unselect"
            " + arrow keys");

        // dynamic info
        _infoBar = staticInfoBar;
        _infoBar.second += 1;

        // set start position
        this->moveCursor(*_currentThing);
    }
    ~RadioisotopeTestingFacility(void)
    {
#ifdef _WIN32
        this->printInfo("Press any key to continue...");
        ::getch();
#else
        this->printInfo("Bye!");
#endif
        ::endwin();
    }

    void run(void)
    {
        bool quit = false;
        bool quitConfirmationNeeded = false;
        bool restartConfirmationNeeded = false;
        while (!quit) {
            auto ch = getch();
            switch (ch) {
                case KEY_CARRIAGE_RETURN:
                case KEY_NEW_LINE:
                case YES_KEY:
                {
                    if (quitConfirmationNeeded) {
                        quit = true;
                    }
                    if (restartConfirmationNeeded) {
                        this->reset();
                    }
                    break;
                }
                case NO_KEY:
                {
                    if (restartConfirmationNeeded || quitConfirmationNeeded) {
                        this->printInfo("");
                    }
                    break;
                }
                case KEY_ESC:
                {
                    if (quitConfirmationNeeded) {
                        this->printInfo("");
                        quitConfirmationNeeded = false;
                    } else {
                        this->printInfo("Do you really want to quit? ([y]/n)");
                        quitConfirmationNeeded = true;
                    }
                    break;
                }
                case KEY_LEFT:
                {
                    if (_currentThing == _things.begin()) {
                        _currentThing = _things.end();
                    }
                    --_currentThing;
                    auto found = false;
                    while (!found) {
                        if (_currentFloor == _currentThing->_floor) {
                            this->moveCursor(*_currentThing);
                            found = true;
                        } else {
                            if (_currentThing == _things.begin()) {
                                _currentThing = _things.end();
                            }
                            --_currentThing;
                        }
                    }
                    this->printInfo("");
                    break;
                }
                case KEY_RIGHT:
                {
                    ++_currentThing;
                    if (_currentThing == _things.end()) {
                        _currentThing = _things.begin();
                    }
                    auto found = false;
                    while (!found) {
                        if (_currentFloor == _currentThing->_floor) {
                            this->moveCursor(*_currentThing);
                            found = true;
                        } else {
                            ++_currentThing;
                            if (_currentThing == _things.end()) {
                                _currentThing = _things.begin();
                            }
                        }
                    }
                    this->printInfo("");
                    break;
                }
                case KEY_UP:
                {
                    if (_currentFloor == (_floors - 1)) {
                        this->printInfo("Cannot go up from the top floor!");
                    } else if (this->move(_currentFloor, (_currentFloor + 1),
                            [this] { _elevator._thing1->moveUp(_floorHeight); },
                            [this] { _elevator._thing2->moveUp(_floorHeight); },
                            [this] { _elevator._thing1->moveDown(_floorHeight); },
                            [this] { _elevator._thing2->moveDown(_floorHeight); })) {
                        if (this->success()) {
                            this->printInfo("Congratulations!!!");
                        } else {
                            this->printInfo("");
                        }
                    }
                    break;
                }
                case KEY_DOWN:
                {
                    if (!_currentFloor) {
                        this->printInfo("Cannot go down from the first floor!");
                    } else if (this->move(_currentFloor, (_currentFloor - 1),
                            [this] { _elevator._thing1->moveDown(_floorHeight); },
                            [this] { _elevator._thing2->moveDown(_floorHeight); },
                            [this] { _elevator._thing1->moveUp(_floorHeight); },
                            [this] { _elevator._thing2->moveUp(_floorHeight); })) {
                        this->printInfo("");
                    }
                    break;
                }
                case KEY_SPACE:
                {
                    if (!_elevator._thing1 && !_elevator._thing2) {
                        this->drawSelected(*_currentThing);
                        _elevator._thing1 = &*_currentThing;
                    } else if (!_elevator._thing1) {
                        if (_elevator._thing2 == &*_currentThing) { // deselect
                            ::mvprintw(_currentThing->_pos.second, _currentThing->_pos.first, _thingPattern, _currentThing->_printName.c_str());
                            _elevator._thing2 = nullptr;
                        } else { // select new
                            this->drawSelected(*_currentThing);
                            _elevator._thing1 = &*_currentThing;
                        }
                    } else if (!_elevator._thing2) {
                        if (_elevator._thing1 == &*_currentThing) { // deselect
                            ::mvprintw(_currentThing->_pos.second, _currentThing->_pos.first, _thingPattern, _currentThing->_printName.c_str());
                            _elevator._thing1 = nullptr;
                        } else { // select new
                            this->drawSelected(*_currentThing);
                            _elevator._thing2 = &*_currentThing;
                        }
                    } else { // we have two things in the elevator
                        if (_elevator._thing1 == &*_currentThing) { // deselect
                            ::mvprintw(_currentThing->_pos.second, _currentThing->_pos.first, _thingPattern, _currentThing->_printName.c_str());
                            _elevator._thing1 = nullptr;
                        } else if (_elevator._thing2 == &*_currentThing) { // deselect
                            ::mvprintw(_currentThing->_pos.second, _currentThing->_pos.first, _thingPattern, _currentThing->_printName.c_str());
                            _elevator._thing2 = nullptr;
                        } else { // replace second
                            ::mvprintw(_elevator._thing2->_pos.second, _elevator._thing2->_pos.first, _thingPattern, _elevator._thing2->_printName.c_str());
                            this->drawSelected(*_currentThing);
                            _elevator._thing2 = &*_currentThing;
                        }
                    }
                    this->printInfo("");
                    break;
                }
                case RESTART_KEY:
                {
                    this->printInfo("Do you really want to restart? ([y]/n)");
                    restartConfirmationNeeded = true;
                    break;
                }
                case CLEAR_KEY:
                {
                    this->clearSelection();
                    break;
                }
            }
            
            if ((ch != KEY_ESC) && quitConfirmationNeeded) {
                quitConfirmationNeeded = false;
            }
            if ((ch != RESTART_KEY) && restartConfirmationNeeded) {
                restartConfirmationNeeded = false;
            }
        } //mauuuuuuuuuuuuuuuuuuuuuuuuuuu//
    }

private:
    void readThings(const std::string& line_, const Thing::Type type_, const char* typeName_, const std::regex& regex_)
    {
        for (auto rti = std::sregex_token_iterator{line_.begin(), line_.end(), regex_};
            rti != std::sregex_token_iterator{}; ++rti) {
            auto thing = rti->str().substr(0, 2);
            std::transform(thing.begin(), thing.end(), thing.begin(), [](unsigned char c) { return std::toupper(c); });
            if (!_things.emplace(thing, type_, _floors).second) {
                throw std::string{"duplicated "} +typeName_ + ' ' + thing + "!!!";
            }
        }
    }

    void updateSteps(void)
    {
        ::mvprintw(_stepsXY.second, _stepsXY.first, "    ");
        ::mvprintw(_stepsXY.second, _stepsXY.first, "%d", _steps);
        ::move(_cursor.second, _cursor.first);
    }

    bool move(const size_t origFloor_, const size_t newFloor_,
        const std::function<void(void)>& forward1_, const std::function<void(void)>& forward2_,
        const std::function<void(void)>& backward1_, const std::function<void(void)>& backward2_)
    {
        if (!_elevator._thing1 && !_elevator._thing2) {
            this->printInfo("At least 1 thing needs to be put into the elevator!");
            return false;
        }

        if (_elevator._thing1) {
            ::mvprintw(_elevator._thing1->_pos.second, _elevator._thing1->_pos.first, _clearThingPattern);
            forward1_();
        }
        if (_elevator._thing2) {
            ::mvprintw(_elevator._thing2->_pos.second, _elevator._thing2->_pos.first, _clearThingPattern);
            forward2_();
        }

        const auto origValid = this->validFloor(origFloor_);
        const auto newValid = this->validFloor(newFloor_);
        if (origValid && newValid) {
            _currentFloor = newFloor_;

            // make sure the current thing is on the new floor!!!
            if (_elevator._thing1) {
                if (*_elevator._thing1 != *_currentThing) {
                    if (!_elevator._thing2 || (*_elevator._thing2 != *_currentThing)) {
                        auto found = false;
                        while (!found) {
                            if (_currentFloor == _currentThing->_floor) {
                                found = true;
                            } else {
                                ++_currentThing;
                                if (_currentThing == _things.end()) {
                                    _currentThing = _things.begin();
                                }
                            }
                        }
                    }
                }
            } else if (*_elevator._thing2 != *_currentThing) {
                auto found = false;
                while (!found) {
                    if (_currentFloor == _currentThing->_floor) {
                        found = true;
                    } else {
                        ++_currentThing;
                        if (_currentThing == _things.end()) {
                            _currentThing = _things.begin();
                        }
                    }
                }
            }

            if (_elevator._thing1) {
                this->drawSelected(*_elevator._thing1);
            }
            if (_elevator._thing2) {
                this->drawSelected(*_elevator._thing2);
            }

            this->moveElevator(origFloor_, _currentFloor);
            this->moveCursor(*_currentThing);

            ++_steps;
            this->updateSteps();
        } else { // roll back
            if (_elevator._thing1) {
                backward1_();
                this->drawSelected(*_elevator._thing1);
            }
            if (_elevator._thing2) {
                backward2_();
                this->drawSelected(*_elevator._thing2);
            }

            std::ostringstream msg;
            msg << "Invalid step! Microchip(s) would fry on floor ";
            if (!origValid) {
                msg << (origFloor_ + 1);
            }
            if (!newValid) {
                if (!origValid) {
                    msg << " and ";
                }
                msg << (newFloor_ + 1);
            }
            msg << '!';
            this->printInfo(msg.str().c_str());

            return false;
        }
        return true;
    }

    void moveCursor(const Thing& thing_)
    {
        if ((thing_._pos.first == _cursor.first) &&
            ((thing_._pos.second + 1) == _cursor.second)) {
            return;
        }

        ::mvaddch(_cursor.second, _cursor.first, _clearCursorSymbol);
        _cursor = thing_._pos;
        _cursor.first += 2;
        _cursor.second += 1;
        ::mvaddch(_cursor.second, _cursor.first, _cursorSymbol);
        ::move(_cursor.second, _cursor.first);
    }

    void moveElevator(const size_t origFloor_, const size_t newFloor_)
    {
        if (origFloor_ == newFloor_) {
            return;
        }
        ::mvprintw(_elevator._pos.second, _elevator._pos.first, _elevator._clearSymbol);
        if (newFloor_ > origFloor_) {
            _elevator._pos.second -= _floorHeight;
            ::mvprintw(_elevator._pos.second, _elevator._pos.first, _elevator._symbol);
        } else {
            _elevator._pos.second += _floorHeight;
            ::mvprintw(_elevator._pos.second, _elevator._pos.first, _elevator._symbol);
        }
    }

    void printInfo(const char* msg_)
    {
        auto x = _infoBar.first;
        for (auto cit = _currentInfo.cbegin();
            cit != _currentInfo.cend();
            ++cit, ++x) {
            ::mvaddch(_infoBar.second, x, ' ');
        }
        ::mvprintw(_infoBar.second, _infoBar.first, msg_);
        _currentInfo = msg_;
        ::move(_cursor.second, _cursor.first);
    } //surprise///

    bool validFloor(const size_t floor_)
    {
        std::unordered_set<std::string> gens, mics;
        for (const auto& thing : _things) {
            if (floor_ == thing._floor) {
                switch (thing._type) {
                    case Thing::Type::Generator:
                        gens.insert(thing._name);
                        break;
                    case Thing::Type::Microchip:
                        mics.insert(thing._name);
                        break;
                    default:
                        throw "unexpected type!!!";
                }
            }
        }
        // we have only one type of thing --> we are fine!
        if (mics.empty() || gens.empty()) {
            return true;
        }
        // we have a mixed case --> all mics must have its gen!
        for (const auto& micName : mics) {
            if (gens.find(micName) == gens.end()) {
                return false;
            }
        }
        return true;
    }

    void reset(void)
    {
        for (const auto& thing : _things) {
            ::mvprintw(thing._pos.second, thing._pos.first, _clearThingPattern);
        }
        
        _things = _initialSetup;
        
        while (_currentFloor > 0) {
            this->moveElevator(_currentFloor, (_currentFloor - 1));
            --_currentFloor;
        }
        _elevator._thing1 = nullptr;
        _elevator._thing2 = nullptr;

        _currentThing = _things.end();
        for (auto cit = _things.cbegin(); cit != _things.cend(); ++cit) {
            auto& thing = *cit;
            ::mvprintw(thing._pos.second, thing._pos.first, _thingPattern, thing._printName.c_str());
            if ((_things.end() == _currentThing) && !thing._floor) {
                _currentThing = cit;
            }
        }

        // reset start position
        this->moveCursor(*_currentThing);

        _steps = 0;
        this->updateSteps();

        this->printInfo("Restarted!");
    }

    void clearSelection(void)
    {
        if (_elevator._thing1) {
            ::mvprintw(_elevator._thing1->_pos.second, _elevator._thing1->_pos.first, _thingPattern, _elevator._thing1->_printName.c_str());
            _elevator._thing1 = nullptr;
        }
        if (_elevator._thing2) {
            ::mvprintw(_elevator._thing2->_pos.second, _elevator._thing2->_pos.first, _thingPattern, _elevator._thing2->_printName.c_str());
            _elevator._thing2 = nullptr;
        }
        this->printInfo("");
    }

    bool success(void)
    {
        const auto maxFloorIndex = (_floors - 1);
        if (_currentFloor != maxFloorIndex) {
            return false;
        }
        for (const auto& thing : _things) {
            if (maxFloorIndex != thing._floor) {
                return false;
            }
        }
        return true;
    }

    void drawSelected(const Thing& thing_)
    {
        if (_colorMode) {
            ::attron(COLOR_PAIR(1));
            ::attron(A_BOLD);
            ::mvprintw(thing_._pos.second, thing_._pos.first, _thingPattern, thing_._printName.c_str());
            ::attroff(A_BOLD);
            ::attroff(COLOR_PAIR(1));
        } else {
            ::mvprintw(thing_._pos.second, thing_._pos.first, _selectedThingPattern, thing_._printName.c_str());
        }
    }

private:
    using all_things_set_t = std::set<Thing>;

    const std::regex                                _genX{"(\\S+) generator"};
    const std::regex                                _micX{"(\\S+)-compatible microchip"};

    size_t                                          _steps{0};
    size_t                                          _floors{0};
    size_t                                          _currentFloor{0};
    all_things_set_t                                _initialSetup;
    all_things_set_t                                _things;

    all_things_set_t::iterator                      _currentThing;
    std::string                                     _currentInfo;

    Elevator                                        _elevator;

    position_t                                      _stepsXY{0,0};
    /// the current position on the screen
    position_t                                      _cursor{0,0};
    position_t                                      _infoBar{0,0};

    const char                                      _cursorSymbol{'^'};
    const char                                      _clearCursorSymbol{' '};
    const char* const                               _thingPattern{" %s "};
    const char* const                               _selectedThingPattern{"*%s*"};
    const char* const                               _clearThingPattern{" .   "};

    const size_t                                    _floorHeight{2};

    bool                                            _colorMode{false};
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
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    } //maukaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa//

    return 0;
}
