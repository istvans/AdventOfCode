/**
--- Day 6: Probably a Fire Hazard ---

Because your neighbors keep defeating you in the holiday house decorating contest year after year, you've decided to deploy one million lights in a 1000x1000 grid.

Furthermore, because you've been especially nice this year, Santa has mailed you instructions on how to display the ideal lighting configuration.

Lights in your grid are numbered from 0 to 999 in each direction; the lights at each corner are at 0,0, 0,999, 999,999, and 999,0. The instructions include whether to turn on, turn off, or toggle various inclusive ranges given as coordinate pairs. Each coordinate pair represents opposite corners of a rectangle, inclusive; a coordinate pair like 0,0 through 2,2 therefore refers to 9 lights in a 3x3 square. The lights all start turned off.

To defeat your neighbors this year, all you have to do is set up your lights by doing the instructions Santa sent you in order.

For example:

turn on 0,0 through 999,999 would turn on (or leave on) every light.
toggle 0,0 through 999,0 would toggle the first line of 1000 lights, turning off the ones that were on, and turning on the ones that were off.
turn off 499,499 through 500,500 would turn off (or leave off) the middle four lights.
After following the instructions, how many lights are lit?
*/

#include <iostream>
#include <fstream>

#include <string>
#include <vector>
#include <regex>
#include <functional>
#include <chrono>

inline void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

struct Light
{
    using grid_type     = std::vector<std::vector<Light>>;
    using count_type    = grid_type::size_type;
    using coord_type    = std::pair<count_type, count_type>;

    Light(void) = default;
    ~Light(void) {}

    bool toggle(void)
    {
        this->state(!_state);
        return _state;
    }

    void state(const bool state_)
    {
        _prev_state = _state;
        _state = state_;
        if (_prev_state != _state) {
            if (_state) {
                ++lit_count;
            } else {
                --lit_count;
            }
        }
    }

    bool state(void) const
    {
        return _state;
    }

    static void process_instructions(const std::string& action_, const coord_type& start_corner_, const coord_type& end_corner_, grid_type& grid_)
    {
        if (grid_.empty() || (start_corner_.first >= grid_.size()) || ((start_corner_.second >= grid_[0].size()))
            || (end_corner_.first >= grid_.size()) || ((end_corner_.second >= grid_[0].size()))) {
            throw std::invalid_argument("Wrong grid size or over-indexing detected!");
        }

        if ("turn on" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light& light_) { light_.state(true); },
                grid_);
        } else if ("turn off" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light& light_) { light_.state(false); },
                grid_);
        } else if ("toggle" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light& light_) { light_.toggle(); },
                grid_);
        } else {
            throw std::invalid_argument("The perceived action isn't supported! \'" + action_ + '\'');
        }
    }

    static count_type lit_count;

private:
    static void _execute(const coord_type& start_corner_, const coord_type& end_corner_, const std::function<void(Light&)> action_, grid_type& grid_)
    {
        for (count_type i = start_corner_.first; i <= end_corner_.first; ++i) {
            for (count_type j = start_corner_.second; j <= end_corner_.second; ++j) {
                action_(grid_[i][j]);
            }
        }
    }

private:
    bool _state{false};
    bool _prev_state{false};
};

Light::count_type Light::lit_count{0};

/**
--- Part Two ---

You just finish implementing your winning light pattern when you realize you mistranslated Santa's message from Ancient Nordic Elvish.

The light grid you bought actually has individual brightness controls; each light can have a brightness of zero or more. The lights all start at zero.

The phrase turn on actually means that you should increase the brightness of those lights by 1.

The phrase turn off actually means that you should decrease the brightness of those lights by 1, to a minimum of zero.

The phrase toggle actually means that you should increase the brightness of those lights by 2.

What is the total brightness of all lights combined after following Santa's instructions?

For example:

turn on 0,0 through 0,0 would increase the total brightness by 1.
toggle 0,0 through 999,999 would increase the total brightness by 2000000.
*/
struct Light2
{
    using grid_type = std::vector<std::vector<Light2>>;
    using count_type = grid_type::size_type;
    using coord_type = std::pair<count_type, count_type>;

    Light2(void) = default;
    ~Light2(void) {}

    count_type toggle(void)
    {
        this->bright(2);
        return _state;
    }

    void bright(const long change_)
    {
        _prev_state = _state;
        const long real_change{((change_ < 0) && (_state == 0)) ? 0 : change_};

        _state += real_change;
        if (_prev_state != _state) {
            total_bright += real_change;
        }
    }

