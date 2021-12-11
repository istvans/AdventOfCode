/** --- Day 10: Elves Look, Elves Say ---

Today, the Elves are playing a game called look-and-say(1). They take turns making
sequences by reading aloud the previous sequence and using that reading as the
next sequence. For example, 211 is read as "one two, two ones", which becomes
1221 (1 2, 2 1s).

Look-and-say sequences are generated iteratively, using the previous value as
input for the next step. For each step, take the previous value, and replace each
 run of digits (like 111) with the number of digits (3) followed by the digit
 itself (1).

For example:

    1 becomes 11 (1 copy of digit 1).
    11 becomes 21 (2 copies of digit 1).
    21 becomes 1211 (one 2 followed by one 1).
    1211 becomes 111221 (one 1, one 2, and two 1s).
    111221 becomes 312211 (three 1s, two 2s, and one 1).

Starting with the digits in your puzzle input, apply this process 40 times.
What is the length of the result?

Your puzzle input is 1113122113.
(1) https://en.wikipedia.org/wiki/Look-and-say_sequence
https://adventofcode.com/2015/day/10
*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/program_options.hpp>
CLANG_DIAG_POP()

#include <iostream>
#include <iterator>
#include <string>


class IterableBigNumber
{
    using digits_type = std::string;
    digits_type digits;

public:
    using digit_type = unsigned short;
    using optional_digit_type = std::optional<digit_type>;
    using size_type = digits_type::size_type;

    class iterator
    {
        using original_digits_iterator = digits_type::iterator;
        original_digits_iterator digits_iterator;
    public:
        using difference_type = long;
        using value_type = digit_type;
        using pointer = const digit_type*;
        using reference = const digit_type&;
        using iterator_category = std::random_access_iterator_tag;

        iterator(original_digits_iterator it) : digits_iterator(it) {}
        iterator& operator++()
        {
            std::advance(this->digits_iterator, 1);
            return *this;
        }

        iterator operator++(int)
        {
            iterator retval = *this;
            std::advance(*this, 1);
            return retval;
        }

        iterator& operator--()
        {
            std::advance(this->digits_iterator, -1);
            return *this;
        }

        iterator operator--(int)
        {
            iterator retval = *this;
            std::advance(*this, -1);
            return retval;
        }

        iterator& operator+=(difference_type n)
        {
            std::advance(this->digits_iterator, n);
            return *this;
        }

        bool operator==(iterator other) const
        {
            return this->digits_iterator == other.digits_iterator;
        }

        bool operator!=(iterator other) const
        {
            return !(*this == other);
        }

        digit_type operator*()
        {
            const char char_digit = *(this->digits_iterator);
            return static_cast<digit_type>(char_digit - '0');
        }
    };

    template <class N>
    IterableBigNumber(N number): digits(std::to_string(number))
    {}
    IterableBigNumber(): digits()
    {}

    IterableBigNumber(const IterableBigNumber&) = default;
    IterableBigNumber& operator=(const IterableBigNumber&) = default;
    IterableBigNumber(IterableBigNumber&&) = default;
    IterableBigNumber& operator=(IterableBigNumber&&) = default;

    iterator begin() noexcept
    {
        return iterator(this->digits.begin());
    }

    iterator end() noexcept
    {
        return iterator(this->digits.end());
    }

    template <class N>
    void append(N digit) noexcept
    {
        this->digits += std::to_string(digit);
    }

    size_type length() const noexcept
    {
        return this->digits.length();
    }

    template <class Out>
    friend Out& operator<<(Out&, const IterableBigNumber&);
};

template <class Out>
Out& operator<<(Out& out, const IterableBigNumber& num)
{
    return out << num.digits;
}


int main(int argc_, char** argv_)
{
    namespace po = boost::program_options;
    using puzzle_input_t = unsigned long long;
    puzzle_input_t input_number;
    puzzle_input_t number_of_iterations;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<puzzle_input_t>(&input_number), "input number")
        ("number-of-iterations,n", po::value<puzzle_input_t>(&number_of_iterations), "number of iterations")
        ("help,h", "show this help message")
        ;

    po::variables_map vm;
    int exit_code = 0;
    try {
        po::store(po::parse_command_line(argc_, argv_, description), vm);
    } catch (const std::exception& ex) {
        std::cout << description << " - " << ex.what() << std::endl;
        exit_code = 1;
    }

    if (exit_code == 0) {
        po::notify(vm);

        if (vm.count("help")) {
            std::cout << description << std::endl;
        } else if (!vm.count("input")) {
            std::cout << description << std::endl;
            exit_code = 2;
        }
    }

    if (exit_code == 0) {
        auto number = IterableBigNumber(input_number);
        std::cout << "input: " << number << '\n';
        for (puzzle_input_t i = 0; i < number_of_iterations; ++i) {
            std::cout << "iteration " << (i + 1) << "... ";

            auto new_number = IterableBigNumber();
            auto maybe_previous_digit = IterableBigNumber::optional_digit_type();
            size_t num_last_digit_seen = 0;
            for (auto digit: number) {
                if (maybe_previous_digit.has_value()) {
                    auto previous_digit = maybe_previous_digit.value();
                    if (digit != previous_digit) {
                        new_number.append(num_last_digit_seen);
                        new_number.append(previous_digit);

                        num_last_digit_seen = 0;
                    }
                }

                ++num_last_digit_seen;
                maybe_previous_digit = digit;
            }
            new_number.append(num_last_digit_seen);
            new_number.append(*(std::prev(number.end())));

            number = new_number;
            std::cout << number << '\n';
        }

        std::cout << "What is the length of the result? " << number.length() << "\n";
    }

    return exit_code;
}
