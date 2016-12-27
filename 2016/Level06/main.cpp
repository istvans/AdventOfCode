#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <vector>
#include <unordered_map>
#include <map>
#include <limits>

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

    std::vector<std::unordered_map<char, size_t>>   freqs;
    std::vector<std::map<size_t, char>>   most_freq;

    if (auto file = std::fstream{input_file, std::ios_base::in}) {
        auto begin = std::chrono::steady_clock::now();
        std::string line;
        while (std::getline(file, line)) {
            if (freqs.size() < line.length()) {
                freqs.resize(line.length());
                most_freq.resize(line.length());
            }
            for (size_t i{0}; i < line.length(); ++i) {
                most_freq[i][++freqs[i][line[i]]] = line[i];
            }
        }

        std::cout << "Given the recording in your puzzle input, "
            "what is the error-corrected version of the message being sent? ";
        for (const auto& mf : most_freq) {
            if (mf.begin() == mf.end()) {
                throw "cannot find most frequent character!";
            }
            std::cout << (--mf.end())->second;
        }
        std::cout << " (in " <<
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - begin).count() << " ns)" << std::endl;

        std::cout << "\n\nGiven the recording in your puzzle input and this new decoding"
            " methodology, what is the original message that Santa is trying to send? ";
        begin = std::chrono::steady_clock::now();
        for (const auto& freq : freqs) {
            size_t min{std::numeric_limits<size_t>::max()};
            char minCH{'_'};
            for (const auto& ch : freq) {
                if (ch.second < min) {
                    minCH = ch.first;
                    min = ch.second;
                    if (1 == min) {
                        break;
                    }
                }
            }
            std::cout << minCH;
        }
        std::cout << " (in " <<
            std::chrono::duration_cast<std::chrono::nanoseconds>(
                std::chrono::steady_clock::now() - begin).count() << " ns)" << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
