/**
 * @file test_seir_model.c
 * @brief Unit tests for SEIR epidemiological model
 *
 * Basic unit tests to verify SEIR model functionality.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <assert.h>

#include "seir_model.h"

/**
 * @brief Test basic SEIR model creation and destruction
 */
void test_seir_model_create_destroy() {
    printf("Testing SEIR model creation and destruction...\n");

    seir_model_t *model = seir_model_create(1000000, 5, 0.3, 0.1, 0.02);
    assert(model != NULL);

    seir_model_destroy(model);
    printf("✓ SEIR model create/destroy test passed\n");
}

/**
 * @brief Test SEIR model simulation
 */
void test_seir_model_simulation() {
    printf("Testing SEIR model simulation...\n");

    seir_model_t *model = seir_model_create(1000000, 3, 0.3, 0.1, 0.02);
    assert(model != NULL);

    // Run simulation
    seir_result_t *result = seir_model_simulate(model, 100, 1.0);
    assert(result != NULL);
    assert(result->time_steps == 100);
    assert(result->num_regions == 3);

    // Check that trajectories are not null
    assert(result->S_trajectory != NULL);
    assert(result->E_trajectory != NULL);
    assert(result->I_trajectory != NULL);
    assert(result->R_trajectory != NULL);
    assert(result->D_trajectory != NULL);
    assert(result->time_points != NULL);

    // Check that population is conserved (approximately)
    for (int t = 0; t < result->time_steps; t++) {
        double total_pop = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            int idx = t * result->num_regions + r;
            total_pop += result->S_trajectory[idx] +
                        result->E_trajectory[idx] +
                        result->I_trajectory[idx] +
                        result->R_trajectory[idx] +
                        result->D_trajectory[idx];
        }
        assert(fabs(total_pop - 1000000.0) < 1e-6);  // Allow small numerical errors
    }

    seir_result_destroy(result);
    seir_model_destroy(model);
    printf("✓ SEIR model simulation test passed\n");
}

/**
 * @brief Test R0 calculation
 */
void test_seir_r0_calculation() {
    printf("Testing R0 calculation...\n");

    seir_model_t *model = seir_model_create(1000000, 1, 0.3, 0.1, 0.02);
    assert(model != NULL);

    double r0 = seir_model_calculate_r0(model);
    assert(fabs(r0 - 3.0) < 1e-6);  // R0 = beta / gamma = 0.3 / 0.1 = 3.0

    seir_model_destroy(model);
    printf("✓ R0 calculation test passed\n");
}

/**
 * @brief Test parameter setting
 */
void test_seir_parameter_setting() {
    printf("Testing parameter setting...\n");

    seir_model_t *model = seir_model_create(1000000, 2, 0.3, 0.1, 0.02);
    assert(model != NULL);

    epidemiological_params_t params = {
        .beta = 0.4,
        .gamma = 0.15,
        .mu = 0.03,
        .sigma = 0.25,
        .intervention_strength = 0.5,
        .vaccine_efficacy = 0.8,
        .mask_effectiveness = 0.6
    };

    seir_model_set_parameters(model, &params);

    // Check R0 with interventions
    double r_effective = seir_model_calculate_r_effective(model);
    double expected_r = 0.4 / 0.15 * (1-0.5) * (1-0.8) * (1-0.6);
    assert(fabs(r_effective - expected_r) < 1e-6);

    seir_model_destroy(model);
    printf("✓ Parameter setting test passed\n");
}

/**
 * @brief Test result analysis functions
 */
void test_seir_result_analysis() {
    printf("Testing result analysis functions...\n");

    seir_model_t *model = seir_model_create(100000, 1, 0.3, 0.1, 0.02);
    assert(model != NULL);

    seir_result_t *result = seir_model_simulate(model, 200, 1.0);
    assert(result != NULL);

    // Test peak finding
    double peak_S, peak_E, peak_I, peak_R, peak_D;
    seir_result_get_peaks(result, &peak_S, &peak_E, &peak_I, &peak_R, &peak_D);

    assert(peak_I > 0);  // Should have some infections
    assert(peak_I < peak_S);  // Infections should be less than initial susceptible

    // Test final size
    double final_S, final_E, final_I, final_R, final_D;
    seir_result_get_final_size(result, &final_S, &final_E, &final_I, &final_R, &final_D);

    assert(final_S > 0);
    assert(final_R > 0);
    assert(final_D >= 0);

    seir_result_destroy(result);
    seir_model_destroy(model);
    printf("✓ Result analysis test passed\n");
}

/**
 * @brief Main test function
 */
int main(int argc, char *argv[]) {
    (void)argc;  // Suppress unused parameter warning
    (void)argv;

    printf("🧪 Running SEIR Model Unit Tests\n");
    printf("================================\n\n");

    test_seir_model_create_destroy();
    test_seir_model_simulation();
    test_seir_r0_calculation();
    test_seir_parameter_setting();
    test_seir_result_analysis();

    printf("\n✅ All SEIR model tests passed!\n");
    return 0;
}