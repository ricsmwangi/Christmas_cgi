/**
 * @file input_output_model.h
 * @brief Input-Output economic model header
 *
 * Header file for economic impact modeling.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef INPUT_OUTPUT_MODEL_H
#define INPUT_OUTPUT_MODEL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Economic sector definitions
typedef enum {
    SECTOR_AGRICULTURE,
    SECTOR_MANUFACTURING,
    SECTOR_SERVICES,
    SECTOR_HEALTHCARE,
    SECTOR_TRANSPORTATION,
    SECTOR_RETAIL,
    SECTOR_FINANCE,
    SECTOR_COUNT
} economic_sector_t;

// Input-output matrix structure
typedef struct {
    double matrix[SECTOR_COUNT][SECTOR_COUNT];  // Leontief inverse matrix
    double value_added[SECTOR_COUNT];           // Value added coefficients
    double final_demand[SECTOR_COUNT];          // Final demand vector
    size_t num_sectors;
} input_output_model_t;

// Legacy typedef for compatibility
typedef input_output_model_t input_output_model_s;

input_output_model_t *input_output_model_create(int num_sectors, int num_regions);
void input_output_model_destroy(input_output_model_t *model);

#ifdef __cplusplus
}
#endif

#endif /* INPUT_OUTPUT_MODEL_H */