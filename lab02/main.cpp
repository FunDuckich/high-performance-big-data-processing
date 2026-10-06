#include "experiment.hpp"
#include "series.hpp"
#include <ppl.h>
#include <deque>
#include <list>

template<class Container>
void benchmark_transform(const lab::Options& options, const std::vector<double>& values, const char* name) {
    const Container input(values.begin(), values.end());
    Container sequential(options.size);
    Container parallel(options.size);
    lab::compare_pair(options, name, "std::transform", "parallel_transform",
        [&] { std::fill(sequential.begin(), sequential.end(), 0.0); std::fill(parallel.begin(), parallel.end(), 0.0); },
        [&] { std::transform(input.begin(), input.end(), sequential.begin(), lab::series); },
        [&] { concurrency::parallel_transform(input.begin(), input.end(), parallel.begin(), lab::series); },
        [&] { return lab::verify_values(sequential, parallel); });
}

int main(int argc, char** argv) {
    return lab::entry([&] {
        const auto parsed = lab::parse_options(argc, argv, 2000);
        if (!parsed) { return; }
        lab::banner("Лабораторная II", 9, *parsed, 2000);
        const auto input = lab::transform_input(parsed->size);
        benchmark_transform<std::vector<double>>(*parsed, input, "vector");
        benchmark_transform<std::deque<double>>(*parsed, input, "deque");
        benchmark_transform<std::list<double>>(*parsed, input, "list");
    });
}
