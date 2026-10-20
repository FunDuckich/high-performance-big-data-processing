#pragma once
#include "check.hpp"

void check_series() {
    // Независимые эталоны: Decimal, 55 знаков, косинус через ряд Тейлора.
    const double fixtures[][2] = {
        {0, 0},
        {0.1, 0.05542427005865035624039846860824735},
        {0.5, 0.6106010967612349151974036334394866},
        {1, 1.171820231084450218342384050318139},
        {1.049, 1.205629248953368399923153069217200},
        {1.05, 1.208256008864154343304961073407868},
        {1.051, 1.208789892743944699621199671950464},
        {2, 0.8767165756346780109461172460239964}
    };
    for (const auto& pair : fixtures) {
        check(lab::equal(series(pair[0]), pair[1]), "series: independent reference");
        check(lab::equal(series(-pair[0]), pair[1]), "series: even function");
    }
    check(series_limit(1.049) == 20 && series_limit(1.05) == 21, "series limit");
}
