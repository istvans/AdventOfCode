#include <iostream>
#include <fstream>
#include <string>
#include <conio.h>
#include <chrono>
#include <boost/program_options.hpp>

#include <boost/optional.hpp>

#include <unordered_map>
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

struct Bot
{
    using num_t             = unsigned long long;
    using optional_num_t    = boost::optional<num_t>;
    using bots_t            = std::unordered_map<num_t, Bot>;
    using outputs_t         = std::unordered_map<num_t, num_t>;

    Bot(const num_t id_, bots_t& bots_, outputs_t& outputs_)
        : _id{id_}
        , _low_target_type{'x'}
        , _high_target_type{'x'}
        , _bots{bots_}
        , _outputs{outputs_}
    {}

    void add(const num_t val_)
    {
        if (!_value1) {
            _value1 = val_;
        } else if (!_value2) {
            _value2 = val_;
            this->execute();
        } else {
            throw "this is not expected!";
        }
    }

    void targets(std::string&& low_target_type_, const num_t low_target_id_,
        std::string&& high_target_type_, const num_t high_target_id_)
    {
        _low_target_type = low_target_type_[0];
        _low_target = low_target_id_;
        _high_target_type = high_target_type_[0];
        _high_target = high_target_id_;
        this->execute();
    }

    void execute(void)
    {
        if (_value1 && _value2 && _low_target) {
            if (*_value2 < *_value1) {
                _value1.swap(_value2);
            }
            // value1 is the lower
            
            // solution #1
            if ((17 == *_value1) && (61 == *_value2)) {
                std::cout << "The id is " << _id << std::endl;
            }

            if ('b' == _low_target_type) {
                _bots.emplace(*_low_target, Bot{*_low_target, _bots, _outputs}).first->second.add(*_value1);
                _value1 = boost::none;
            } else if ('o' == _low_target_type) {
                _outputs[*_low_target] = *_value1;
                _value1 = boost::none;
            } else {
                throw "WTF?";
            }

            if ('b' == _high_target_type) {
                _bots.emplace(*_high_target, Bot{*_high_target, _bots, _outputs}).first->second.add(*_value2);
                _value2 = boost::none;
            } else if ('o' == _high_target_type) {
                _outputs[*_high_target] = *_value2;
                _value2 = boost::none;
            } else {
                throw "WTF?";
            }
        }
    }

    num_t               _id;

    optional_num_t      _value1;
    optional_num_t      _value2;
    char                _low_target_type;
    optional_num_t      _low_target;
    char                _high_target_type;
    optional_num_t      _high_target;

    bots_t&             _bots;
    outputs_t&          _outputs;
};

struct BotFactory
{
    void operator()(std::string&& instruction_)
    {
        std::smatch match;
        if (std::regex_match(instruction_, match, _input)) {
            const auto value = std::stoull(match[1].str());
            const auto botid = std::stoull(match[2].str());

            _bots.emplace(botid, Bot{botid, _bots, _outputs}).first->second.add(value);
        } else if (std::regex_match(instruction_, match, _pass)) {
            const auto botid = std::stoull(match[1].str());
            auto low_target_type = match[2].str();
            const auto low_target_id = std::stoull(match[3].str());
            auto high_target_type = match[4].str();
            const auto high_target_id = std::stoull(match[5].str());

            _bots.emplace(botid, Bot{botid, _bots, _outputs}).first->second.targets(
                std::move(low_target_type), low_target_id,
                std::move(high_target_type), high_target_id);
        } else {
            throw "this is not expected either!";
        }
    }

    template <class OStream>
    friend OStream& operator<< (OStream& lhs_, const BotFactory& rhs_)
    {
        // solution #2
        return lhs_ << (rhs_._outputs.at(0) * rhs_._outputs.at(1) * rhs_._outputs.at(2));
    }

private:
    const std::regex                    _input{"value (\\d+) goes to bot (\\d+)"};
    const std::regex                    _pass{"bot (\\d+) gives low to"
        " (bot|output) (\\d+) and high to"
        " (bot|output) (\\d+)"};

    Bot::bots_t                         _bots;
    Bot::outputs_t                      _outputs;
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
        BotFactory factory;
        std::cout << "Based on your instructions,"
            " what is the number of the bot that is responsible for "
            "comparing value-61 microchips with value-17 microchips? "
            << std::endl;
        for (std::string line; std::getline(file, line); ) {
            factory(std::move(line));
        }
        std::cout << "What do you get if you multiply together"
            " the values of one chip in"
            " each of outputs 0, 1, and 2? " << factory << std::endl;
    } else {
        std::cerr << "cannot open input file \'" << input_file << '\'' << std::endl;
        return generic_exit(3);
    }

    return generic_exit(0);
}
