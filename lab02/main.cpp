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
bool compare_transform(const std::vector<double>& values, const char* name,
    std::vector<lab::Result>& results) {
    std::cout << "transform: " << name << "..." << std::endl;
    const Container input(values.begin(), values.end());
    Container sequential(input.size()), parallel(input.size());
    lab::Timer timer;
    std::transform(input.begin(), input.end(), sequential.begin(), series);
    double sequential_time = timer.seconds();
    timer.reset();
    concurrency::parallel_transform(input.begin(), input.end(), parallel.begin(), series);
    double parallel_time = timer.seconds();
    bool correct = std::equal(sequential.begin(), sequential.end(), parallel.begin(), lab::equal);
    results.push_back({"std::transform", name, sequential_time, sequential_time, correct});
    results.push_back({"parallel_transform", name, parallel_time, sequential_time, correct});
    return correct;
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная II", 9);
        std::cout << "Количество значений: " << N << '\n';
        std::vector<double> input(N);
        for (int i = 0; i < N; ++i) input[i] = 100.0 * std::cos(i + 1.0);
        std::vector<lab::Result> results;
        bool correct = compare_transform<std::vector<double>>(input, "vector", results);
        correct &= compare_transform<std::deque<double>>(input, "deque", results);
        correct &= compare_transform<std::list<double>>(input, "list", results);
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
