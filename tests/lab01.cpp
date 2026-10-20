// Подключаем код лабораторной с переименованным входом, не меняя учебную программу.
#define main lab_program_main
#include "../lab01/main.cpp"
#undef main
#include "series_checks.hpp"

int main() {
    check_series();
    std::vector<double> input{0, 0.5, -1, 2};
    std::vector<lab::Result> results;
    check(compare_for_each<std::vector<double>>(input, "vector", results), "vector result");
    check(compare_for_each<std::deque<double>>(input, "deque", results), "deque result");
    check(compare_for_each<std::list<double>>(input, "list", results), "list result");
    check(results.size() == 6, "all methods measured");
    std::cout << "lab01: OK\n";
}
