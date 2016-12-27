#include <iostream>
#include <fstream>

#include <string>
#include <set>
#include <tuple>
#include <utility>

#include <boost/optional.hpp>

using opt_char_type = boost::optional<std::string::value_type>;

inline void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
}

// first puzzle

inline bool inappropriate(const std::string& word_)
{
    return (word_.find("ab") != std::string::npos) ||
        (word_.find("cd") != std::string::npos) ||
        (word_.find("pq") != std::string::npos) ||
        (word_.find("xy") != std::string::npos);
}

inline bool vowel(const char ch_)
{
    switch (ch_) {
        case 'a':
        case 'e':
        case 'i':
        case 'o':
        case 'u': return true;
        default: return false;
    }
}

inline bool has_three_vowels(const std::string& word_)
{
    std::size_t vowels{0};
    for (const auto& ch: word_) {
        if (vowel(ch)) {
            if (++vowels >= 3) {
                return true;
            }
        }
    }
    return false;
}

inline bool has_double_letter(const std::string& word_)
{
    opt_char_type prev;
    for (const auto& ch : word_) {
        if (prev && *prev == ch) {
            return true;
        }
        prev = ch;
    }
    return false;
}

inline bool nice(const std::string& word_)
{
    //std::cout << "nice: DEBUG " << word_ << " " << !inappropriate(word_) << ' ' << has_three_vowels(word_) << ' ' << has_double_letter(word_) << std::endl;
    return !inappropriate(word_) && has_three_vowels(word_) && has_double_letter(word_);
}

// second puzzle

struct Pair
{
    Pair(const std::size_t fid_, const char first_, const std::size_t sid_, const char second_)
        : _letters{first_, second_}
        , _indices{fid_, sid_}
    {}
    ~Pair(void)
    {}

    Pair(const Pair&) = default;
    Pair(Pair&&) = default;
    Pair& operator=(const Pair&) = default;
    Pair& operator=(Pair&&) = default;

    friend bool operator<(const Pair& lhs_, const Pair& rhs_)
    {
        return lhs_._letters < rhs_._letters;
    }

    std::pair<char, char> _letters;
    std::pair<std::size_t, std::size_t> _indices;
};

// Nice conditions
// ===============
// 1) It contains a pair of any two letters that appears at least twice in the string without overlapping,
// like xyxy (xy) or aabcdefgaa (aa), but not like aaa (aa, but it overlaps).
//      0123         0123456789                    012
//       ^ ^          ^ ^ ^ ^ ^                     ^ 
// good: xxyxx
//       01234
//        ^ ^
// 
// 2) It contains at least one letter which repeats with exactly one letter between them,
// like xyx, abcdefeghi (efe), or even aaa.
inline bool upgraded_nice(const std::string& word_)
{
    using letter_set_type = std::set<Pair>;
    letter_set_type letter_set;
    opt_char_type prev;
    opt_char_type prev_prev;

    std::size_t i{0};
    letter_set_type::iterator it;
    bool inserted{false};
    bool twice_pair{false};
    bool among_twins{false};
    for (const auto& ch : word_) {
        if (prev) {
            std::tie(it, inserted) = letter_set.emplace((i - 1), *prev, i, ch);
            if (!inserted) {
                //std::cout << "DEBUG " << it->_indices.second << ' ' << (i - 1) << std::endl;
                if (it->_indices.second != (i - 1)) { // eliminating the 'aaa' overlapping case
                    twice_pair = true;
                }
            }

            if (prev_prev && *prev_prev == ch) {
                among_twins = true;
            }

            prev_prev = prev;
        }

        //std::cout << "upgraded_nice: DEBUG " << word_ << ':' << ((prev)?*prev:'_') << '-' << ch << ' ' << twice_pair << ' ' << among_twins << std::endl;
        if (twice_pair && among_twins) {
            return true;
        }

        prev = ch;
        ++i;
    }
    return false;
}

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

    std::string word;
    unsigned long long num_of_nice_words1{0};
    unsigned long long num_of_nice_words2{0};
    while (input >> word) {
        if (nice(word)) {
            ++num_of_nice_words1;
        }
        if (upgraded_nice(word)) {
            ++num_of_nice_words2;
        }
    }

    std::cout << "How many strings are nice? " << num_of_nice_words1 <<
        "\nHow many strings are nice under the new rules? " << num_of_nice_words2 << std::endl;

    wait();
    return 0;
}
