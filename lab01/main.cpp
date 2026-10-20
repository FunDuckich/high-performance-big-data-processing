#include "../common/support.hpp"
#include <ppl.h>
#include <algorithm>
#include <cmath>
#include <vector>
#include <deque>
#include <list>

constexpr int N = 2000;

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

template<class Container>
bool compare_for_each(const std::vector<double>& input, const char* name,
    std::vector<lab::Result>& results) {
    std::cout << "for_each: " << name << "..." << std::endl;
    Container sequential(input.begin(), input.end());
    Container parallel(input.begin(), input.end());
    lab::Timer timer;
    std::for_each(sequential.begin(), sequential.end(), [](double& x) { x = series(x); });
    double sequential_time = timer.seconds();
    timer.reset();
    concurrency::parallel_for_each(parallel.begin(), parallel.end(), [](double& x) { x = series(x); });
    double parallel_time = timer.seconds();
    bool correct = std::equal(sequential.begin(), sequential.end(), parallel.begin(), lab::equal);
    results.push_back({"std::for_each", name, sequential_time, sequential_time, correct});
    results.push_back({"parallel_for_each", name, parallel_time, sequential_time, correct});
    return correct;
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная I", 9);
        std::cout << "Количество значений: " << N << "\nfor / parallel_for..." << std::endl;
        std::vector<double> input(N), sequential(N), parallel(N);
        for (int i = 0; i < N; ++i) input[i] = 100.0 * std::cos(i + 1.0);
        lab::Timer timer;
        for (int i = 0; i < N; ++i) sequential[i] = series(input[i]);
        double sequential_time = timer.seconds();
        timer.reset();
        concurrency::parallel_for(0, N, [&](int i) { parallel[i] = series(input[i]); });
        double parallel_time = timer.seconds();
        bool correct = std::equal(sequential.begin(), sequential.end(), parallel.begin(), lab::equal);
        std::vector<lab::Result> results{
            {"for", "vector", sequential_time, sequential_time, correct},
            {"parallel_for", "vector", parallel_time, sequential_time, correct}
        };
        correct &= compare_for_each<std::vector<double>>(input, "vector", results);
        correct &= compare_for_each<std::deque<double>>(input, "deque", results);
        correct &= compare_for_each<std::list<double>>(input, "list", results);
        lab::print_results(results);
        return correct ? 0 : 1;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти. Уменьшите размер в начале main.cpp и пересоберите программу.\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
