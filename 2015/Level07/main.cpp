/**
--- Day 7: Some Assembly Required ---

This year, Santa brought little Bobby Tables a set of wires and bitwise logic gates! Unfortunately, little Bobby is a little under the recommended age range, and he needs help assembling the circuit.

Each wire has an identifier (some lowercase letters) and can carry a 16-bit signal (a number from 0 to 65535). A signal is provided to each wire by a gate, another wire, or some specific value. Each wire can only get a signal from one source, but can provide its signal to multiple destinations. A gate provides no signal until all of its inputs have a signal.

The included instructions booklet describes how to connect the parts together: x AND y -> z means to connect wires x and y to an AND gate, and then connect its output to wire z.

For example:

123 -> x means that the signal 123 is provided to wire x.
x AND y -> z means that the bitwise AND of wire x and wire y is provided to wire z.
p LSHIFT 2 -> q means that the value from wire p is left-shifted by 2 and then provided to wire q.
NOT e -> f means that the bitwise complement of the value from wire e is provided to wire f.
Other possible gates include OR (bitwise OR) and RSHIFT (right-shift). If, for some reason, you'd like to emulate the circuit instead, almost all programming languages (for example, C, JavaScript, or Python) provide operators for these gates.

For example, here is a simple circuit:

123 -> x
456 -> y
x AND y -> d
x OR y -> e
x LSHIFT 2 -> f
y RSHIFT 2 -> g
NOT x -> h
NOT y -> i
After it is run, these are the signals on the wires:

d: 72
e: 507
f: 492
g: 114
h: 65412
i: 65079
x: 123
y: 456
In little Bobby's kit's instructions booklet (provided as your puzzle input), what signal is ultimately provided to wire a?
*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/lexical_cast.hpp>
#include <boost/optional.hpp>
#include <boost/optional/optional_io.hpp>
CLANG_DIAG_POP()

#include <chrono>
#include <fstream>
#include <functional>
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>
#include <vector>


inline void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

struct Wire;
struct Op
{
    using raw_signal_type = std::uint16_t;
    using signal_type = boost::optional<raw_signal_type>;
    using source_type = std::shared_ptr<Op>;
    using wire_type = Wire*;

    static boost::none_t sig_none;
    static wire_type wire_none;

    virtual ~Op(void) = 0;
    virtual signal_type eval(void) const = 0;
    virtual std::string print(void) const = 0;
};
inline Op::~Op(void) {}
boost::none_t Op::sig_none{boost::none};
Op::wire_type Op::wire_none{nullptr};

struct Wire
{
    Wire(void) = default;
    Wire(const std::string& target_) : _target{target_}
    {}
    Wire(const std::string& target_, Op::source_type source_)
        : _source{source_}, _target{target_}
    {}
    ~Wire(void)
    {}

    Op::signal_type eval(void) const;

    std::string print(void) const
    {
        std::ostringstream ost;
        ost << _target << ": " << ((_source) ? _source->print() : "__");
        return ost.str();
    }

    static void reset_cache(void)
    {
        eval_cache.clear();
    }

    friend std::ostream& operator<<(std::ostream& lhs_, const Wire& rhs_)
    {
        return lhs_ << rhs_.print();
    }

    Op::source_type _source{};
    std::string _target{};

private:
    using cache_type = std::unordered_map<std::string, Op::signal_type>;

private:
    static cache_type eval_cache;
};
Wire::cache_type Wire::eval_cache;

struct And : public Op
{
    using child_source_type = std::shared_ptr<And>;

    And(const wire_type lhs_, const wire_type rhs_)
        : _lhs{lhs_}, _rhs{rhs_}
    {}
    virtual ~And(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_lhs && _rhs) {
            signal_type lhv{_lhs->eval()};
            signal_type rhv{_rhs->eval()};
            if (lhv && rhv) {
                return signal_type(*lhv & *rhv);
            }
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        std::ostringstream ost;
        ost << ((_lhs) ? _lhs->print() : "._.") << " & " << ((_rhs) ? _rhs->print() : "._.");
        return  ost.str();
    }

    wire_type _lhs{wire_none};
    wire_type _rhs{wire_none};
};

struct Or : public Op
{
    using child_source_type = std::shared_ptr<Or>;

    Or(const wire_type lhs_, const wire_type rhs_)
        : _lhs{lhs_}, _rhs{rhs_}
    {}
    virtual ~Or(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_lhs && _rhs) {
            signal_type lhv{_lhs->eval()};
            signal_type rhv{_rhs->eval()};
            if (lhv && rhv) {
                return signal_type(*lhv | *rhv);
            }
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        std::ostringstream ost;
        ost << ((_lhs) ? _lhs->print() : "._.") << " | " << ((_rhs) ? _rhs->print() : "._.");
        return  ost.str();
    }

    wire_type _lhs{wire_none};
    wire_type _rhs{wire_none};
};

struct Not : public Op
{
    using child_source_type = std::shared_ptr<Not>;

    virtual ~Not(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_rhs) {
            signal_type rhv{_rhs->eval()};
            if (rhv) {
                return signal_type(~*rhv);
            }
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        std::ostringstream ost;
        ost << '~' << ((_rhs) ? _rhs->print() : "._.");
        return  ost.str();
    }

    wire_type _rhs{wire_none};
};

struct LShift : public Op
{
    using child_source_type = std::shared_ptr<LShift>;

    virtual ~LShift(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_lhs) {
            signal_type lhv{_lhs->eval()};
            if (lhv) {
                return signal_type(*lhv << _rhs);
            }
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        std::ostringstream ost;
        ost << ((_lhs) ? _lhs->print() : "._.") << " << " << std::to_string(_rhs);
        return  ost.str();
    }

    wire_type _lhs{wire_none};
    raw_signal_type _rhs{0};
};

struct RShift : public Op
{
    using child_source_type = std::shared_ptr<RShift>;

    virtual ~RShift(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_lhs) {
            signal_type lhv{_lhs->eval()};
            if (lhv) {
                return signal_type(*lhv >> _rhs);
            }
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        std::ostringstream ost;
        ost << ((_lhs) ? _lhs->print() : "._.") << " >> " << std::to_string(_rhs);
        return  ost.str();
    }

    wire_type _lhs{wire_none};
    raw_signal_type _rhs{0};
};

struct Assign : public Op
{
    using child_source_type = std::shared_ptr<Assign>;

    Assign(const wire_type lhs_)
        : _lhs{lhs_}
    {}
    virtual ~Assign(void)
    {}

    virtual signal_type eval(void) const override
    {
        if (_lhs) {
            return _lhs->eval();
        }
        return sig_none;
    }

    virtual std::string print(void) const override
    {
        return  ((_lhs) ? _lhs->print() : "._.");
    }

    wire_type _lhs{wire_none};
};

struct Signal : public Op
{
    using child_source_type = std::shared_ptr<Signal>;

    Signal(const raw_signal_type lhs_) : _lhs{lhs_}
    {}
    virtual ~Signal(void)
    {}

    virtual signal_type eval(void) const override
    {
        return signal_type{_lhs};
    }

    virtual std::string print(void) const override
    {
        return std::to_string(_lhs);
    }

    raw_signal_type _lhs{0};
};


Op::signal_type Wire::eval(void) const
{
    if (_source) {
        const auto& cit = Wire::eval_cache.find(_target);
        if (cit != Wire::eval_cache.cend()) {
            return cit->second;
        }

        Op::signal_type res{_source->eval()};
        if (res) {
            Wire::eval_cache[_target] = res;
        }
        return res;
    }
    return Op::sig_none;
}

class Circuit
{
private:
    using wire_stack_type = std::unordered_map<std::string, Wire>;

public:
    Circuit(void) = default;
    ~Circuit(void)
    {}

    Circuit(const Circuit&) = delete;
    Circuit(Circuit&&) = delete;
    Circuit& operator=(const Circuit&) = delete;
    Circuit& operator=(Circuit&&) = delete;

    void add(const std::string& instruction_)
    {
        std::smatch matching_parts;
        if (std::regex_match(instruction_, matching_parts, _and)) {
            Op::signal_type lhs, rhs;
            try {
                lhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str());
            } catch (const boost::bad_lexical_cast&) { lhs.reset(); }
            try {
                rhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[2].str());
            } catch (const boost::bad_lexical_cast&) { rhs.reset(); }
            
            Op::wire_type lhsrc, rhsrc;
            if (lhs) { // signal
                Wire lw{"SIGNAL" + std::to_string(*lhs), std::make_shared<Signal>(*lhs)};
                auto helper = _wires.emplace(lw._target, lw);
                lhsrc = &helper.first->second;
            } else { // gate
                Wire lw(matching_parts[1].str());
                auto helper = _wires.emplace(lw._target, lw);
                lhsrc = &helper.first->second;
            }
            if (rhs) { // signal
                Wire rw{"SIGNAL" + std::to_string(*rhs), std::make_shared<Signal>(*rhs)};
                auto helper = _wires.emplace(rw._target, rw);
                rhsrc = &helper.first->second;
            } else { // gate
                Wire rw(matching_parts[2].str());
                auto helper = _wires.emplace(rw._target, rw);
                rhsrc = &helper.first->second;
            }
            And::child_source_type src{std::make_shared<And>(lhsrc, rhsrc)};

            Wire w{matching_parts[3].str(), src};
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _or)) {
            Op::signal_type lhs, rhs;
            try {
                lhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str());
            } catch (const boost::bad_lexical_cast&) { lhs.reset(); }
            try {
                rhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[2].str());
            } catch (const boost::bad_lexical_cast&) { rhs.reset(); }

            Op::wire_type lhsrc, rhsrc;
            if (lhs) { // signal
                Wire lw{"SIGNAL" + std::to_string(*lhs), std::make_shared<Signal>(*lhs)};
                auto helper = _wires.emplace(lw._target, lw);
                lhsrc = &helper.first->second;
            } else { // gate
                Wire lw(matching_parts[1].str());
                auto helper = _wires.emplace(lw._target, lw);
                lhsrc = &helper.first->second;
            }
            if (rhs) { // signal
                Wire rw{"SIGNAL" + std::to_string(*rhs), std::make_shared<Signal>(*rhs)};
                auto helper = _wires.emplace(rw._target, rw);
                rhsrc = &helper.first->second;
            } else { // gate
                Wire rw(matching_parts[2].str());
                auto helper = _wires.emplace(rw._target, rw);
                rhsrc = &helper.first->second;
            }
            Or::child_source_type src{std::make_shared<Or>(lhsrc, rhsrc)};
            
            Wire w{matching_parts[3].str(), src};
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _not)) {
            Op::signal_type rhs;
            try {
                rhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str());
            } catch (const boost::bad_lexical_cast&) { rhs.reset(); }

            Not::child_source_type src{std::make_shared<Not>()};
            if (rhs) { // signal
                Wire rw{"SIGNAL" + std::to_string(*rhs), std::make_shared<Signal>(*rhs)};
                auto helper = _wires.emplace(rw._target, rw);
                src->_rhs = &helper.first->second;
            } else { // gate
                Wire rw(matching_parts[1].str());
                auto helper = _wires.emplace(rw._target, rw);
                src->_rhs = &helper.first->second;
            }

            Wire w{matching_parts[2].str(), src};
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _lshift)) {
            Op::signal_type lhs;
            try {
                lhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str());
            } catch (const boost::bad_lexical_cast&) { lhs.reset(); }
            
            LShift::child_source_type src{std::make_shared<LShift>()};
            if (lhs) { // signal
                Wire lw{"SIGNAL" + std::to_string(*lhs), std::make_shared<Signal>(*lhs)};
                auto helper = _wires.emplace(lw._target, lw);
                src->_lhs = &helper.first->second;
            } else { // gate
                Wire lw(matching_parts[1].str());
                auto helper = _wires.emplace(lw._target, lw);
                src->_lhs = &helper.first->second;
            }
            
            Op::raw_signal_type rhs{boost::lexical_cast<Op::raw_signal_type>(matching_parts[2].str())};
            src->_rhs = rhs;

            Wire w{matching_parts[3].str(), src};
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _rshift)) {
            Op::signal_type lhs;
            try {
                lhs = boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str());
            } catch (const boost::bad_lexical_cast&) { lhs.reset(); }
            
            RShift::child_source_type src{std::make_shared<RShift>()};
            if (lhs) { // signal
                Wire lw{"SIGNAL" + std::to_string(*lhs), std::make_shared<Signal>(*lhs)};
                auto helper = _wires.emplace(lw._target, lw);
                src->_lhs = &helper.first->second;
            } else { // gate
                Wire lw(matching_parts[1].str());
                auto helper = _wires.emplace(lw._target, lw);
                src->_lhs = &helper.first->second;
            }

            Op::raw_signal_type rhs{boost::lexical_cast<Op::raw_signal_type>(matching_parts[2].str())};
            src->_rhs = rhs;

            Wire w{matching_parts[3].str(), src};
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _assign)) {
            Wire lw(matching_parts[1].str());
            auto helper_src = _wires.emplace(lw._target, lw);
            Assign::child_source_type src{std::make_shared<Assign>(&helper_src.first->second)};

            Wire w{matching_parts[2].str(), src};
            auto helper_trg = _wires.emplace(w._target, w);
            if (!helper_trg.second) {
                helper_trg.first->second._source = src;
            }
        } else if (std::regex_match(instruction_, matching_parts, _signal)) {
            Signal::child_source_type src{std::make_shared<Signal>(boost::lexical_cast<Op::raw_signal_type>(matching_parts[1].str()))};
            
            Wire w(matching_parts[2].str(), src);
            auto helper = _wires.emplace(w._target, w);
            if (!helper.second) {
                helper.first->second._source = src;
            }
        } else {
            throw std::invalid_argument("Cannot interpret the following instruction: \'"+instruction_+'\'');
        }
    }

    Op::signal_type eval(const std::string& wire_) const
    {
        const auto& cit = _wires.find(wire_);
        return ((cit == _wires.cend()) ? Op::sig_none : cit->second.eval());
    }

    bool set_signal(const std::string& wire_, const Op::raw_signal_type signal_)
    {
        auto& it = _wires.find(wire_);
        if (it == _wires.end()) {
            return false;
        }
        it->second._source = std::make_shared<Signal>(signal_);
        return true;
    }

    void reset(void)
    {
        Wire::reset_cache();
        std::cout << "The circuit has been reseted." << std::endl;
    }

    friend std::ostream& operator<<(std::ostream& lhs_, const Circuit& rhs_)
    {
        for (const auto& w : rhs_._wires) {
            lhs_ << w.first << " <- " << w.second << " = " << w.second.eval() << "\n";
        }
        return lhs_;
    }

private:
    std::regex _and{"(\\S+)\\s+AND\\s+(\\S+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _or{"(\\S+)\\s+OR\\s+(\\S+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _not{"NOT\\s+(\\S+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _lshift{"(\\S+)\\s+LSHIFT\\s+(\\d+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _rshift{"(\\S+)\\s+RSHIFT\\s+(\\d+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _assign{"([[:lower:]]+)\\s+->\\s+([[:lower:]]+)"};
    std::regex _signal{"(\\d+)\\s+->\\s+([[:lower:]]+)"};

    wire_stack_type _wires;
};

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

    std::string instruction;
    Circuit circ;
    try {
        auto start = std::chrono::steady_clock::now();
        std::cout << "Building the circuit emulator...";
        std::size_t i{0};
        while (std::getline(input, instruction)) {
            circ.add(instruction);
            //std::cout << "\nDEBUG " << ++i << "\n" << circ << std::endl;
        }
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
        std::cout << " finished in " << duration.count() << "s" << std::endl;
    } catch (const std::exception& e_) {
        std::cerr << "Caught deadly exception: " << e_.what() << std::endl;
        std::cerr.flush();
        throw;
    }

    //std::cout << "\nDEBUG Result:\n" << circ << std::endl;

    std::string wire1{"a"};
    std::cout << "In little Bobby's kit's instructions booklet, what signal is ultimately provided to wire " << wire1 << '?' << std::endl;
    std::cout << "The signal on wire " << wire1 << " is ";
    auto start = std::chrono::steady_clock::now();
    auto result = circ.eval(wire1);
    std::cout << result;
    auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
    std::cout << ". Found in " << duration.count() << "s" << std::endl;

    if (result) {
        std::string wire2{"b"};
        std::cout << "Now, take the signal you got on wire " << wire1 << ", override wire " << wire2 << " to that signal"
            ", and reset the other wires (including wire " << wire1 << ")."
            " What new signal is ultimately provided to wire " << wire1 << '?' << std::endl;
        circ.set_signal(wire2, *result);
        std::cout << "The signal on wire " << wire2 << " has been changed to " << result << std::endl;
        circ.reset();

        std::cout << "Now the signal on wire " << wire1 << " changed to ";
        auto start = std::chrono::steady_clock::now();
        auto result = circ.eval(wire1);
        std::cout << result;
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
        std::cout << ". Found in " << duration.count() << "s" << std::endl;
    } else {
        std::cerr << "Cannot evaluate part 2!" << std::endl;
    }

    wait();
    return 0;
}
