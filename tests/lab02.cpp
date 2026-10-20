// Подключаем код лабораторной с переименованным входом, не меняя учебную программу.
#define main lab_program_main
#include "../lab02/main.cpp"
#undef main
#include "series_checks.hpp"

int main() {
    check_series();
    std::vector<double> input{0, 0.5, -1, 2};
    std::vector<lab::Result> results;
    check(compare_transform<std::vector<double>>(input, "vector", results), "vector result");
    check(compare_transform<std::deque<double>>(input, "deque", results), "deque result");
    check(compare_transform<std::list<double>>(input, "list", results), "list result");
    check(results.size() == 6, "all methods measured");
    std::cout << "lab02: OK\n";
}
