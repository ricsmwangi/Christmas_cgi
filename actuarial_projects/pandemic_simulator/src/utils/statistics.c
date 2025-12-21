/**
 * @file statistics.c
 * @brief Statistical analysis implementation
 *
 * Implementation of statistical analysis functions.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include "statistics.h"
#include <math.h>
#include <stddef.h>

double statistics_mean(const double *data, size_t n) {
    if (n == 0) return 0.0;
    double sum = 0.0;
    for (size_t i = 0; i < n; i++) {
        sum += data[i];
    }
    return sum / n;
}

double statistics_stddev(const double *data, size_t n) {
    if (n <= 1) return 0.0;
    double mean = statistics_mean(data, n);
    double sum_sq = 0.0;
    for (size_t i = 0; i < n; i++) {
        double diff = data[i] - mean;
        sum_sq += diff * diff;
    }
    return sqrt(sum_sq / (n - 1));
}

double statistics_correlation(const double *x, const double *y, size_t n) {
    if (n <= 1) return 0.0;
    double mean_x = statistics_mean(x, n);
    double mean_y = statistics_mean(y, n);
    double numerator = 0.0;
    double sum_sq_x = 0.0;
    double sum_sq_y = 0.0;

    for (size_t i = 0; i < n; i++) {
        double dx = x[i] - mean_x;
        double dy = y[i] - mean_y;
        numerator += dx * dy;
        sum_sq_x += dx * dx;
        sum_sq_y += dy * dy;
    }

    double denominator = sqrt(sum_sq_x * sum_sq_y);
    return denominator == 0.0 ? 0.0 : numerator / denominator;
}