/** --- Day 11: Corporate Policy ---

Santa's previous password expired, and he needs help choosing a new one.

To help him remember his new password after the old one expires, Santa has devised
a method of coming up with a password based on the previous one. Corporate policy
dictates that passwords must be exactly eight lowercase letters (for security
reasons), so he finds his new password by incrementing his old password string
repeatedly until it is valid.

Incrementing is just like counting with numbers: xx, xy, xz, ya, yb, and so on.
Increase the rightmost letter one step; if it was z, it wraps around to a, and
repeat with the next letter to the left until one doesn't wrap around.

Unfortunately for Santa, a new Security-Elf recently started, and he has imposed
some additional password requirements:

    Passwords must include one increasing straight of at least three letters,
    like abc, bcd, cde, and so on, up to xyz. They cannot skip letters; abd
    doesn't count.
    Passwords may not contain the letters i, o, or l, as these letters can be
    mistaken for other characters and are therefore confusing.
    Passwords must contain at least two different, non-overlapping pairs of
    letters, like aa, bb, or zz.

For example:

    hijklmmn meets the first requirement (because it contains the straight hij)
    but fails the second requirement requirement (because it contains i and l).
    abbceffg meets the third requirement (because it repeats bb and ff) but fails
    the first requirement.
    abbcegjk fails the third requirement, because it only has one double letter (bb).
    The next password after abcdefgh is abcdffaa.
    The next password after ghijklmn is ghjaabcc, because you eventually skip all
    the passwords that start with ghi..., since i is not allowed.

Given Santa's current password (your puzzle input), what should his next password be?

Your puzzle input is vzbxkghb.
--- Part Two ---

Santa's password expired again. What's the next one?
https://adventofcode.com/2015/day/11
*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/program_options.hpp>
CLANG_DIAG_POP()

#include <cstdint>
#include <iostream>
#include <string>
#include <vector>
#include <unordered_set>


#define PRINT_IGNORED_PASSWORDS() (false)


class IncrementablePassword
{
    using elem_type = uint8_t;

    static const size_t required_length = 8;
    static const auto element_range = static_cast<size_t>('z' - 'a' + static_cast<char>(1));
    static constexpr elem_type invalid_elements[] = {
        static_cast<elem_type>('i' - 'a'),
        static_cast<elem_type>('l' - 'a'),
        static_cast<elem_type>('o' - 'a'),
    };

    std::vector<elem_type> elements;

    bool is_invalid_elem(elem_type elem) const
    {
        for (auto i = 0ul; i < NUM_ELEMENTS_OF_ARRAY(this->invalid_elements); ++i) {
            const auto invalid_elem = this->invalid_elements[i];
            if (elem == invalid_elem) {
                return true;
            }
        }
        return false;
    }

    bool increment_to_reach_valid_letters()
    {
        auto& e = this->elements;

        bool has_any_invalid;
        auto was_incremented = false;
        do {
            has_any_invalid = false;
            for (auto i = 0ul; i < e.size(); ++i) {
                if (this->is_invalid_elem(e[i])) {
                    #if PRINT_IGNORED_PASSWORDS()
                        std::cout << "!!REACH contains invalid letter: " << *this << '\n';
                    #endif

                    const auto initial_value = e[i];

                    e[i] = (e[i] + 1) % this->element_range;
                    assert(e[i] > initial_value);

                    // e.g. abicf -> abjaa
                    for (auto j = (i + 1); j < e.size(); ++j) {
                        e[j] = 0;
                    }

                    has_any_invalid = true;
                    was_incremented = true;
                    break;
                }
            }
        } while(has_any_invalid);

        return was_incremented;
    }

    void increment()
    {
        auto& e = this->elements;

        bool has_any_invalid;
        do {
            has_any_invalid = false;
            for (auto i = e.size(); i --> 0; ) {
                const auto initial_value = e[i];

                e[i] = (e[i] + 1) % this->element_range;

                if (this->is_invalid_elem(e[i])) {
                    #if PRINT_IGNORED_PASSWORDS()
                        std::cout << "!!contains invalid letter: " << *this << '\n';
                    #endif
                    e[i] = (e[i] + 1) % this->element_range;
                    has_any_invalid = true;
                }

                if (e[i] > initial_value) {
                    break;
                }
            }
        } while(has_any_invalid);
    }

public:
    explicit IncrementablePassword(const std::string& current_password)
    {
        if (current_password.length() != this->required_length) {
            throw std::invalid_argument(current_password + " must have exactly "
                + std::to_string(this->required_length) + "characters");
        }

        this->elements.reserve(this->required_length);
        for (auto ch: current_password) {
            this->elements.push_back(static_cast<elem_type>(ch - 'a'));
        }
    }

    /** Increment the password.
     *
     *  Example:
     *  xx, xy, xz, ya, yb ...
     *
     *  @post no invalid letters. */
    IncrementablePassword& operator++()
    {
        const auto was_incremented = this->increment_to_reach_valid_letters();

        if (!was_incremented) {
            this->increment();
        }

        return *this;
    }

    IncrementablePassword operator++(int)
    {
        const auto retval = *this;
        ++(*this);
        return retval;
    }

    explicit operator bool() const
    {
        auto has_increasing_straight_3_letters = false;
        auto contains_two_different_non_overlapping_pairs_of_letters = false;

        auto num_increasing_straight_letters = 1;
        auto previous_elem = -1;
        auto previous_previous_elem = -1;

        auto previous_repeated_letter = -1;
        auto num_unique_letter_pairs = 0;
        for (auto elem: this->elements) {
            if (!has_increasing_straight_3_letters) {
                if ((previous_elem > -1) && ((previous_elem + 1) == elem)) {
                    ++num_increasing_straight_letters;
                } else {
                    num_increasing_straight_letters = 1;
                }

                has_increasing_straight_3_letters = (num_increasing_straight_letters >= 3);
            }

            if (!contains_two_different_non_overlapping_pairs_of_letters
             && (previous_previous_elem > -1) && (previous_elem > -1)) {
                const auto found_pair_except_last = (previous_previous_elem == previous_elem) && (previous_elem != elem);
                const auto found_pair_except_first = (previous_previous_elem != previous_elem) && (previous_elem == elem);
                const auto repeated_letter = (found_pair_except_last || found_pair_except_first) ? previous_elem : -1;

                if (repeated_letter > -1) {
                    if (previous_repeated_letter > -1) {
                        if (previous_repeated_letter != repeated_letter) {
                            ++num_unique_letter_pairs;
                        }
                    } else {
                        ++num_unique_letter_pairs;
                    }

                    previous_repeated_letter = repeated_letter;
                }

                contains_two_different_non_overlapping_pairs_of_letters = (num_unique_letter_pairs >= 2);
            }

            previous_previous_elem = previous_elem;
            previous_elem = elem;
        }

        #if PRINT_IGNORED_PASSWORDS()
            if (!has_increasing_straight_3_letters) {
                std::cout << "!!does not have increasing straight 3 letters: " << *this << '\n';
            }
            if (!contains_two_different_non_overlapping_pairs_of_letters) {
                std::cout << "!!does not contain 2 different, non-overlapping pairs of letters: "
                    << *this << '\n';
            }
        #endif

        return has_increasing_straight_3_letters
            && contains_two_different_non_overlapping_pairs_of_letters;
    }

    template <class Out>
    friend Out& operator<<(Out& out, const IncrementablePassword& password);

    ~IncrementablePassword() = default;
    IncrementablePassword(const IncrementablePassword&) = default;
    IncrementablePassword& operator=(const IncrementablePassword&) = default;
    IncrementablePassword(IncrementablePassword&&) = default;
    IncrementablePassword& operator=(IncrementablePassword&&) = default;
};


template <class Out>
Out& operator<<(Out& out, const IncrementablePassword& password)
{
    auto string_password = std::string();
    for (auto elem: password.elements) {
        string_password += static_cast<char>('a' + elem);
    }
    return out << string_password;
}


static auto generate_new_password(const std::string& current_password)
{
    auto new_password = IncrementablePassword(current_password);
    do {
        ++new_password;
    } while (!new_password);
    return new_password;
}


int main(int argc_, char** argv_)
{
    namespace po = boost::program_options;

    using puzzle_input_type = std::string;
    puzzle_input_type current_password;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<puzzle_input_type>(&current_password), "input number")
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
        const auto new_password = generate_new_password(current_password);
        std::cout << "Given Santa's current password (" << current_password << "), "
            "what should his next password be? " << new_password << '\n';
    }

    return exit_code;
}
