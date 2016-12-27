#include <iostream>
#include <fstream>
#include <regex>
#include <string>
#include <algorithm>

void wait(void)
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

    std::string box_size;
    unsigned long long total_surface_area{0};
    unsigned long long total_ribbon_length{0};
    std::regex size_pattern ("(\\d+)x(\\d+)x(\\d+)");
    while (input >> box_size) {
        //std::cout << "DEBUG input:" << box_size << std::endl;
        std::smatch match_parts;
        if (std::regex_match(box_size, match_parts, size_pattern)) {
            if (match_parts.size() != 4) {
                throw std::invalid_argument("Wrong input file!");
            }
            
            std::vector<unsigned long long> dimensions(match_parts.size() - 1);
            for (std::size_t i = 1; i < match_parts.size(); ++i) {
                dimensions[i-1] = std::stoull(match_parts[i].str());
                //std::cout << "    DEBUG " << dimensions[i-1] << std::endl;
            }

            const auto& l = dimensions[0];
            const auto& w = dimensions[1];
            const auto& h = dimensions[2];
            const auto lw = l*w;
            const auto wh = w*h;
            const auto hl = h*l;
            unsigned long long area{2 * lw + 2 * wh + 2 * hl};
            
            const auto min_area = std::min({lw, wh, hl});
            const auto area_of_needed_wrapper = area + min_area;
            
            total_surface_area += area_of_needed_wrapper;
                                    
            std::sort(dimensions.begin(), dimensions.end());
            unsigned long long ribbon_length{2 * (dimensions[0] + dimensions[1])};
            unsigned long long bow{1};
            std::for_each(dimensions.cbegin(), dimensions.cend(), [&bow](const auto& d_) { bow *= d_; });
            total_ribbon_length += ribbon_length + bow;
            
            std::cout << "DEBUG l:" << l << ";w:" << w << ";h:" << h << " -> "
                << area << " + " << min_area << " = " << area_of_needed_wrapper
                << "\n    ribbon:" << ribbon_length << " + bow:" << bow << std::endl;
        } else {
            throw std::invalid_argument("Wrong input file!");
        }
    }
    std::cout << "The total square feet of wrapping paper the elves should order is " << total_surface_area << ".\n" <<
        "The total feet of ribbon they should order is " << total_ribbon_length << std::endl;
    
    wait();
    return 0;
}
