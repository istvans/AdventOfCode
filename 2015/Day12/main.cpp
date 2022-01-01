/** --- Day 12: JSAbacusFramework.io ---

Santa's Accounting-Elves need help balancing the books after a recent order.
Unfortunately, their accounting software uses a peculiar storage format.
That's where you come in.

They have a JSON(1) document which contains a variety of things:
arrays ([1,2,3]),
objects ({"a":1, "b":2}),
numbers, and
strings.
Your first job is to simply find all of the numbers throughout the document and
add them together.

For example:

    [1,2,3] and {"a":2,"b":4} both have a sum of 6.
    [[[3]]] and {"a":{"b":4},"c":-1} both have a sum of 3.
    {"a":[-1,1]} and [-1,{"a":1}] both have a sum of 0.
    [] and {} both have a sum of 0.

You will not encounter any strings containing numbers.

What is the sum of all numbers in the document?
--- Part Two ---

Uh oh - the Accounting-Elves have realized that they double-counted everything red.

Ignore any object (and all of its children) which has any property with the value "red".
Do this only for objects ({...}), not arrays ([...]).

    [1,2,3] still has a sum of 6.
    [1,{"c":"red","b":2},3] now has a sum of 4, because the middle object is ignored.
    {"d":"red","e":[1,2,3,4],"f":5} now has a sum of 0, because the entire structure is ignored.
    [1,"red",5] has a sum of 6, because "red" in an array has no effect.

(1) http://json.org/
https://adventofcode.com/2015/day/11
*/
#include "macros.h"

CLANG_DIAG_IGNORE_BOOST_WARNINGS()
#include <boost/json/src.hpp>
#include <boost/program_options.hpp>
CLANG_DIAG_POP()

#include <fstream>
#include <iostream>


#define IGNORE_RED() (true)

static auto read_file(std::string_view path) -> std::string {
    constexpr auto read_size = std::size_t{4096};
    auto stream = std::ifstream{path.data()};
    stream.exceptions(std::ios_base::badbit);

    auto out = std::string{};
    auto buf = std::string(read_size, '\0');
    while (stream.read(& buf[0], read_size)) {
        const auto num_read_chars = stream.gcount();
        assert(num_read_chars >= 0);
        out.append(buf, 0, static_cast<std::string::size_type>(num_read_chars));
    }
    const auto num_read_chars = stream.gcount();
    assert(num_read_chars >= 0);
    out.append(buf, 0, static_cast<std::string::size_type>(num_read_chars));
    return out;
}

template <class T>
static void process(T& sum, const boost::json::value& data, bool ignore_red)
{
    if (data.is_number()) {
        if (const auto* int_val = data.if_int64()) {
            sum += *int_val;
        } else if (const auto* uint_val = data.if_uint64()) {
            sum += *uint_val;
        } else {
            sum += static_cast<T>(data.as_double());
        }
    } else if (const auto* obj = data.if_object()) {
        if (ignore_red) {
            auto has_any_red = false;
            for (const auto& v: *obj) {
                const auto& val = v.value();
                if (const auto* str = val.if_string()) {
                    has_any_red = *str == "red";
                }
                if (has_any_red) {
                    break;
                }
            }
            if (has_any_red) {
                return;
            }
        }

        for (const auto& v: *obj) {
            process(sum, v.value(), ignore_red);
        }
    } else if (const auto* arr = data.if_array()) {
        for (const auto& elem: *arr) {
            process(sum, elem, ignore_red);
        }
    } else {
        // we do not care about the rest of possible kinds
    }
}


int main(int argc_, char** argv_)
{
    namespace po = boost::program_options;

    using puzzle_input_type = std::string;
    puzzle_input_type input_filename;

    po::options_description description("Options");
    description.add_options()
        ("input,i", po::value<puzzle_input_type>(&input_filename), "input file name")
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
        const auto json_string = read_file(input_filename);
        const auto data = boost::json::parse(json_string);

        long double part_1_sum = 0.0l;
        process(part_1_sum, data, !IGNORE_RED());
        std::cout << "What is the sum of all numbers in the document? " << part_1_sum << '\n';

        long double part_2_sum = 0.0l;
        process(part_2_sum, data, IGNORE_RED());
        std::cout << "What is the sum of all numbers in the document ignoring any "
            "object (and all of its children) which has any property with the value \"red\"? "
            << part_2_sum << '\n';
    }

    return exit_code;
}