    count_type bright(void) const
    {
        return _state;
    }

    static void process_instructions(const std::string& action_, const coord_type& start_corner_, const coord_type& end_corner_, grid_type& grid_)
    {
        if (grid_.empty() || (start_corner_.first >= grid_.size()) || ((start_corner_.second >= grid_[0].size()))
            || (end_corner_.first >= grid_.size()) || ((end_corner_.second >= grid_[0].size()))) {
            throw std::invalid_argument("Wrong grid size or over-indexing detected!");
        }

        if ("turn on" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light2& light_) { light_.bright(1); },
                grid_);
        } else if ("turn off" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light2& light_) { light_.bright(-1); },
                grid_);
        } else if ("toggle" == action_) {
            _execute(start_corner_, end_corner_,
                [](Light2& light_) { light_.toggle(); },
                grid_);
        } else {
            throw std::invalid_argument("The perceived action isn't supported! \'" + action_ + '\'');
        }
    }

    static count_type total_bright;

private:
    static void _execute(const coord_type& start_corner_, const coord_type& end_corner_, const std::function<void(Light2&)> action_, grid_type& grid_)
    {
        for (count_type i = start_corner_.first; i <= end_corner_.first; ++i) {
            for (count_type j = start_corner_.second; j <= end_corner_.second; ++j) {
                action_(grid_[i][j]);
            }
        }
    }

private:
    count_type _state{0};
    count_type _prev_state{0};
};

Light2::count_type Light2::total_bright{0};

int main(int argc_, char** argv_)
{
    if (argc_ != 4) {
        std::cerr << "Usage: " << argv_[0] << " <input_file> <width> <height>" << std::endl;
        wait();
        std::exit(1);
    }

    const Light2::count_type grid_width{std::stoul(argv_[2])};
    const Light2::count_type grid_height{std::stoul(argv_[3])};
    
    std::fstream input(argv_[1], std::ios_base::in);
    if (!input.good()) {
        std::cerr << "Cannot use input file \'" << argv_[0] << "\'!" << std::endl;
        wait();
        std::exit(2);
    }

    // grid init
    Light2::grid_type grid(grid_width);
    for (Light2::count_type k = 0; k < grid_width; ++k) {
        grid[k].resize(grid_height);
    }

    // processing instroctuins
    std::string instruction;
    std::regex inst_pattern{"(turn on|turn off|toggle)\\s+(\\d+),(\\d+)\\s+through\\s+(\\d+),(\\d+)"};
    std::smatch matching_parts;
    try {
        auto start = std::chrono::steady_clock::now();
        std::cout << "Reading & executing the instructions...";
        while (std::getline(input, instruction)) {
            if (std::regex_match(instruction, matching_parts, inst_pattern)) {
                const auto& action = matching_parts[1];
                const auto& fx = matching_parts[2];
                const auto& fy = matching_parts[3];
                const auto& tx = matching_parts[4];
                const auto& ty = matching_parts[5];
                Light2::process_instructions(action.str(),
                {std::stoul(fx.str()), std::stoul(fy.str())},
                {std::stoul(tx.str()), std::stoul(ty.str())},
                    grid);
            } else {
                throw std::invalid_argument("The following instruction cannot be interpreted! \'" + instruction + '\'');
            }
        }
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
        std::cout << " finished in " << duration.count() << "s" << std::endl;
    } catch (const std::exception& e_) {
        std::cerr << "Exception: " << e_.what() << std::endl;
        std::cerr.flush();
        throw;
    }

    /*const std::string output_name{"lights.txt"};
    std::fstream output(output_name, std::ios_base::out);
    if (!output.good()) {
        std::cerr << "Cannot use output file \'" << output_name << "\'!" << std::endl;
        wait();
        std::exit(3);
    }
    std::cout << "Writing result into " << output_name << "...";
    auto start = std::chrono::steady_clock::now();
    for (const auto& col : grid) {
        for (const auto& light : col) {
            output << static_cast<char>(light.state() ? '*' : ' ');
        }
        output << std::endl;
    }
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    std::cout << " finished in " << duration.count() << "s" << std::endl;*/

    /*std::cout << "After following the instructions, how many lights are lit? " << Light::lit_count <<
        " [" << grid_width << ',' << grid_height << ']' << std::endl;*/

    std::cout << "What is the total brightness of all lights combined after following Santa's instructions? " << Light2::total_bright <<
        " [" << grid_width << ',' << grid_height << ']' << std::endl;

    wait();
    return 0;
}
