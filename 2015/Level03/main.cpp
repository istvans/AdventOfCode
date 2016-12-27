#include <iostream>
#include <fstream>

#include <set>
#include <tuple>

void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

struct Position
{
    using coordinate_type       = std::size_t;
    using position_set_type     = std::set<Position>;

    Position(const coordinate_type x_, const coordinate_type y_)
        : _x{x_}, _y{y_}
    {
        _positions_received_present.insert(*this);
    }
    Position(const Position&)               = default;
    Position(Position&&)                    = default;
    Position& operator=(const Position&)    = default;
    Position& operator=(Position&&)         = default;

    friend bool operator<(const Position& lhs_, const Position& rhs_)
    {
        return std::tie(lhs_._x, lhs_._y)
            < std::tie(rhs_._x, rhs_._y); // keep the same order
    }

    friend bool operator==(const Position& lhs_, const Position& rhs_)
    {
        return !(lhs_ < rhs_) && !(rhs_ < lhs_);
    }
    
    friend bool operator!=(const Position& lhs_, const Position& rhs_)
    {
        return !(lhs_ == rhs_);
    }

    bool move(const char dir_)
    {
        Position orig(*this);
        switch (dir_) {
            case '^': _y += 1; break;
            case '<': _x -= 1; break;
            case '>': _x += 1; break;
            case 'v': _y -= 1; break;
            default: throw std::invalid_argument("Cannot interpret direction: \'" + dir_ + '\'');
        }
        if (orig != *this) {
            _positions_received_present.insert(*this);
            return true;
        }
        return false;
    }

    coordinate_type _x;
    coordinate_type _y;

    static position_set_type _positions_received_present;
};

Position::position_set_type Position::_positions_received_present;

int main(int argc_, char** argv_)
{
    if (argc_ != 2) {
        std::cerr << "Usage: " << argv_[0] << " <input_file>" << std::endl;
        wait();
        std::exit(1);
    }

    std::fstream input(argv_[1], std::ios_base::in);
    if (!input.good()) {
        std::cerr << "Cannot use input file \'" << argv_[0] << "\'!" << std::endl;
        wait();
        std::exit(2);
    }

    Position santa(0, 0);
    Position robo_santa(0, 0);
    char direction{'_'};
    unsigned long long count{0};
    while (input >> std::noskipws >> direction) {
        if (++count % 2 == 0) {
            santa.move(direction);
        } else {
            robo_santa.move(direction);
        }
    }

    std::cout << "How many houses receive at least one present? " << Position::_positions_received_present.size() << std::endl;

    wait();
    return 0;
}
