#include "experiment.hpp"
#include "series.hpp"
#include <ppl.h>
#include <array>

int main(int argc, char** argv) {
    return lab::entry([&] {
        const auto parsed = lab::parse_options(argc, argv, 50000000);
        if (!parsed) { return; }
        const auto options = *parsed;
        lab::banner("Лабораторная III", 8, options, 50000000);
        std::cout << "Подготовка исходной последовательности...\n" << std::flush;
        std::vector<double> input(options.size);
        for (std::size_t n = 0; n < options.size; ++n) { input[n] = lab::sort_value(n); }
        std::vector<double> reference(options.size);
        std::vector<double> work(options.size);
        std::array<std::vector<double>, 3> times;
        for (std::size_t repetition = 0; repetition < options.repeats; ++repetition) {
            std::cout << "Измерение " << repetition + 1 << '/' << options.repeats << ": std::sort...\n" << std::flush;
            reference = input;
            times[0].push_back(lab::elapsed([&] { std::sort(reference.begin(), reference.end()); }));
            if (!std::is_sorted(reference.begin(), reference.end())) {
                throw std::runtime_error("std::sort: нарушен порядок сортировки.");
            }
            for (int step = 0; step < 2; ++step) {
                const int method = repetition % 2 == 0 ? step + 1 : 2 - step;
                work = input;
                std::cout << "Измерение " << repetition + 1 << '/' << options.repeats << ": "
                    << (method == 1 ? "parallel_sort" : "parallel_buffered_sort") << "...\n" << std::flush;
                times[method].push_back(lab::elapsed([&] {
                    if (method == 1) { concurrency::parallel_sort(work.begin(), work.end()); }
                    else { concurrency::parallel_buffered_sort(work.begin(), work.end()); }
                }));
                if (!std::is_sorted(work.begin(), work.end()) || work != reference) {
                    throw std::runtime_error("Параллельная сортировка не совпадает с std::sort.");
                }
            }
        }
        const double baseline = lab::median(times[0]);
        lab::table_header();
        lab::row("std::sort", "vector", baseline, baseline);
        lab::row("parallel_sort", "vector", lab::median(times[1]), baseline);
        lab::row("parallel_buffered_sort", "vector", lab::median(times[2]), baseline);
        lab::table_border();
        std::cout << "Минимум: " << std::fixed << std::setprecision(12) << reference.front()
            << "; максимум: " << reference.back() << '\n';
    });
}
