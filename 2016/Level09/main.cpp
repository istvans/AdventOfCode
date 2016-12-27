#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <regex>

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

struct Decompressor
{
    using symu_len_type = unsigned long long;

    void operator()(std::string&& line_)
    {
        for (auto cit = line_.cbegin(); cit != line_.cend(); ) {
            std::smatch matched;
            if (std::regex_search(cit, line_.cend(), matched, _marker)) {
                const auto chars = std::stoul(matched[1].str());
                const auto repeat = std::stoul(matched[2].str());
                const auto matched_size = matched[0].str().size();
                const auto from = std::distance(line_.cbegin(), cit) + matched_size;
                const auto part = line_.substr(from, chars);
                for (size_t k = 0; k < repeat; ++k) {
                    _data += part;
                }
                cit += matched_size + chars;
            } else {
                _data += *cit;
                ++cit;
            }
        }
    }
    template <class OStream>
    friend OStream& operator<<(OStream& lhs_, const Decompressor& rhs_)
    {
        return lhs_ << rhs_._data;
    }
    size_t length(void) const
    {
        return _data.length();
    }
    void calc_version2(std::string&& line_)
    {
        _symlen = this->recursive_calculator(std::move(line_));
    }
    symu_len_type symulated_length(void) const
    {
        return _symlen;
    }

private:
    symu_len_type recursive_calculator(const std::string& line_)
    {
        symu_len_type len{0};
        for (auto cit = line_.cbegin(); cit != line_.cend(); ) {
            std::smatch matched;
            if (std::regex_search(cit, line_.cend(), matched, _marker)) {
                const auto chars = std::stoul(matched[1].str());
                const auto repeat = std::stoul(matched[2].str());
                const auto matched_size = matched[0].str().size();
                const auto from = std::distance(line_.cbegin(), cit) + matched_size;
                const auto part = line_.substr(from, chars);

                auto part_len = this->recursive_calculator(part);

                for (symu_len_type k = 0; k < repeat; ++k) {
                    len += part_len;
                }
                cit += matched_size + part.size();
            } else {
                ++len;
                ++cit;
            }
        }

        return len;
    }


    symu_len_type               _symlen{0};

    std::string                 _data;
    const std::regex            _marker{"^\\((\\d+)x(\\d+)\\)"};
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
        Decompressor d;
        for (std::string line; std::getline(file, line); ) {
            d.calc_version2(std::move(line));
        }
        // std::cout << "What is the decompressed length of the file (your puzzle input)?\n"
        //     "Don't count whitespace. " << d.length() << std::endl;
        std::cout << "What is the decompressed length of the file using"
            " this improved format? " << d.symulated_length() << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
