#include <iostream>
#include <fstream>

void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

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

    char direction{'G'};
    long long floor{0};
    bool basement_entered{false};
    long long steps{0};
    while (input >> std::noskipws >> direction) {
        if (!basement_entered) ++steps;
        
        switch (direction) {
            case '(': ++floor; break;
            case ')':
            {
                --floor;
                if ((-1 == floor) && (!basement_entered)) {
                    basement_entered = true;
                }
                break;
            }
            default: throw std::invalid_argument("Cannot interpret direction: \'"+direction+'\'');
        }
    }
    std::cout << "Final floor: " << floor << "; Entered the basement at character position: " << steps << std::endl;
    
    wait();
    return 0;
}
