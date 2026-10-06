#pragma once

#include <algorithm>
#include <cmath>
#include <vector>

namespace lab {

inline int series_limit(double x) {
    return std::max(20, static_cast<int>(std::floor(20.0 * std::abs(x))));
}

inline double series(double x) {
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

inline std::vector<double> transform_input(std::size_t size) {
    std::vector<double> values(size);
    for (std::size_t i = 0; i < size; ++i) {
        values[i] = 100.0 * std::cos(static_cast<double>(i + 1));
    }
    return values;
}

inline double sort_value(std::size_t n) {
    const double root = std::sqrt(static_cast<double>(n));
    return root / (1.0 + root) * std::sin(static_cast<double>(n));
}

}
