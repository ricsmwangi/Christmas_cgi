/**
 * @file input_output_model.c
 * @brief Input-Output economic model implementation
 *
 * Implementation of Leontief input-output model for economic impact analysis.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include "input_output_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

// Initialize the input-output model with a basic Leontief matrix
int input_output_model_init(input_output_model_t *model) {
    if (!model) return -1;

    // For simplicity, create a basic 8-sector model
    model->num_sectors = SECTOR_COUNT;

    // Initialize Leontief inverse matrix (simplified)
    // In a real model, this would be calculated from actual input coefficients
    for (int i = 0; i < SECTOR_COUNT; i++) {
        for (int j = 0; j < SECTOR_COUNT; j++) {
            if (i == j) {
                model->matrix[i][j] = 1.5;  // Direct + indirect effects
            } else {
                model->matrix[i][j] = 0.2;  // Inter-sectoral dependencies
            }
        }
        model->value_added[i] = 0.25 + (i * 0.05);  // Varying value-added ratios
        model->final_demand[i] = 100.0 + (i * 20.0); // Base final demand
    }

    return 0;
}

// Calculate economic impact from sector shocks using Leontief model
int input_output_model_calculate_impact(input_output_model_t *model,
                                       const double *sector_shocks,
                                       double *economic_impact) {
    if (!model || !sector_shocks || !economic_impact) return -1;

    // Calculate total output change using Leontief inverse
    for (int i = 0; i < model->num_sectors; i++) {
        economic_impact[i] = 0.0;
        for (int j = 0; j < model->num_sectors; j++) {
            economic_impact[i] += model->matrix[i][j] * sector_shocks[j];
        }
        // Scale by value-added to get GDP impact
        economic_impact[i] *= model->value_added[i];
    }

    return 0;
}

void input_output_model_free(input_output_model_t *model) {
    if (model) {
        // No dynamic allocation in this simple implementation
        // In a real implementation, free any allocated matrices
    }
}

// Legacy create/destroy functions for compatibility
input_output_model_t *input_output_model_create(int num_sectors, int num_regions) {
    (void)num_regions;  // Not used in this implementation

    input_output_model_t *model = malloc(sizeof(input_output_model_t));
    if (!model) return NULL;

    if (input_output_model_init(model) != 0) {
        free(model);
        return NULL;
    }

    return model;
}

void input_output_model_destroy(input_output_model_t *model) {
    input_output_model_free(model);
    free(model);
}