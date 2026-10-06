#include "experiment.hpp"
#include "series.hpp"
#include <ppl.h>
#include <deque>
#include <list>

template<class Container>
void benchmark_for_each(const lab::Options& options, const std::vector<double>& input, const char* name) {
    const Container original(input.begin(), input.end());
    Container sequential(original);
    Container parallel(original);
    lab::compare_pair(options, name, "std::for_each", "parallel_for_each",
        [&] { sequential = original; parallel = original; },
        [&] { std::for_each(sequential.begin(), sequential.end(), [](double& x) { x = lab::series(x); }); },
        [&] { concurrency::parallel_for_each(parallel.begin(), parallel.end(), [](double& x) { x = lab::series(x); }); },
        [&] { return lab::verify_values(sequential, parallel); });
}

int main(int argc, char** argv) {
    return lab::entry([&] {
        const auto parsed = lab::parse_options(argc, argv, 2000);
        if (!parsed) { return; }
        const auto options = *parsed;
        lab::banner("Лабораторная I", 9, options, 2000);
        const auto input = lab::transform_input(options.size);
        std::vector<double> sequential(options.size);
        std::vector<double> parallel(options.size);
        lab::compare_pair(options, "vector", "for", "parallel_for",
            [&] { std::fill(sequential.begin(), sequential.end(), 0.0); std::fill(parallel.begin(), parallel.end(), 0.0); },
            [&] {
                for (std::size_t i = 0; i < options.size; ++i) { sequential[i] = lab::series(input[i]); }
            },
            [&] {
                concurrency::parallel_for(std::size_t{0}, options.size,
                    [&](std::size_t i) { parallel[i] = lab::series(input[i]); });
            },
            [&] { return lab::verify_values(sequential, parallel); });
        benchmark_for_each<std::vector<double>>(options, input, "vector");
        benchmark_for_each<std::deque<double>>(options, input, "deque");
        benchmark_for_each<std::list<double>>(options, input, "list");
    });
}
