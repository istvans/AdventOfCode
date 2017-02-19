/*
--- Day 13: A Maze of Twisty Little Cubicles ---

You arrive at the first floor of this new building to discover a much less 
welcoming environment than the shiny atrium of the last one.
Instead, you are in a maze of twisty little cubicles, all alike.

Every location in this area is addressed by a pair of non-negative integers 
(x,y). Each such coordinate is either a wall or an open space.
You can't move diagonally. The cube maze starts at 0,0 and seems to extend 
infinitely toward positive x and y; negative values are invalid,
as they represent a location outside the building. You are in a small waiting 
area at 1,1.

While it seems chaotic, a nearby morale-boosting poster explains, the layout 
is actually quite logical. You can determine whether a given x,y coordinate
will be a wall or an open space using a simple system:

  Find x*x + 3*x + 2*x*y + y + y*y.
  Add the office designer's favourite number (your puzzle input).
  Find the binary representation of that sum; count the number of bits that 
  are 1.
    If the number of bits that are 1 is even, it's an open space.
    If the number of bits that are 1 is odd, it's a wall.

For example, if the office designer's favourite number were 10, drawing walls 
as # and open spaces as ., the corner of the building containing 0,0 would
look like this:

  0123456789
0 .#.####.##
1 ..#..#...#
2 #....##...
3 ###.#.###.
4 .##..#..#.
5 ..##....#.
6 #...##.###
Now, suppose you wanted to reach 7,4. The shortest route you could take is 
marked as O:

  0123456789
0 .#.####.##
1 .O#..#...#
2 #OOO.##...
3 ###O#.###.
4 .##OO#OO#.
5 ..##OOO.#.
6 #...##.###
Thus, reaching 7,4 would take a minimum of 11 steps (starting from your 
current location, 1,1).

What is the fewest number of steps required for you to reach 31,39?

Your puzzle input is 1358.

   0 1 2 3 4 5 6 7 8 910111213141516171819202122232425262728293031323334353637383940
 0 
 1   O
 2
 3
 4
 5
 6
 7
 8
 9
10
11
12
13
14
15
16
17
18
19
20
21
22
23
24
25
26
27
28
29
30
31
32
33
34
35
36
37
38
39                                                               X
40
41
42
43
44
45
46
47
48
49
50  
*/
#include "grid.hpp"

#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <boost/program_options.hpp>

#include <boost/optional.hpp>
#include <boost/functional/hash.hpp>

#include <cmath>
#include <bitset>
#include <vector>
#include <unordered_set>
#include <algorithm>
#include <cassert>
#include <type_traits>

#define DEFAULT_STEPS_LIMIT 100
#define DEFAULT_MODE 'g'
#define BYTE 8

#define STRINGIFY(x) #x
#define TO_STRING(x) STRINGIFY(x)

namespace po = boost::program_options;

namespace
{

int generic_exit(const int error_code_, const bool wait_ = true)
{
#ifdef _WIN32
    if (wait_) {
        std::cout << "Press any key to continue...";
        while (std::char_traits<char>::eof() == std::cin.get()) {}
    }
#endif
    return error_code_;
}

class Maze
{
public:    
    using steps_t = long long;
    using solutions_t = std::vector<steps_t>;
    using num_t = int;
    using mode_t = char;

private:
    struct Pos;
    using opt_pos_t = boost::optional<Pos>;
    struct Pos
    {
        enum class Direction {Down, Right, Left, Up, MAX};
        using Direction_utype = std::underlying_type<Direction>::type;

        static const Direction_utype max_dir =
            static_cast<Direction_utype>(Direction::MAX);
        static const Direction_utype first_dir = 0;

        Pos() : _x{std::numeric_limits<num_t>::quiet_NaN()}
            , _y{std::numeric_limits<num_t>::quiet_NaN()}
            , _ndir{Pos::first_dir}
        {}
        
        Pos(const num_t x_, const num_t y_)
            : _x{x_}
            , _y{y_}
            , _ndir{Pos::first_dir}
        {}

        Pos(const Pos& rhs_)
        {
            this->_x = rhs_._x;
            this->_y = rhs_._y;
            this->_ndir = Pos::first_dir;
        }
        Pos& operator=(const Pos& rhs_)
        {
            if (this == &rhs_) {
                return *this;
            }
            this->_x = rhs_._x;
            this->_y = rhs_._y;
            this->_ndir = Pos::first_dir;
            return *this;
        }
        Pos(Pos&& rhs_)                 = default;
        Pos& operator=(Pos&&)           = default;

