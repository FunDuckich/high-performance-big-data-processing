#include "../common/support.hpp"
#include <ppl.h>
#include <cmath>
#include <vector>

constexpr double Y = 1.0;
constexpr double UPPER_LIMIT = 5000.0;
constexpr int SIMPSON_PAIRS = 200000;
constexpr int PRODUCT_TERMS = 1000;

std::vector<double> make_factors(int last_index = PRODUCT_TERMS) {
    std::vector<double> factors(last_index + 1);
    for (int k = 0; k <= last_index; ++k)
        factors[k] = std::exp(-std::sqrt(static_cast<double>(k)));
    return factors;
}

double function(double x, double y, const std::vector<double>& factors) {
    double product = std::exp(-std::sqrt(x));
    double cos_y = std::cos(y);
    for (double q : factors)
        product *= std::cos(x * q / (1.0 + cos_y * cos_y * q));
    return product;
}

double integrand(double t, double y, const std::vector<double>& factors) {
    // Замена x = t^2, dx = 2t dt устраняет особенность производной в нуле.
    return 2.0 * t * function(t * t, y, factors);
}

double simpson(double y, int pairs, const std::vector<double>& factors,
    double upper = UPPER_LIMIT) {
    double end = std::sqrt(upper);
    double h = end / (2 * pairs);
    double sum = 0;
    for (int i = 1; i < 2 * pairs; ++i) {
        int weight = i % 2 == 0 ? 2 : 4;
        sum += weight * integrand(i * h, y, factors);
    }
    return h / 3.0 * (integrand(0, y, factors) + sum + integrand(end, y, factors));
}

double parallel_simpson(double y, int pairs, const std::vector<double>& factors,
    double upper = UPPER_LIMIT) {
    double end = std::sqrt(upper);
    double h = end / (2 * pairs);
    concurrency::combinable<double> sums([] { return 0.0; });
    concurrency::parallel_for(1, 2 * pairs, [&](int i) {
        int weight = i % 2 == 0 ? 2 : 4;
        sums.local() += weight * integrand(i * h, y, factors);
    });
    double sum = sums.combine([](double a, double b) { return a + b; });
    return h / 3.0 * (integrand(0, y, factors) + sum + integrand(end, y, factors));
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная VII", 5);
        std::cout << "y = " << Y << "; верхний предел x: " << UPPER_LIMIT
            << "; интервалы Симпсона: " << 2 * SIMPSON_PAIRS
            << "; множители: k = 0..." << PRODUCT_TERMS
            << "\nПоследовательное интегрирование..." << std::endl;
        const auto factors = make_factors();
        lab::Timer timer;
        double sequential = simpson(Y, SIMPSON_PAIRS, factors);
        double sequential_time = timer.seconds();
        std::cout << "Параллельное интегрирование..." << std::endl;
        timer.reset();
        double parallel = parallel_simpson(Y, SIMPSON_PAIRS, factors);
        double parallel_time = timer.seconds();
        bool correct = lab::equal(sequential, parallel);
        lab::print_results({
            {"Симпсон: for", "-", sequential_time, sequential_time, correct},
            {"Симпсон: combinable", "-", parallel_time, sequential_time, correct}
        });
        std::cout << std::setprecision(14) << "Интеграл последовательно: " << sequential
            << "\nИнтеграл параллельно:     " << parallel
            << "\nАбсолютное расхождение:  " << std::scientific << std::abs(sequential - parallel) << '\n';
        return correct ? 0 : 1;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти для вычислений.\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
