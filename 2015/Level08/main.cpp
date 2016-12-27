/**
--- Day 8: Matchsticks ---

Space on the sleigh is limited this year, and so Santa will be bringing his list as a digital copy. He needs to know how much space it will take up when stored.

It is common in many programming languages to provide a way to escape special characters in strings. For example, C, JavaScript, Perl, Python, and even PHP handle special characters in very similar ways.

However, it is important to realize the difference between the number of characters in the code representation of the string literal and the number of characters in the in-memory string itself.

For example:

"" is 2 characters of code (the two double quotes), but the string contains zero characters.
"abc" is 5 characters of code, but 3 characters in the string data.
"aaa\"aaa" is 10 characters of code, but the string itself contains six "a" characters and a single, escaped quote character, for a total of 7 characters in the string data.
"\x27" is 6 characters of code, but the string itself contains just one - an apostrophe ('), escaped using hexadecimal notation.
Santa's list is a file that contains many double-quoted string literals, one on each line. The only escape sequences used are \\ (which represents a single backslash), \" (which represents a lone double-quote character), and \x plus two hexadecimal characters (which represents a single character with that ASCII code).

Disregarding the whitespace in the file, what is the number of characters of code for string literals minus the number of characters in memory for the values of the strings in total for the entire file?

For example, given the four strings above, the total number of characters of string code (2 + 5 + 10 + 6 = 23) minus the total number of characters in memory for string values (0 + 3 + 7 + 1 = 11) is 23 - 11 = 12.
*/

#include <iostream>
#include <fstream>
#include <chrono>
#include <string>

#include <sstream>
#include <vector>
#include <cstdio>
#include <regex>

inline void wait(void)
{
    std::cout << "Press any key to continue...";
    char wait;
    std::cin >> std::noskipws >> wait;
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

    std::fstream decoded("decode.txt", std::ios_base::out);
    if (!decoded.good()) {
        std::cerr << "Cannot use input file \'decode.txt\'!" << std::endl;
        wait();
        std::exit(2);
    }

    std::fstream encoded("encode.txt", std::ios_base::out);
    if (!encoded.good()) {
        std::cerr << "Cannot use input file \'encode.txt\'!" << std::endl;
        wait();
        std::exit(2);
    }

    std::string line;
    try {
        std::size_t encode_size{0};
        std::size_t code_size{0};
        std::size_t in_memory_size{0};
        std::regex lineR{"\"(.*)\""};

        std::vector<std::pair<std::regex, std::string>> decode_trafos{
            {std::regex{"\\\\\\\\"},"\\"}
            ,{std::regex{"\\\\\""},"\""}
            ,{std::regex{"\\\\x[a-f0-9][a-f0-9]"},"H"}
        };

        std::vector<std::pair<std::regex, std::string>> encode_trafos{
            {std::regex{"^\"|\"$"},                     "QQQ"}
            ,{std::regex{"\\\\\\\\"},                   "WWWW"}
            ,{std::regex{"\\\\\""},                     "SSSS"}
            ,{std::regex{"\\\\(x[a-f0-9][a-f0-9])"},    "PP$1"}
        };

        auto start = std::chrono::steady_clock::now();
        std::ostringstream lnstrm;
        std::string header;
        while (std::getline(input, line)) {
            lnstrm << line << " #" << line.length() << std::endl;
            header = lnstrm.str();
            encoded << header;
            decoded << header;
            lnstrm.str("");
            lnstrm.clear();
            
            code_size += line.length();
            
            std::string encoded_line{line};
            encoded << "  BEFORE trafos:" << encoded_line << " #" << encoded_line.length() << std::endl;
            for (const auto& trafo : encode_trafos) {
                encoded_line = std::regex_replace(encoded_line, trafo.first, trafo.second);
            }
            encoded << "  AFTER trafos:" << encoded_line << " #" << encoded_line.length() << std::endl;
            
            encode_size += encoded_line.length();

            std::smatch parts;
            if (std::regex_match(line, parts, lineR)) {
                std::string text{parts[1]};

                decoded << "  BEFORE trafos:" << text << " #" << text.length() << std::endl;
                for (const auto& trafo : decode_trafos) {
                    text = std::regex_replace(text, trafo.first, trafo.second);
                }
                decoded << "  AFTER trafos:" << text << " #" << text.length() << std::endl;

                in_memory_size += text.length();
            } else {
                throw std::invalid_argument("Line does not match the expected pattern: \'" + line + '\'');
            }
        }
        auto duration = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::steady_clock::now() - start);
        std::cout << "Disregarding the whitespace in the file, what is the number of characters of code for string literals minus the number of characters in memory for the values of the strings in total for the entire file?" << std::endl;
        std::cout << "Code size: " << code_size << " - in-memory size: " << in_memory_size << " = " << (code_size - in_memory_size) << '.' << std::endl;
        std::cout << "Your NEW task is to find the total number of characters to represent the newly encoded strings minus the number of characters of code in each original string literal:" << std::endl;
        std::cout << "Newly encoded size: " << encode_size << " - original code size: " << code_size << " = " << (encode_size - code_size) << ". Finished in " << duration.count() << "s" << std::endl;
    } catch (const std::exception& e_) {
        std::cerr << "Caught deadly exception: " << e_.what() << std::endl;
        std::cerr.flush();
        throw;
    } catch (...) {
        std::cerr << "Caught deadly UNKNOWN exception!!!" << std::endl;
        std::cerr.flush();
        throw;
    }

    wait();
    return 0;
}