        void set(const num_t x_, const num_t y_)
        {
            _x = x_;
            _y = y_;
        }

        bool valid() const
        {
            return (_x >= 0) && (_y >= 0);
        }

        opt_pos_t move() const
        {
            _ndir = ((_ndir + 1) % Pos::max_dir);
            if (Pos::first_dir == _ndir) {
                return boost::none;
            }
            return this->move(static_cast<Direction>(_ndir));
        }

        opt_pos_t move(const Direction dir_) const
        {
            Pos np = *this;
            switch (dir_)
            {
                case Direction::Down:
                {
                    np._y += 1;
                    break;
                }
                case Direction::Right:
                {
                    np._x += 1;
                    break;
                }
                case Direction::Left:
                {
                    np._x -= 1;
                    break;
                }
                case Direction::Up:
                {
                    np._y -= 1;
                    break;
                }
                default:
                {
                    throw std::runtime_error{"Unexpected direction!!!"};
                }
            }
            return opt_pos_t{np};
        }

        bool operator== (const Pos& rhs_) const
        {
            return (this->_x == rhs_._x) && (this->_y == rhs_._y);
        }

        template <class Ostream>
        friend Ostream& operator<<(Ostream& lhs_, const Maze::Pos& rhs_)
        {
            return lhs_ << '[' << rhs_._x << ',' << rhs_._y << ']';
        }

        struct Hash
        {
            size_t operator()(const Pos& p_) const 
            {
                auto hash = static_cast<size_t>(0);
                boost::hash_combine(hash, p_._x);
                boost::hash_combine(hash, p_._y);
                return hash;
            }
        };

        num_t               _x;
        num_t               _y;
        
        /// Helper to calculate next direction.
        mutable Direction_utype     _ndir;
    };

    using pos_set_t = std::unordered_set<Pos, Pos::Hash>;

    enum class DrawObj {Start, Target, Position, Accepted, Checking
        , RSteps, RBlacklist, RInvalid, RWall, Finished};
    enum class Info {Steps, Limit, Status, Target, Solution};

    using grid_t = Display::Grid<num_t, DrawObj, Info>;
    using opt_grid_t = boost::optional<grid_t>;

    mode_t              _mode{std::numeric_limits<mode_t>::quiet_NaN()};
    const bool          _just_calc{false};
    const bool          _wait{true};
    const bool          _just_draw_the_map{false};
    num_t               _fav_num{std::numeric_limits<num_t>::quiet_NaN()};
    steps_t             _steps_limit{std::numeric_limits<steps_t>::quiet_NaN()};
    Pos                 _target;
    pos_set_t           _blacklist;
    solutions_t         _solutions;
    opt_grid_t          _opt_grid;

    /** Find x*x + 3*x + 2*x*y + y + y*y.
     * Add the office designer's favourite number (your puzzle input).
     * Find the binary representation of that sum; count the number of bits
     * that are 1.
     *      If the number of bits that are 1 is even, it's an open space.
     *      If the number of bits that are 1 is odd, it's a wall. */
    bool wall(const Pos& p_) const
    {
        if (!p_.valid()) {
            throw std::runtime_error{"Invalid position should have been "
                "filtered out earlier!"};
        }
        const auto number = static_cast<size_t>((p_._x * p_._x) + (3 * p_._x)
            + (2 * p_._x * p_._y) + p_._y + (p_._y * p_._y) + this->_fav_num);
        const auto bits = std::bitset<sizeof(size_t)*BYTE>{number};
        const auto bit1s = bits.count();
        // std::cout << p_ << ' ' << number << ' '<< bits << " #1 bits: " <<
        //      bit1s << std::endl;
        return (0 != (bit1s % 2));
    }

    /** Decide whether @c p_ can be the next step
     * @param[in] p_ The position under checking
     * @param steps_ How many steps we made before this point */
    bool reject(const Pos& p_, const steps_t steps_)
    {
        if (steps_ >= _steps_limit) {
            if (_opt_grid) {
                _opt_grid->draw(DrawObj::RSteps, {p_._x, p_._y});
            }
            return true;
        }
        
        if (_blacklist.cend() != _blacklist.find(p_)) {
            if (_opt_grid) {
                _opt_grid->draw(DrawObj::RBlacklist, {p_._x, p_._y});
            }
            return true;
        }

        if (!p_.valid()) {
            if (_opt_grid) {
                _opt_grid->draw(DrawObj::RInvalid, {p_._x, p_._y});
            }
            return true;
        }

        if (this->wall(p_)) {
            if (_opt_grid) {
                _opt_grid->draw(DrawObj::RWall, {p_._x, p_._y});
            }
            _blacklist.insert(p_); // quicker reject next time
            return true;
        }
        
        return false;
    }

