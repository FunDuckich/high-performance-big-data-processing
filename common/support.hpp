#pragma once

#ifndef NOMINMAX
#define NOMINMAX
#endif
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace lab {

struct Console {
    bool own_window = false;

    Console() {
        SetConsoleCP(CP_UTF8);
        SetConsoleOutputCP(CP_UTF8);
        DWORD processes[2], input_mode, output_mode;
        own_window = GetConsoleProcessList(processes, 2) == 1
            && GetConsoleMode(GetStdHandle(STD_INPUT_HANDLE), &input_mode)
            && GetConsoleMode(GetStdHandle(STD_OUTPUT_HANDLE), &output_mode);
    }

    ~Console() {
        if (own_window) {
            std::cout << "\nНажмите Enter для выхода..." << std::flush;
            std::cin.get();
        }
    }
};

struct Timer {
    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();

    void reset() { start = std::chrono::steady_clock::now(); }

    double seconds() const {
        return std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
    }
};

inline bool equal(double a, double b) {
    return std::isfinite(a) && std::isfinite(b)
        && std::abs(a - b) <= 1e-10 * (1.0 + std::max(std::abs(a), std::abs(b)));
}

struct Result {
    std::string method, container;
    double seconds, baseline;
    bool correct;
};

inline std::string number(double value, int precision) {
    std::ostringstream text;
    text << std::fixed << std::setprecision(precision) << value;
    return text.str();
}

inline std::size_t text_width(const std::string& text) {
    std::size_t width = 0;
    // У русской буквы несколько байтов UTF-8, но одна позиция в консоли.
    for (unsigned char byte : text)
        if ((byte & 0xC0) != 0x80) ++width;
    return width;
}

inline void print_results(const std::vector<Result>& results,
    std::ostream& out = std::cout, int window_width = 0) {
    const std::vector<std::string> headers{
        "Метод", "Контейнер", "Время, с", "Ускорение", "Проверка"
    };
    std::vector<std::vector<std::string>> rows;
    std::vector<std::size_t> widths;
    for (const auto& header : headers) widths.push_back(text_width(header));
    for (const auto& result : results) {
        rows.push_back({result.method, result.container, number(result.seconds, 6),
            result.seconds > 0 ? number(result.baseline / result.seconds, 3) : "н/д",
            result.correct ? "OK" : "ОШИБКА"});
        for (std::size_t i = 0; i < widths.size(); ++i)
            widths[i] = std::max(widths[i], text_width(rows.back()[i]));
    }

    if (window_width <= 0) {
        CONSOLE_SCREEN_BUFFER_INFO info{};
        window_width = GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info)
            ? info.srWindow.Right - info.srWindow.Left + 1 : 100;
    }
    std::size_t total = 1;
    for (auto width : widths) total += width + 3;
    if (total >= static_cast<std::size_t>(window_width)) {
        for (const auto& row : rows) {
            for (std::size_t i = 0; i < headers.size(); ++i)
                out << headers[i] << ": " << row[i] << '\n';
            out << '\n';
        }
        return;
    }

    const auto border = [&] {
        out << '+';
        for (auto width : widths) out << std::string(width + 2, '-') << '+';
        out << '\n';
    };
    const auto line = [&](const std::vector<std::string>& cells, bool heading) {
        out << '|';
        for (std::size_t i = 0; i < widths.size(); ++i) {
            std::string spaces(widths[i] - text_width(cells[i]), ' ');
            bool right = !heading && (i == 2 || i == 3);
            out << ' ' << (right ? spaces : "") << cells[i] << (right ? "" : spaces) << " |";
        }
        out << '\n';
    };
    border();
    line(headers, true);
    border();
    for (const auto& row : rows) line(row, false);
    border();
}

inline void heading(const char* title, int variant) {
    std::cout << title << ". Исходный вариант: 20; вариант задания: " << variant << ".\n"
        << "Ускорение = время последовательного метода / время метода.\n\n";
}

}
