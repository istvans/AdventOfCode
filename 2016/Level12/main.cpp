#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <regex>
#include <cmath>
#include <deque>

namespace po = boost::program_options;

#define DEBUG 0
#define IS_DEBUG defined DEBUG && DEBUG == 1

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

struct Processor
{
    void operator()(std::fstream&& input_)
    {
        for (std::string cmd; std::getline(input_, cmd); ) {
            _instructions.push_back(cmd);
            
            this->process(cmd);

            while (_instc < _instructions.size()) {
                // we jumped back...
#if IS_DEBUG
                std::cout << "    executing command " << _instc << ":["<<
                    _instructions[_instc] << ']' << std::endl;
#endif
                this->process(_instructions[_instc]);
            }
        }
    }

    template <class Ostream>
    friend Ostream& operator<<(Ostream& lhs_, const Processor& rhs_)
    {
        return lhs_ << "After executing the assembunny code in your "
            "puzzle input, what value is left in register a? " << rhs_._a;
    }

private:
    using instc_t = size_t;

    inline void process(const std::string& command_)
    {
        if (_skipper && --_skipper) {
#if IS_DEBUG
            std::cout << "skipping command " << _instc << ":["<<
                command_ << ']' << std::endl;
#endif
            ++_instc;
        } else {
#if IS_DEBUG
            std::cout << "executing command " << _instc << ":["<<
                command_ << ']' << std::endl;
#endif
            std::smatch matched;
            if (std::regex_match(command_, matched, _copy)) {
                this->copy(matched[1].str(), matched[2].str());
            } else if (std::regex_match(command_, matched, _inc)) {
                this->increase(matched[1].str());
            } else if (std::regex_match(command_, matched, _dec)) {
                this->decrease(matched[1].str());
            } else if (std::regex_match(command_, matched, _jump)) {
                this->jump(matched[1].str(), matched[2].str());
            } else {
                throw "unknown command! " + command_;
            }
        }

#if IS_DEBUG
        this->print_state();
#endif
    }
    inline void copy(std::string&& src_, std::string&& trg_)
    {
        auto srcVal = std::numeric_limits<int>::quiet_NaN();
        try {
            srcVal = std::stoi(src_);
        } catch (const std::exception&) {
            switch (src_[0]) {
                case 'a': srcVal = _a; break;
                case 'b': srcVal = _b; break;
                case 'c': srcVal = _c; break;
                case 'd': srcVal = _d; break;
                default: throw "unknown register! " + src_;
            }
        }
        switch (trg_[0]) {
            case 'a': _a = srcVal; break;
            case 'b': _b = srcVal; break;
            case 'c': _c = srcVal; break;
            case 'd': _d = srcVal; break;
            default: throw "unknown register! " + trg_;
        }

        ++_instc;
    }
    inline void increase(std::string&& reg_)
    {
        switch (reg_[0]) {
            case 'a': ++_a; break;
            case 'b': ++_b; break;
            case 'c': ++_c; break;
            case 'd': ++_d; break;
            default: throw "unknown register! " + reg_;
        }

        ++_instc;
    }
    inline void decrease(std::string&& reg_)
    {
        switch (reg_[0]) {
            case 'a': --_a; break;
            case 'b': --_b; break;
            case 'c': --_c; break;
            case 'd': --_d; break;
            default: throw "unknown register! " + reg_;
        }

        ++_instc;
    }
    void jump(std::string&& condition_, std::string&& jump_)
    {
        auto jumpCondition = std::numeric_limits<int>::quiet_NaN();
        try {
            jumpCondition = std::stoi(condition_);
        } catch (const std::exception&) {
            switch (condition_[0]) {
                case 'a': jumpCondition = _a; break;
                case 'b': jumpCondition = _b; break;
                case 'c': jumpCondition = _c; break;
                case 'd': jumpCondition = _d; break;
                default: throw "unknown register! " + condition_;
            }
        }

        auto jumping = false;
        if (jumpCondition) {
            const auto theJump = std::stoi(jump_);
#if IS_DEBUG
            std::cout << "    #the jump is " << theJump << " from " << _instc << ":[" << _instructions[_instc] << ']';
#endif
            if (theJump < 0) {
                jumping = true;
                if (std::abs(theJump) > _instc) {
                    _instc = 0;
                } else {
                    _instc += theJump;
                }
#if IS_DEBUG
                std::cout << " backward to " << _instc << ":["<< _instructions[_instc] << ']' << std::endl;
#endif
            } else if (theJump > 0) {
                const auto lastAvailCmdId = _instructions.size() - 1;
                if (_instc == lastAvailCmdId) {
                    _skipper = theJump;
#if IS_DEBUG
                    std::cout << " forward with skipper " << _skipper << std::endl;
#endif
                } else {
                    const auto availCmdCount = lastAvailCmdId - _instc;
                    if (availCmdCount >= theJump) {
                        jumping = true;
                        _instc += theJump;
#if IS_DEBUG
                        std::cout << " forward to " << _instc << ":["<< _instructions[_instc] << ']' << std::endl;
#endif
                    } else {
                        _instc = lastAvailCmdId;
                        _skipper = theJump - availCmdCount;
#if IS_DEBUG
                        std::cout << " forward to " << _instc << ":["<< _instructions[_instc] <<
                            "] with skipper " << _skipper << std::endl;
#endif
                    }
                }
            } else {
                throw "zero jump!";
            }
        }

        if (!jumping) {
            ++_instc;
        }
    }
    inline void print_state(void)
    {
        std::cout << "a:" << _a << ",b:" << _b << ",c:" << _c << ",d:" << _d <<
            ",instc:" << _instc << ",skipper:" << _skipper << std::endl;
    }


    int                         _a{0}, _b{0}, _c{1}, _d{0};
    instc_t                     _instc{0};
    const std::regex            _copy{"cpy (\\S+) (\\S+)"};
    const std::regex            _inc{"inc (\\S+)"};
    const std::regex            _dec{"dec (\\S+)"};
    const std::regex            _jump{"jnz (\\S+) (\\S+)"};
    
    size_t                      _skipper{0};
    std::deque<std::string>     _instructions;
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
        Processor cpu;
        cpu(std::move(file));
        std::cout << cpu << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