    bool accept(const Pos& p_)
    {
        return (_target == p_);
    }

    opt_pos_t first(const Pos& p_)
    {
        return p_.move(static_cast<Pos::Direction>(Pos::first_dir));
    }

    opt_pos_t next(const Pos& p_)
    {
        return p_.move();
    }

    bool no_solution(const solutions_t::const_iterator sol_) const
    {
        return (_solutions.cend() == sol_);
    }

    steps_t steps(const solutions_t::const_iterator sol_) const
    {
        assert(!this->no_solution(sol_));
        return *sol_;
    }

    /** Draw the map from @c start_ to Maze::_target
     * @param start_ starting point for the drawing */
    void draw_the_map(Pos start_)
    {
        if ((start_._x > _target._x) || (start_._y > _target._y)) {
            throw std::runtime_error("start should be upper left, target "
                "should be lower right corner coordinates!");
        }
        for (auto y = start_._y; y <= _target._y; ++y) {
            for (auto x = start_._x; x <= _target._x; ++x) {
                std::cout << (this->wall({x, y}) ? '#' : '.');
            }
            std::cout << std::endl;
        }
    }

    /** Recursive backtracking search algorithm.
     * It tries to reach the Maze::_target.
     * @param p_ The point that is checked
     * @param steps_ The steps counter */
    void search(Pos p_, steps_t steps_)
    {
        ++steps_;
        if (_opt_grid) {
            _opt_grid->update(Info::Steps, std::to_string(steps_));
            std::ostringstream ost;
            ost << "Checking " << p_;
            _opt_grid->update(Info::Status, ost.str());
            _opt_grid->draw(DrawObj::Checking, {p_._x, p_._y});
        }

        if (this->reject(p_, steps_)) {
            return;
        }

        if (this->accept(p_)) {
            if (_opt_grid) {
                _opt_grid->draw(DrawObj::Accepted, {p_._x, p_._y});
            }
            _solutions.emplace_back(steps_);
        } else if (_opt_grid) {
            _opt_grid->draw(DrawObj::Position, {p_._x, p_._y});
        }
        
        _blacklist.insert(p_); // never look back

        auto s = this->first(p_);
        while (s) {
            this->search(*s, steps_);
            s = this->next(p_);
        }

        if (_opt_grid) {
            _opt_grid->draw(DrawObj::Finished, {p_._x, p_._y});
        }
    }

public:
    Maze(const mode_t mode_, const bool just_calc_
        , const bool do_not_wait_, const bool just_draw_the_map_)
        : _mode{mode_}
        , _just_calc{just_calc_}
        , _wait{!do_not_wait_}
        , _just_draw_the_map{just_draw_the_map_}
    {
        if (!(_just_calc || _just_draw_the_map) && ('g' == _mode)) {
            auto objmap = grid_t::object_map_t{{
                {DrawObj::Start, 'O'}
                ,{DrawObj::Target, 'X'}
                ,{DrawObj::Position, '*'}
                ,{DrawObj::Accepted, 'A'}
                ,{DrawObj::Checking, '.'}
                ,{DrawObj::RSteps, 's'}
                ,{DrawObj::RBlacklist, 'b'}
                ,{DrawObj::RInvalid, 'i'}
                ,{DrawObj::RWall, '#'}
                ,{DrawObj::Finished, 'f'}
            }};
            auto infosetup = grid_t::info_setup_t{{
                {Info::Steps, "steps: ", "--"}
                ,{Info::Limit, "limit: ", "--"}
                ,{Info::Status, "status:", "Initialising..."}
                ,{Info::Target, "target:", "x,y"}
                ,{Info::Solution, "solution:", "--"}
            }};
            _opt_grid.emplace(std::move(objmap), std::move(infosetup), '|'
                , grid_t::time_t{50000}, /*init steps:*/6);
        }
    }
    ~Maze()
    {}

    Maze(const Maze&) = delete;
    Maze& operator=(const Maze&) = delete;
    Maze(Maze&&) = delete;
    Maze& operator=(Maze&&) = delete;

