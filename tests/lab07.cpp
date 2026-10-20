#define main lab_program_main
#include "../lab07/main.cpp"
#undef main
#include "check.hpp"

int main() {
    auto factors = make_factors();
    check(function(0,1,factors) == 1 && integrand(0,1,factors) == 0, "origin");
    const double fixtures[][2] = {
        {0, 0.7052061306052992},
        {1, 0.5198731621656486},
        {1.5707963267948966, 0.39991601696482804}
    };
    for (const auto& fixture : fixtures) {
        double first = simpson(fixture[0], 10000, factors);
        double refined = simpson(fixture[0], 20000, factors);
        double parallel = parallel_simpson(fixture[0], 10000, factors);
        check(std::abs(first-fixture[1]) < 1e-8, "independent integral reference");
        check(std::abs(first-refined) < 1e-8, "grid convergence");
        check(lab::equal(first, parallel), "parallel reduction");
    }
    double baseline = simpson(1,10000,factors);
    check(std::abs(baseline - simpson(1,10000,make_factors(1500))) < 1e-8, "product convergence");
    check(std::abs(baseline - simpson(1,20000,factors,10000)) < 1e-8, "upper limit convergence");
    check(lab::equal(function(1,1,factors),function(1,-1,factors)), "even in y");
    std::cout << "lab07: OK\n";
}
