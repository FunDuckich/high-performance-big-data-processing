// Подключаем код лабораторной с переименованным входом, не меняя учебную программу.
#define main lab_program_main
#include "../lab05/main.cpp"
#undef main
#include "series_checks.hpp"

int main() {
    check_series();
    std::cout << "lab05: OK\n";
}
