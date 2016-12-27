#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <regex>
#include <iterator>
#include <unordered_set>

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

// transport-layer snooping
struct TLS
{
    void process(const std::string& ip_)
    {
        bool hypernet_sequence{false};
        bool supportTLS{false};

        //std::cout << ip_ << std::endl;
        for (size_t i{0}; i < ip_.length(); ++i) {
            const auto& ch = ip_[i];

            //std::cout << ch;

            if ((ip_.length() - i) < 4) {
                //std::cout << ":too late";
                break;
            } else if ('[' == ch) {
                hypernet_sequence = true;
            } else if (']' == ch) {
                hypernet_sequence = false;
            } else {
                const auto part = ip_.substr(i, 4);

                if (std::regex_match(part, _abba) && !std::regex_match(part, _unabba)) {
                    if (hypernet_sequence) {
                        supportTLS = false;
                        break;
                    } else {
                        supportTLS = true;
                    }
                }
            }
            //std::cout << ((hypernet_sequence) ? 'H' : '*') << std::endl;
        }

        if (supportTLS) {
            ++_count;
        }
        //std::cout << ((supportTLS) ? 'T' : '!') << std::endl;
    }

    const std::regex _abba{"(.)(.)\\2\\1"};
    const std::regex _unabba{"(.)\\1\\1\\1"};
    size_t _count{0};

};

// super-secret listening
struct SSL
{
    void process(const std::string& ip_)
    {
        bool hypernet_sequence{false};
        bool supportSSL{false};
        std::unordered_set<std::string> abaSet;
        std::unordered_set<std::string> babSet;

        //std::cout << ip_ << std::endl;
        for (size_t i{0}; i < ip_.length(); ++i) {
            const auto& ch = ip_[i];
            
            //std::cout << ch;

            if ((ip_.length() - i) < _lookAheadSize) {
                //std::cout << ":too late";
                break;
            } else if ('[' == ch) {
                hypernet_sequence = true;
            } else if (']' == ch) {
                hypernet_sequence = false;
            } else {
                const auto part = ip_.substr(i, _lookAheadSize);

                std::smatch match;
                if (!std::regex_match(part, _unababab)
                 && std::regex_match(part, match, _ababab)) {
                    const auto a = *match[1].first;
                    const auto b = *match[2].first;
                    std::ostringstream bab;
                    bab << b << a << b;
                    if (hypernet_sequence) {
                        // we've just found a BAB (Byte Allocation Block)
                        if (abaSet.find(bab.str()) != abaSet.end()) {
                            supportSSL = true;
                            break;
                        }
                        babSet.insert(part);
                    } else {
                        // we've just found an ABA (Area-Broadcast Accessor)
                        if (babSet.find(bab.str()) != babSet.end()) {
                            supportSSL = true;
                            break;
                        }
                        abaSet.insert(part);
                    }
                }
            }
            //std::cout << ((hypernet_sequence) ? 'H' : '*') << std::endl;
        }

        if (supportSSL) {
            ++_count;
        }
        //std::cout << ((supportTLS) ? 'T' : '!') << std::endl;
    }

    const std::regex _ababab{"(.)(.)\\1"};
    const std::regex _unababab{"(.)\\1\\1"};
    size_t _count{0};
    const size_t _lookAheadSize{3};
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
        const auto begin = std::chrono::steady_clock::now();
        
        TLS tls;
        SSL ssl;

        std::string ip;
        while (std::getline(file, ip)) {
            tls.process(ip);
            ssl.process(ip);
        }

        const auto end = std::chrono::steady_clock::now();

        std::cout << "How many IPs in your puzzle input support TLS "
            "(Transport-Layer Snooping)? " << tls._count <<
            "\nHow many IPs in your puzzle input support SSL "
            "(Super-Secret Listening)? " << ssl._count <<
            "\n[in " << std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count()
            << " us]"<< std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
