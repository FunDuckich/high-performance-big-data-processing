#pragma once

#include <windows.h>
#include <algorithm>
#include <array>
#include <charconv>
#include <chrono>
#include <cmath>
#include <cstddef>
#include <iomanip>
#include <iostream>
#include <limits>
#include <new>
#include <optional>
#include <sstream>
#include <stdexcept>
#include <string>
#include <string_view>
#include <thread>
#include <vector>

namespace lab {

struct Options {
    std::size_t size;
    std::size_t repeats = 1;
};

inline std::size_t positive_number(std::string_view text) {
    std::size_t result = 0;
    const auto parsed = std::from_chars(text.data(), text.data() + text.size(), result);
    if (text.empty() || parsed.ec != std::errc{} || parsed.ptr != text.data() + text.size()
        || result == 0 || result > static_cast<std::size_t>((std::numeric_limits<int>::max)())) {
        throw std::invalid_argument("Ожидалось целое число от 1 до 2147483647.");
    }
    return result;
}

inline std::optional<Options> parse_options(int argc, char** argv, std::size_t default_size) {
    Options options{default_size};
    for (int i = 1; i < argc; ++i) {
        const std::string_view argument(argv[i]);
        if (argument == "--help") {
            std::cout << "Параметры: --size N --repeats K --help\n"
                << "Размер по умолчанию: " << default_size << "; повторов: 1.\n"
                << "При нескольких повторах выводится медиана времени.\n";
            return std::nullopt;
        }
        if (argument != "--size" && argument != "--repeats") {
            throw std::invalid_argument("Неизвестный параметр: " + std::string(argument));
        }
        if (++i == argc) {
            throw std::invalid_argument("После " + std::string(argument) + " требуется число.");
        }
        const auto value = positive_number(argv[i]);
        if (argument == "--size") {
            options.size = value;
        } else {
            if (value > 1000) {
                throw std::invalid_argument("Число повторов должно быть не больше 1000.");
            }
            options.repeats = value;
        }
    }
    return options;
}

inline void banner(const char* name, int variant, const Options& options, std::size_t default_size) {
    std::cout << name << "; исходный вариант: 20; вариант в файле: " << variant << '\n'
        << "Размер: " << options.size << "; по заданию: " << default_size
        << "; повторов: " << options.repeats
        << "; логических процессоров: " << std::thread::hardware_concurrency() << '\n';
    if (options.size != default_size) {
        std::cout << "Используется изменённый размер, результаты не относятся к полному заданию.\n";
    }
    std::cout << "Время включает только вычисление.\n"
        << "Ускорение = время последовательного алгоритма / время метода.\n\n" << std::flush;
}

template<class Operation>
double elapsed(Operation&& operation) {
    const auto start = std::chrono::steady_clock::now();
    operation();
    return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
}

inline double median(std::vector<double> values) {
    if (values.empty()) {
        throw std::invalid_argument("Нельзя вычислить медиану пустого набора.");
    }
    std::sort(values.begin(), values.end());
    const auto middle = values.size() / 2;
    return values.size() % 2 ? values[middle] : (values[middle - 1] + values[middle]) / 2.0;
}

inline bool close(double first, double second) {
    return std::isfinite(first) && std::isfinite(second)
        && std::abs(first - second) <= 1e-10 * (1.0 + std::max(std::abs(first), std::abs(second)));
}

template<class First, class Second>
double verify_values(const First& first, const Second& second) {
    if (first.size() != second.size()) {
        throw std::runtime_error("Размеры результатов различаются.");
    }
    double error = 0.0;
    auto right = second.begin();
    std::size_t index = 0;
    for (auto left = first.begin(); left != first.end(); ++left, ++right, ++index) {
        if (!close(*left, *right)) {
            throw std::runtime_error("Результаты не совпадают или не конечны, индекс " + std::to_string(index));
        }
        error = std::max(error, std::abs(*left - *right));
    }
    return error;
}

inline constexpr std::array<std::size_t, 5> table_widths{22, 9, 12, 10, 8};

inline void table_border() {
    std::cout << '+';
    for (const auto width : table_widths) {
        std::cout << std::string(width + 2, '-') << '+';
    }
    std::cout << '\n';
}

inline void table_line(const std::array<std::string_view, 5>& cells) {
    std::cout << '|';
    for (std::size_t i = 0; i < cells.size(); ++i) {
        // Русские буквы в UTF-8 занимают несколько байтов, но одну позицию в консоли.
        const auto length = static_cast<std::size_t>(std::count_if(cells[i].begin(), cells[i].end(),
            [](unsigned char byte) { return (byte & 0xC0) != 0x80; }));
        const std::string padding(table_widths[i] > length ? table_widths[i] - length : 0, ' ');
        const bool align_right = i == 2 || i == 3;
        std::cout << ' ' << (align_right ? padding : "") << cells[i]
            << (align_right ? "" : padding) << " |";
    }
    std::cout << '\n';
}

inline void table_header() {
    table_border();
    table_line({"Алгоритм", "Контейнер", "Время, с", "Ускорение", "Проверка"});
    table_border();
}

inline void row(const char* method, const char* container, double seconds, double baseline) {
    std::ostringstream time;
    time << std::fixed << std::setprecision(6) << seconds;
    std::ostringstream speedup;
    if (seconds > 0.0) {
        speedup << std::fixed << std::setprecision(3) << baseline / seconds;
    } else {
        speedup << "н/д";
    }
    table_line({method, container, time.str(), speedup.str(), "OK"});
    std::cout << std::flush;
}

template<class Prepare, class Sequential, class Parallel, class Verify>
void compare_pair(const Options& options, const char* container, const char* sequential_name,
    const char* parallel_name, Prepare prepare, Sequential sequential, Parallel parallel, Verify verify) {
    std::vector<double> sequential_times;
    std::vector<double> parallel_times;
    double max_error = 0.0;
    for (std::size_t repetition = 0; repetition < options.repeats; ++repetition) {
        std::cout << "Измерение " << repetition + 1 << '/' << options.repeats
            << ": " << sequential_name << " / " << parallel_name << " (" << container << ")...\n"
            << std::flush;
        prepare();
        if (repetition % 2 == 0) {
            sequential_times.push_back(elapsed(sequential));
            parallel_times.push_back(elapsed(parallel));
        } else {
            parallel_times.push_back(elapsed(parallel));
            sequential_times.push_back(elapsed(sequential));
        }
        max_error = std::max(max_error, verify());
    }
    const double baseline = median(sequential_times);
    table_header();
    row(sequential_name, container, baseline, baseline);
    row(parallel_name, container, median(parallel_times), baseline);
    table_border();
    std::cout << "Максимальная абсолютная ошибка: " << std::scientific << max_error << "\n\n";
}

template<class Main>
int entry(Main body) {
    SetConsoleOutputCP(CP_UTF8);
    try {
        body();
        return 0;
    } catch (const std::bad_alloc&) {
        std::cerr << "Недостаточно памяти. Закройте другие программы или уменьшите --size.\n";
    } catch (const std::invalid_argument& error) {
        std::cerr << "Ошибка параметров: " << error.what() << " Используйте --help.\n";
        return 2;
    } catch (const std::exception& error) {
        std::cerr << "Ошибка: " << error.what() << '\n';
    } catch (...) {
        std::cerr << "Неизвестная ошибка выполнения.\n";
    }
    return 1;
}

}
