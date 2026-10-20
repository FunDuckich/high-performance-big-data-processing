#include "../common/support.hpp"
#include <ppl.h>
#include <concurrent_vector.h>
#include <concurrent_queue.h>
#include <algorithm>
#include <cmath>
#include <queue>
#include <vector>

constexpr int GRID_SIZE = 400;
constexpr int SERIES_SIZE = 50;
constexpr double PI = 3.14159265358979323846;

struct Evaluation {
    double f = 0;
    double derivative = 0;
};

struct Point {
    int index;
    double x, y, f;
};

Evaluation evaluate(double x, double y) {
    double sx[SERIES_SIZE + 1], sy[SERIES_SIZE + 1], cy[SERIES_SIZE + 1];
    for (int k = 1; k <= SERIES_SIZE; ++k) {
        sx[k] = std::sin(k * x);
        sy[k] = std::sin(k * y);
        cy[k] = std::cos(k * y);
    }
    Evaluation result;
    for (int k = 1; k <= SERIES_SIZE; ++k) {
        for (int j = 1; j <= SERIES_SIZE; ++j) {
            double denominator = (1.0 + k * k * k + j * j * j) * std::sqrt(1.0 + k + j);
            result.f += sx[k] * sy[j] / denominator;
            result.derivative += sx[k] * j * cy[j] / denominator;
        }
    }
    return result;
}

bool selected_point(int row, int column, Point& point) {
    double step = 2.0 * PI / GRID_SIZE;
    double x = row * step, y = column * step;
    Evaluation result = evaluate(x, y);
    point = {row * GRID_SIZE + column, x, y, result.f};
    return result.derivative <= 0;
}

bool same_points(std::vector<Point> actual, const std::vector<Point>& expected) {
    if (actual.size() != expected.size()) return false;
    // Потоки добавляют точки в разном порядке; сравниваем по номеру узла.
    std::sort(actual.begin(), actual.end(),
        [](const Point& a, const Point& b) { return a.index < b.index; });
    for (std::size_t i = 0; i < actual.size(); ++i) {
        if (actual[i].index != expected[i].index
            || !lab::equal(actual[i].x, expected[i].x)
            || !lab::equal(actual[i].y, expected[i].y)
            || !lab::equal(actual[i].f, expected[i].f)) return false;
    }
    return true;
}

int main() {
    lab::Console console;
    try {
        lab::heading("Лабораторная VI", 8);
        std::cout << "Сетка: " << GRID_SIZE << " x " << GRID_SIZE
            << "; члены ряда: " << SERIES_SIZE << " x " << SERIES_SIZE
            << "\nУсловие отбора: производная по y <= 0.\nstd::vector..." << std::endl;
        std::vector<Point> reference;
        lab::Timer timer;
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                Point point;
                if (selected_point(i, j, point)) reference.push_back(point);
            }
        }
        double vector_time = timer.seconds();

        std::cout << "concurrent_vector..." << std::endl;
        concurrency::concurrent_vector<Point> parallel_vector;
        timer.reset();
        concurrency::parallel_for(0, GRID_SIZE, [&](int i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                Point point;
                if (selected_point(i, j, point)) parallel_vector.push_back(point);
            }
        });
        double parallel_vector_time = timer.seconds();
        bool vector_correct = same_points(
            std::vector<Point>(parallel_vector.begin(), parallel_vector.end()), reference);

        std::cout << "std::queue..." << std::endl;
        std::queue<Point> sequential_queue;
        timer.reset();
        for (int i = 0; i < GRID_SIZE; ++i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                Point point;
                if (selected_point(i, j, point)) sequential_queue.push(point);
            }
        }
        double queue_time = timer.seconds();
        std::vector<Point> queue_points;
        while (!sequential_queue.empty()) {
            queue_points.push_back(sequential_queue.front());
            sequential_queue.pop();
        }
        bool queue_correct = same_points(queue_points, reference);

        std::cout << "concurrent_queue..." << std::endl;
        concurrency::concurrent_queue<Point> parallel_queue;
        timer.reset();
        concurrency::parallel_for(0, GRID_SIZE, [&](int i) {
            for (int j = 0; j < GRID_SIZE; ++j) {
                Point point;
                if (selected_point(i, j, point)) parallel_queue.push(point);
            }
        });
        double parallel_queue_time = timer.seconds();
        queue_points.clear();
        Point point;
        while (parallel_queue.try_pop(point)) queue_points.push_back(point);
        bool parallel_queue_correct = same_points(queue_points, reference);

        lab::print_results({
            {"for", "vector", vector_time, vector_time, true},
            {"parallel_for", "concurrent_vector", parallel_vector_time, vector_time, vector_correct},
            {"for", "queue", queue_time, queue_time, queue_correct},
            {"parallel_for", "concurrent_queue", parallel_queue_time, queue_time, parallel_queue_correct}
        });
        std::cout << "Сохранено точек: " << reference.size() << " из "
            << GRID_SIZE * GRID_SIZE << ".\n";
        return vector_correct && queue_correct && parallel_queue_correct ? 0 : 1;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти. Уменьшите размер сетки в начале main.cpp и пересоберите программу.\n";
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
        return 1;
    }
}
