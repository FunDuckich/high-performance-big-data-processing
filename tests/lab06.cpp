#define main lab_program_main
#include "../lab06/main.cpp"
#undef main
#include "check.hpp"

int main() {
    check(lab::equal(evaluate(0.4, 0.7).f, 0.16097186038758582), "independent function reference");
    for (double x : {0.0, 0.4, 1.2, 3.0}) {
        for (double y : {0.0, 0.7, 2.3, 5.0}) {
            const double h = 1e-6;
            double numerical = (evaluate(x, y+h).f - evaluate(x, y-h).f) / (2*h);
            check(std::abs(evaluate(x,y).derivative - numerical) < 1e-7, "analytic derivative");
        }
    }
    Point point;
    check(selected_point(0, 0, point), "zero derivative belongs to selection");
    check(point.index == 0 && point.f == 0, "grid origin");
    std::vector<Point> expected{{0,0,0,0}, {1,0,1,2}, {2,1,0,3}};
    auto actual = expected;
    std::reverse(actual.begin(),actual.end());
    check(same_points(actual, expected), "unordered result");
    actual[0] = actual[1];
    check(!same_points(actual, expected), "duplicate replaces missing point");
    std::cout << "lab06: OK\n";
}
