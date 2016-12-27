#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <boost/program_options.hpp>

#include <regex>
#include <map>
#include <set>

namespace po = boost::program_options;

#define KEYPAD_SIZE 5

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

class Finder
{
    std::regex              _room_regex{"((?:(?:[a-z]+)-?)+)-(\\d+)\\[([a-z]+)\\]"};
    unsigned long           _real_room_sectorID_sum{0};
    std::regex              _searched_room_regex;

public:
    Finder(std::string&& search_regex_)
        : _searched_room_regex{search_regex_}
    {}

    void parse_room(std::string&& room_name_)
    {
        // std::cout << room_name_ << std::endl;
        std::smatch matches;
        if (std::regex_match(room_name_, matches, _room_regex)) {
            /*for (size_t i = 0; i < matches.size(); ++i) {
                auto sub_match = matches[i];
                std::cout << "  submatch " << i << ": " << sub_match.str() << '\n';
            }*/
            
            const auto& encrypted_name = matches[1];
            auto sector_id = std::stoul(matches[2].str());
            const auto checksum = matches[3].str();

            // find the five most common letter in the encrypted name in order
            // in case of tie, use alphabet
            std::string                                                 expected_checksum;
            std::map<char, size_t>                                      counts;
            std::map<size_t, std::set<char>, std::greater<size_t>>      order;
            for (auto it = encrypted_name.first; it != encrypted_name.second; ++it) {
                if ('-' == *it) {
                    continue;
                }

                auto& count = counts[*it];
                if (count) {
                    order[count].erase(*it);
                }
                ++count;
                order[count].insert(*it);
            }
            size_t n{0};
            const size_t checksum_length{5};
            for (const auto& o : order) {
                // std::cout << o.first << " ->" << std::endl;
                for (auto chIt = o.second.begin(); (chIt != o.second.end()) && (n != checksum_length); ++chIt) {
                    const auto& ch = *chIt;
                    // std::cout << o.first << "  " << ch << "\n";
                    expected_checksum += ch;
                    ++n;
                }
                if (checksum_length == n) {
                    break;
                }
            }
            //std::cout << expected_checksum << " VS " << checksum << std::endl;

            if (expected_checksum == checksum) {
                _real_room_sectorID_sum += sector_id;
            }
        } else {
            throw "Unexpected room name! " + room_name_;
        }
    }
    
    unsigned long real_room_sectorID_sum(void) const
    {
        return _real_room_sectorID_sum;
    }

    unsigned long found_room(std::string&& room_name_) const
    {
        std::smatch matches;
        if (std::regex_match(room_name_, matches, _room_regex)) {
            /*for (size_t i = 0; i < matches.size(); ++i) {
                auto sub_match = matches[i];
                std::cout << "  submatch " << i << ": " << sub_match.str() << '\n';
            }*/

            const auto& encrypted_name = matches[1];
            auto sector_id = std::stoul(matches[2].str());
            const auto checksum = matches[3].str();

            // find the five most common letter in the encrypted name in order
            // in case of tie, use alphabet
            std::string                                                 expected_checksum;
            std::map<char, size_t>                                      counts;
            std::map<size_t, std::set<char>, std::greater<size_t>>      order;
            for (auto it = encrypted_name.first; it != encrypted_name.second; ++it) {
                if ('-' == *it) {
                    continue;
                }

                auto& count = counts[*it];
                if (count) {
                    order[count].erase(*it);
                }
                ++count;
                order[count].insert(*it);
            }
            size_t n{0};
            const size_t checksum_length{5};
            for (const auto& o : order) {
                // std::cout << o.first << " ->" << std::endl;
                for (auto chIt = o.second.begin(); (chIt != o.second.end()) && (n != checksum_length); ++chIt) {
                    const auto& ch = *chIt;
                    // std::cout << o.first << "  " << ch << "\n";
                    expected_checksum += ch;
                    ++n;
                }
                if (checksum_length == n) {
                    break;
                }
            }
            //std::cout << expected_checksum << " VS " << checksum << std::endl;

            if (expected_checksum == checksum) {
                // real room, lets decrypt its name!
                std::string decrypted_name;
                for (auto it = encrypted_name.first; it != encrypted_name.second; ++it) {
                    auto ch = *it;
                    if ('-' == ch) {
                        ch = ' ';
                    } else {
                        for (size_t i = 0; i < sector_id; ++i) {
                            if ('z' == ch) {
                                ch = 'a';
                            } else {
                                ++ch;
                            }
                        }
                    }
                    decrypted_name += ch;
                }
                std::cout << encrypted_name << " --> " << decrypted_name << "\n";
                if (std::regex_search(decrypted_name, _searched_room_regex)) {
                    return sector_id;
                }
            }
        } else {
            throw "Unexpected room name! " + room_name_;
        }

        return 0;
    }
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
        Finder f{"north"};
        std::string line;
        while (std::getline(file, line)) {
            if (auto sector_id = f.found_room(std::move(line))) {
                std::cout << "What is the sector ID of the room where North Pole objects are stored? " << sector_id << std::endl;
                break;
            }
        }
        // std::cout << "What is the sum of the sector IDs of the real rooms? " << f.real_room_sectorID_sum() << std::endl;
        
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
