#include "experiment.hpp"
#include "series.hpp"
#include <ppl.h>
#include <array>

void require(bool condition, const char* message) {
    if (!condition) { throw std::runtime_error(message); }
}

int main() {
    return lab::entry([] {
        // Эталоны вычислены отдельно через Decimal с точностью 55 знаков и рядом Тейлора для cos.
        const std::array<std::pair<double, double>, 8> fixtures{{
            {0.0, 0.0},
            {0.1, 0.05542427005865035624039846860824735},
            {0.5, 0.6106010967612349151974036334394866},
            {1.0, 1.171820231084450218342384050318139},
            {1.049, 1.205629248953368399923153069217200},
            {1.05, 1.208256008864154343304961073407868},
            {1.051, 1.208789892743944699621199671950464},
            {2.0, 0.8767165756346780109461172460239964}
        }};
        for (const auto& [x, expected] : fixtures) {
            require(lab::close(lab::series(x), expected), "Функция не совпадает с независимым эталоном.");
            require(lab::close(lab::series(-x), expected), "Нарушена чётность функции.");
        }
        require(lab::series_limit(1.049) == 20 && lab::series_limit(1.05) == 21
            && lab::series_limit(1.051) == 21, "Неверное усечение ряда.");
        const auto input = lab::transform_input(2);
        require(lab::close(input[0], 100 * std::cos(1.0))
            && lab::close(input[1], 100 * std::cos(2.0)), "Неверная индексация входов I и II.");
        require(lab::sort_value(0) == 0.0 && lab::close(lab::sort_value(1), std::sin(1.0) / 2.0),
            "Неверная последовательность III.");
        require(lab::median({3, 1, 2}) == 2 && lab::median({4, 1, 3, 2}) == 2.5, "Неверная медиана.");
        require(!lab::close(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity())
            && !lab::close(std::numeric_limits<double>::quiet_NaN(), 0), "Принято неконечное значение.");
        bool rejected = false;
        try { lab::verify_values(std::vector<double>{1}, std::vector<double>{2}); }
        catch (const std::runtime_error&) { rejected = true; }
        require(rejected, "Проверка пропустила ошибочный результат.");
        for (const auto& values : std::vector<std::vector<double>>{
            {}, {1}, {2, -1, 2, 0, -1}, {1, 2, 3}, {3, 2, 1}, {4, 4, 4}}) {
            auto reference = values;
            std::sort(reference.begin(), reference.end());
            auto actual = values;
            concurrency::parallel_sort(actual.begin(), actual.end());
            require(actual == reference, "Ошибка parallel_sort на граничном наборе.");
            actual = values;
            concurrency::parallel_buffered_sort(actual.begin(), actual.end());
            require(actual == reference, "Ошибка parallel_buffered_sort на граничном наборе.");
        }
        std::cout << "Численные и граничные проверки пройдены.\n";
    });
}
