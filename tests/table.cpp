#include "../common/support.hpp"
#include "check.hpp"
#include <limits>

int main() {
    check(lab::text_width("Привет, мир!") == 12, "UTF-8 width");
    check(!lab::equal(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity()), "infinity");
    check(!lab::equal(std::numeric_limits<double>::quiet_NaN(),0), "NaN");
    std::vector<lab::Result> rows{
        {"Очень длинное название метода", "concurrent_vector", 12345.123456, 1, false},
        {"for", "vector", 0, 1, true},
        {"parallel_for", "vector", 0.000001, 100, true}
    };
    std::ostringstream wide;
    lab::print_results(rows,wide,160);
    std::istringstream lines(wide.str());
    std::string line;
    std::size_t width = 0;
    while (std::getline(lines,line)) {
        if (!width) width = lab::text_width(line);
        check(lab::text_width(line) == width, "aligned rows");
        check(line.front() == '+' || line.front() == '|', "table frame");
    }
    check(wide.str().find("ОШИБКА") != std::string::npos && wide.str().find("н/д") != std::string::npos, "status and zero time");
    std::ostringstream narrow;
    lab::print_results(rows,narrow,40);
    check(narrow.str().find("Метод: ") != std::string::npos && narrow.str().find('|') == std::string::npos, "narrow fallback");
    std::cout << "table: OK\n";
}
