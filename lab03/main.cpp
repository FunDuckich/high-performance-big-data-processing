#include "../common/support.hpp"
#include <ppl.h>
#include <algorithm>
#include <cmath>
#include <vector>

constexpr int N = 50000000;

double value(int n) {
    double root = std::sqrt(static_cast<double>(n));
    return root / (1.0 + root) * std::sin(static_cast<double>(n));
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная III", 8);
        std::cout << "Количество значений: " << N << "\nПодготовка данных..." << std::endl;
        std::vector<double> input(N);
        for (int n = 0; n < N; ++n) input[n] = value(n);
        std::vector<double> reference(input), work(input);
        std::cout << "std::sort..." << std::endl;
        lab::Timer timer;
        std::sort(reference.begin(), reference.end());
        double baseline = timer.seconds();
        bool correct = std::is_sorted(reference.begin(), reference.end());
        std::vector<lab::Result> results{{"std::sort", "vector", baseline, baseline, correct}};
        work = input;
        std::cout << "parallel_sort..." << std::endl;
        timer.reset();
        concurrency::parallel_sort(work.begin(), work.end());
        double parallel_sort_time = timer.seconds();
        bool parallel_sort_correct = work == reference;
        correct &= parallel_sort_correct;
        results.push_back({"parallel_sort", "vector", parallel_sort_time, baseline, parallel_sort_correct});
        work = input;
        std::cout << "parallel_buffered_sort..." << std::endl;
        timer.reset();
        concurrency::parallel_buffered_sort(work.begin(), work.end());
        double parallel_buffered_sort_time = timer.seconds();
        bool parallel_buffered_sort_correct = work == reference;
        correct &= parallel_buffered_sort_correct;
        results.push_back({"parallel_buffered_sort", "vector", parallel_buffered_sort_time, baseline, parallel_buffered_sort_correct});
        lab::print_results(results);
        std::cout << "Минимум: " << std::setprecision(12) << reference.front()
            << "; максимум: " << reference.back() << '\n';
        return correct ? 0 : 1;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти. Уменьшите размер в начале main.cpp и пересоберите программу.\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