    /** Try to solve the maze and find the shortest path to the target
        location.
        @param limit_ Limit maximum how many steps the solution can be.
        @param fav_num_ The office designer's favourite number.
        @param sx_ The x coordinate of the start position.
        @param sy_ The y coordinate of the start position.
        @param tx_ The x coordinate of the target position.
        @param ty_ The y coordinate of the target position. */
    void solve(const steps_t limit_,
        const num_t fav_num_,
        const num_t sx_, const num_t sy_,
        const num_t tx_, const num_t ty_)
    {
        _fav_num = fav_num_;
        if (_just_calc) {
            const auto p = Pos{sx_, sy_};
            std::cout << p << " is a wall? " <<
                (this->wall(p) ? "true" : "false") << std::endl; 
            generic_exit(0, _wait);
        } else {
            _target.set(tx_, ty_);
            if (_just_draw_the_map) {
                this->draw_the_map({sx_, sy_});
            } else {
                _steps_limit = limit_;
                if (_opt_grid) {
                    _opt_grid->update(Info::Limit
                        , std::to_string(_steps_limit));
                }
                auto steps = static_cast<Maze::steps_t>(-1);

                if (_opt_grid) {
                    _opt_grid->draw(DrawObj::Start, {sx_, sy_});
                    _opt_grid->draw(DrawObj::Target, {_target._x, _target._y});
                    _opt_grid->update(Info::Target, std::to_string(_target._x)
                        + std::string{","}
                        + std::to_string(_target._y));
                    _opt_grid->update(Info::Steps, std::to_string(steps));
                    _opt_grid->update(Info::Status, "Starting...");
                }

                this->search(Pos{sx_, sy_}, steps);
                
                const auto sol = std::min_element(_solutions.cbegin()
                    , _solutions.cend());
                auto answer = (this->no_solution(sol) ? "I cannot answer this "
                    "question." : std::to_string(this->steps(sol)));
                if (_opt_grid) {
                    _opt_grid->update(Info::Status, "Finished");
                    _opt_grid->update(Info::Solution, std::move(answer));
                } else {
                    std::cout << "What is the fewest number of steps required"
                    " for you to reach " << _target._x << ',' << _target._y << "? "
                    << answer << std::endl;
                    generic_exit(0, _wait);
                }
            }
        }
    }
}; // class Maze

}

int main(int argc_, char** argv_)
{
    auto mode = DEFAULT_MODE;
    auto limit = static_cast<Maze::steps_t>(DEFAULT_STEPS_LIMIT);
    auto fav_num = std::numeric_limits<Maze::num_t>::quiet_NaN();
    auto sx = std::numeric_limits<Maze::num_t>::quiet_NaN();
    auto sy = std::numeric_limits<Maze::num_t>::quiet_NaN();
    auto tx = std::numeric_limits<Maze::num_t>::quiet_NaN();
    auto ty = std::numeric_limits<Maze::num_t>::quiet_NaN();
    auto just_calc = false;
    auto do_not_wait_on_windows = false;
    auto just_draw_the_map = false;

    po::options_description desc("Options");
    desc.add_options()
        ("fav,f", po::value<Maze::num_t>(&fav_num)
            , "the office designer's favourite number")
        ("start-x,x", po::value<Maze::num_t>(&sx), "the start x coordinate")
        ("start-y,y", po::value<Maze::num_t>(&sy), "the start y coordinate")
        ("target-x,w", po::value<Maze::num_t>(&tx), "the target x coordinate")
        ("target-y,z", po::value<Maze::num_t>(&ty), "the target y coordinate")
        ("limit,l", po::value<Maze::steps_t>(&limit)
            , "the steps limit [default: " TO_STRING(DEFAULT_STEPS_LIMIT) "]")
        ("mode,m", po::value<char>(&mode)
            , "mode: s - simple, g - graphical [default: "
              TO_STRING(DEFAULT_MODE) "]")
        ("just-calc,j", "just calculate whether start-x and start-y would "
                        "be a wall")
        ("dont-wait,d", "do not wait for any key to continue on windows")
        ("map,a", "draw the map from x,y to w,z and quit")
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

    just_calc = (vm.count("just-calc") != 0);
    do_not_wait_on_windows = (vm.count("dont-wait") != 0);
    just_draw_the_map = (vm.count("map") != 0);

    if (vm.count("help") || !vm.count("fav")
    || !vm.count("start-x") || !vm.count("start-y")
    || (!just_calc
     && (!vm.count("target-x") || !vm.count("target-y") || !(('g' == mode)
      || ('s' == mode))))
    || (just_draw_the_map && (!vm.count("target-x")
      || !vm.count("target-y")))
      ) {
        std::cout << desc << std::endl;
        return generic_exit(2);
    }

    Maze m{mode, just_calc, do_not_wait_on_windows, just_draw_the_map};
    m.solve(limit, fav_num, sx, sy, tx, ty);
    
    return 0;
}
