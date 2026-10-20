#include "../common/support.hpp"
#include <ppl.h>
#include <algorithm>
#include <cmath>
#include <vector>

constexpr int N = 1000;

int series_limit(double x) {
    return std::max(20, static_cast<int>(std::floor(20.0 * std::abs(x))));
}

double series(double x) {
    const int limit = series_limit(x);
    const double x2 = x * x;
    std::vector<double> squares(limit + 1);
    std::vector<double> fourths(limit + 1);
    std::vector<double> cosines(limit);
    for (int k = 0; k <= limit; ++k) {
        squares[k] = static_cast<double>(k) * k;
        fourths[k] = squares[k] * squares[k];
    }
    // Косинус зависит только от разности индексов; порядок суммирования сохраняется.
    for (int difference = 0; difference < limit; ++difference) {
        cosines[difference] = std::cos(difference * x);
    }
    double sum = 0.0;
    for (int k = 1; k <= limit; ++k) {
        const double denominator = x2 + fourths[k];
        for (int j = 1; j <= limit; ++j) {
            sum += x2 * (squares[k] + squares[j]) / (denominator + fourths[j])
                * cosines[std::abs(k - j)];
        }
    }
    return sum;
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная V", 9);
        std::cout << "Количество значений и задач: " << N << "\nПоследовательный расчёт..." << std::endl;
        std::vector<double> input(N), sequential(N), parallel(N);
        for (int i = 0; i < N; ++i) input[i] = 100.0 * std::cos(i + 1.0);
        lab::Timer timer;
        for (int i = 0; i < N; ++i) sequential[i] = series(input[i]);
        double sequential_time = timer.seconds();
        std::cout << "Расчёт через task_group..." << std::endl;
        timer.reset();
        concurrency::task_group tasks;
        for (int i = 0; i < N; ++i)
            tasks.run([&, i] { parallel[i] = series(input[i]); });
        tasks.wait();
        double parallel_time = timer.seconds();
        bool correct = std::equal(sequential.begin(), sequential.end(), parallel.begin(), lab::equal);
        lab::print_results({
            {"for", "vector", sequential_time, sequential_time, correct},
            {"task_group", "vector", parallel_time, sequential_time, correct}
        });
        return correct ? 0 : 1;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти. Уменьшите размер в начале main.cpp и пересоберите программу.\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
