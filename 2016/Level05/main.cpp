#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <boost/program_options.hpp>
#include <chrono>

#include "md5.h"

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

}

int main(int argc_, char** argv_)
{
    const std::string input{"ffykfhsq"};
    const std::size_t init_zeros{5};
    
    using num_type = unsigned long long;

    num_type n = 0;

    size_t i{0};
    std::string password{"        "};

    std::cout << "decrypting..." << std::endl;
    std::cout << '\'' << password << '\'' << std::endl;

    auto start = std::chrono::steady_clock::now();
    
    for (size_t n{0}; (n <= std::numeric_limits<num_type>::max()) && (i < password.size()); ++n) {
        auto md5 = MD5{input + std::to_string(n)};
        const auto result = md5.hexdigest();
        
        size_t zero_index{0};
        for (;(zero_index < init_zeros) && ('0' == result[zero_index]); ++zero_index) {}

        // std::cout << "DEBUG " << result << std::endl;
        if (zero_index == init_zeros) {
            size_t position;
            switch (result[5]) {
                case '0':
                    position = 0;
                    break;
                case '1':
                    position = 1;
                    break;
                case '2':
                    position = 2;
                    break;
                case '3':
                    position = 3;
                    break;
                case '4':
                    position = 4;
                    break;
                case '5':
                    position = 5;
                    break;
                case '6':
                    position = 6;
                    break;
                case '7':
                    position = 7;
                    break;
                default:
                    continue;
            }

            if (' ' == password[position]) {
                password[position] = result[6];
                ++i;
                std::cout << '\'' << password << '\'' << std::endl;
            }
        }
    }
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    std::cout << "Given the actual Door ID and this new method, what is the password? " << password << " (in " << duration.count() << " s)" << std::endl;
    
    return generic_exit(0);
}
