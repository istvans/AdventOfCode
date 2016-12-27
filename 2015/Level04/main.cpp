#include "md5/md5.h"

#include <iostream>

#include <string>
#include <limits>
#include <chrono>

void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

int main(int argc_, char** argv_)
{
    //const std::string input{"yzbqklnj"};
    //const std::string input{"abcdef"};
    
    if (argc_ != 3) {
        std::cerr << "Usage: " << argv_[0] << " <input text for md5 hash> <# of init zeros>" << std::endl;
        wait();
        std::exit(1);
    }

    const std::string input{argv_[1]};
    const std::size_t init_zeros{std::stoul(argv_[2])};
    
    std::cout << "Text:\'" << input << "\'\nInitial zeros:" << init_zeros << std::endl;

    using num_type = unsigned long long;
    
    num_type n = 1;
    std::string test = input + std::to_string(n);
    bool found{false};

    std::cout << "Calculating..." << std::endl;
    auto start = std::chrono::steady_clock::now();
    for (; n <= std::numeric_limits<num_type>::max(); ++n) {
        test = input + std::to_string(n);
        MD5 md5(test);
        const std::string result{md5.hexdigest()};
        //std::cout << "DEBUG " << test << " -> " << result << std::endl;
        if (result.find_first_not_of('0') == init_zeros) {
            auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
            std::cout << "The first MD5 hash, in hexadecimal, which starts with 5 zeros for input \'" << test << "\' is " << result <<
                ". [time: " << duration.count() << "s]" << std::endl;
            found = true;
            break;
        }
    }

    if (!found) std::cerr << "ERROR: CANNOT FIND SUCH MD5!!!" << std::endl;

    wait();
    return 0;
}
