/**
 * @file data_loader.h
 * @brief Data loading utilities header
 *
 * Header file for data loading and file I/O utilities.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef DATA_LOADER_H
#define DATA_LOADER_H

int data_loader_load_population_data(const char *filename);
int data_loader_load_economic_data(const char *filename);

#endif /* DATA_LOADER_H */