/**
 * @file statistics.h
 * @brief Statistical analysis header
 *
 * Header file for statistical analysis functions.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef STATISTICS_H
#define STATISTICS_H

#include <stddef.h>

double statistics_mean(const double *data, size_t n);
double statistics_stddev(const double *data, size_t n);
double statistics_correlation(const double *x, const double *y, size_t n);

#endif /* STATISTICS_H */