#define main lab_program_main
#include "../lab04/main.cpp"
#undef main
#include "check.hpp"
#include <limits>

int main() {
    check(value(0) == 0 && lab::equal(value(1), 4.0e9 * std::sin(1.0) / 2), "input sequence");
    const double tiny = std::numeric_limits<double>::denorm_min();
    const double big = std::numeric_limits<double>::max();
    for (const auto& values : std::vector<std::vector<double>>{
        {}, {1}, {1, 1, 1}, {-3, -1, 0, 1, 3}, {3, 1, 0, -1, -3},
        {-4e9, 4e9, -0.0, 0.0, -1.5, 1.5, -1.5, std::nextafter(1.5, 2.0), tiny, -tiny, big, -big}
    }) {
        auto expected = values;
        std::sort(expected.begin(), expected.end());
        auto actual = values;
        concurrency::parallel_sort(actual.begin(), actual.end());
        check(actual == expected, "parallel_sort");
        actual = values;
        concurrency::parallel_buffered_sort(actual.begin(), actual.end());
        check(actual == expected, "buffered_sort");
        actual = values;
        concurrency::parallel_radixsort(actual.begin(), actual.end(), radix_key);
        check(actual == expected, "radix order and values");
    }
    std::cout << "lab04: OK\n";
}
