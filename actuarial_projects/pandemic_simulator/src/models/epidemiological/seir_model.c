/**
 * @file seir_model.c
 * @brief SEIR epidemiological model implementation
 *
 * This file implements the SEIR (Susceptible-Exposed-Infected-Recovered)
 * compartment model for pandemic simulation. It supports multi-region
 * modeling, network epidemiology, and intervention effects.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include "seir_model.h"
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <stdio.h>
#include <gsl/gsl_odeiv2.h>
#include <gsl/gsl_errno.h>
#include <omp.h>

/**
 * @brief SEIR model structure
 */
struct seir_model_s {
    int population_size;              /**< Total population */
    int num_regions;                  /**< Number of regions */

    double beta;                      /**< Transmission rate */
    double gamma;                     /**< Recovery rate */
    double mu;                        /**< Mortality rate */
    double sigma;                     /**< Incubation rate */

    double intervention_strength;     /**< Intervention effectiveness */
    double vaccine_efficacy;          /**< Vaccine effectiveness */
    double mask_effectiveness;        /**< Mask effectiveness */

    double *contact_matrix;           /**< Inter-region contact patterns */
    double *region_populations;       /**< Population by region */

    // Current state
    double *S;                        /**< Susceptible by region */
    double *E;                        /**< Exposed by region */
    double *I;                        /**< Infected by region */
    double *R;                        /**< Recovered by region */
    double *D;                        /**< Deaths by region */

    // GSL ODE solver
    gsl_odeiv2_system ode_system;
    gsl_odeiv2_driver *ode_driver;

    // Random number generation
    unsigned long int rng_seed;
};

/**
 * @brief ODE function for SEIR model
 */
static int seir_ode_function(double t, const double y[], double f[], void *params) {
    (void)t;  // Suppress unused parameter warning

    seir_model_t *model = (seir_model_t *)params;

    for (int i = 0; i < model->num_regions; i++) {
        // Extract regional state
        int offset = i * 5;
        double S_i = y[offset];
        double E_i = y[offset + 1];
        double I_i = y[offset + 2];
        double R_i = y[offset + 3];
        double D_i = y[offset + 4];

        double N_i = S_i + E_i + I_i + R_i + D_i;

        // Calculate force of infection
        double lambda_i = model->beta;

        // Add inter-region transmission
        for (int j = 0; j < model->num_regions; j++) {
            if (i != j) {
                int offset_j = j * 5;
                double I_j = y[offset_j + 2];
                double N_j = y[offset_j] + y[offset_j + 1] + y[offset_j + 2] +
                           y[offset_j + 3] + y[offset_j + 4];

                if (N_j > 0) {
                    lambda_i += model->beta * model->contact_matrix[i * model->num_regions + j] *
                              (I_j / N_j);
                }
            }
        }

        // Within-region transmission
        if (N_i > 0) {
            lambda_i *= (I_i / N_i);
        }

        // Apply interventions
        lambda_i *= (1.0 - model->intervention_strength);
        lambda_i *= (1.0 - model->mask_effectiveness);
        lambda_i *= (1.0 - model->vaccine_efficacy);

        // SEIR equations
        f[offset] = -lambda_i * S_i;                                    // dS/dt
        f[offset + 1] = lambda_i * S_i - model->sigma * E_i;           // dE/dt
        f[offset + 2] = model->sigma * E_i - (model->gamma + model->mu) * I_i;  // dI/dt
        f[offset + 3] = model->gamma * I_i;                            // dR/dt
        f[offset + 4] = model->mu * I_i;                               // dD/dt
    }

    return GSL_SUCCESS;
}

/**
 * @brief Create SEIR model instance
 */
seir_model_t *seir_model_create(int population_size, int num_regions,
                               double beta, double gamma, double mu) {
    seir_model_t *model = (seir_model_t *)malloc(sizeof(seir_model_t));
    if (!model) {
        fprintf(stderr, "Failed to allocate SEIR model\n");
        return NULL;
    }

    // Initialize basic parameters
    model->population_size = population_size;
    model->num_regions = num_regions;
    model->beta = beta;
    model->gamma = gamma;
    model->mu = mu;
    model->sigma = 0.2;  // Default incubation rate (1/5 days)

    model->intervention_strength = 0.0;
    model->vaccine_efficacy = 0.0;
    model->mask_effectiveness = 0.0;

    // Allocate arrays
    model->contact_matrix = (double *)malloc(num_regions * num_regions * sizeof(double));
    model->region_populations = (double *)malloc(num_regions * sizeof(double));
    model->S = (double *)malloc(num_regions * sizeof(double));
    model->E = (double *)malloc(num_regions * sizeof(double));
    model->I = (double *)malloc(num_regions * sizeof(double));
    model->R = (double *)malloc(num_regions * sizeof(double));
    model->D = (double *)malloc(num_regions * sizeof(double));

    if (!model->contact_matrix || !model->region_populations ||
        !model->S || !model->E || !model->I || !model->R || !model->D) {
        fprintf(stderr, "Failed to allocate SEIR model arrays\n");
        seir_model_destroy(model);
        return NULL;
    }

    // Initialize contact matrix (uniform mixing by default)
    for (int i = 0; i < num_regions; i++) {
        for (int j = 0; j < num_regions; j++) {
            model->contact_matrix[i * num_regions + j] = 0.1;  // 10% inter-region mixing
        }
        model->contact_matrix[i * num_regions + i] = 0.9;  // 90% intra-region mixing
    }

    // Initialize populations (uniform by default)
    double pop_per_region = (double)population_size / num_regions;
    for (int i = 0; i < num_regions; i++) {
        model->region_populations[i] = pop_per_region;
    }

    // Initialize state (almost all susceptible, few exposed)
    for (int i = 0; i < num_regions; i++) {
        model->S[i] = model->region_populations[i] * 0.999;
        model->E[i] = model->region_populations[i] * 0.001;
        model->I[i] = 0.0;
        model->R[i] = 0.0;
        model->D[i] = 0.0;
    }

    // Set up GSL ODE system
    model->ode_system.function = seir_ode_function;
    model->ode_system.jacobian = NULL;  // Use numerical Jacobian
    model->ode_system.dimension = num_regions * 5;  // 5 compartments per region
    model->ode_system.params = model;

    // Create ODE driver
    model->ode_driver = gsl_odeiv2_driver_alloc_y_new(
        &model->ode_system, gsl_odeiv2_step_rkf45, 1e-6, 1e-6, 0.0);

    if (!model->ode_driver) {
        fprintf(stderr, "Failed to create GSL ODE driver\n");
        seir_model_destroy(model);
        return NULL;
    }

    // Initialize random seed
    model->rng_seed = time(NULL);

    return model;
}

/**
 * @brief Destroy SEIR model instance
 */
void seir_model_destroy(seir_model_t *model) {
    if (model) {
        if (model->ode_driver) {
            gsl_odeiv2_driver_free(model->ode_driver);
        }

        free(model->contact_matrix);
        free(model->region_populations);
        free(model->S);
        free(model->E);
        free(model->I);
        free(model->R);
        free(model->D);

        free(model);
    }
}

/**
 * @brief Set model parameters
 */
void seir_model_set_parameters(seir_model_t *model, const epidemiological_params_t *params) {
    if (!model || !params) return;

    model->beta = params->beta;
    model->gamma = params->gamma;
    model->mu = params->mu;
    model->sigma = params->sigma;
    model->intervention_strength = params->intervention_strength;
    model->vaccine_efficacy = params->vaccine_efficacy;
    model->mask_effectiveness = params->mask_effectiveness;
}

/**
 * @brief Set contact matrix
 */
void seir_model_set_contact_matrix(seir_model_t *model, const double *contact_matrix) {
    if (!model || !contact_matrix) return;

    memcpy(model->contact_matrix, contact_matrix,
           model->num_regions * model->num_regions * sizeof(double));
}

/**
 * @brief Set initial conditions
 */
void seir_model_set_initial_conditions(seir_model_t *model,
                                     const double *S, const double *E,
                                     const double *I, const double *R,
                                     const double *D) {
    if (!model) return;

    if (S) memcpy(model->S, S, model->num_regions * sizeof(double));
    if (E) memcpy(model->E, E, model->num_regions * sizeof(double));
    if (I) memcpy(model->I, I, model->num_regions * sizeof(double));
    if (R) memcpy(model->R, R, model->num_regions * sizeof(double));
    if (D) memcpy(model->D, D, model->num_regions * sizeof(double));
}

/**
 * @brief Run SEIR simulation
 */
seir_result_t *seir_model_simulate(seir_model_t *model, int time_steps, double dt) {
    if (!model || time_steps <= 0 || dt <= 0) {
        return NULL;
    }

    // Allocate result structure
    seir_result_t *result = (seir_result_t *)malloc(sizeof(seir_result_t));
    if (!result) {
        fprintf(stderr, "Failed to allocate SEIR result\n");
        return NULL;
    }

    result->time_steps = time_steps;
    result->num_regions = model->num_regions;

    // Allocate trajectory arrays
    size_t trajectory_size = time_steps * model->num_regions * sizeof(double);
    result->S_trajectory = (double *)malloc(trajectory_size);
    result->E_trajectory = (double *)malloc(trajectory_size);
    result->I_trajectory = (double *)malloc(trajectory_size);
    result->R_trajectory = (double *)malloc(trajectory_size);
    result->D_trajectory = (double *)malloc(trajectory_size);
    result->time_points = (double *)malloc(time_steps * sizeof(double));

    if (!result->S_trajectory || !result->E_trajectory || !result->I_trajectory ||
        !result->R_trajectory || !result->D_trajectory || !result->time_points) {
        fprintf(stderr, "Failed to allocate trajectory arrays\n");
        seir_result_destroy(result);
        return NULL;
    }

    // Initial state vector
    double *y = (double *)malloc(model->num_regions * 5 * sizeof(double));
    if (!y) {
        fprintf(stderr, "Failed to allocate state vector\n");
        seir_result_destroy(result);
        return NULL;
    }

    // Pack initial conditions
    for (int i = 0; i < model->num_regions; i++) {
        int offset = i * 5;
        y[offset] = model->S[i];
        y[offset + 1] = model->E[i];
        y[offset + 2] = model->I[i];
        y[offset + 3] = model->R[i];
        y[offset + 4] = model->D[i];
    }

    // Simulation loop
    double t = 0.0;
    for (int step = 0; step < time_steps; step++) {
        // Store current state
        for (int i = 0; i < model->num_regions; i++) {
            int offset = i * 5;
            int traj_offset = step * model->num_regions + i;

            result->S_trajectory[traj_offset] = y[offset];
            result->E_trajectory[traj_offset] = y[offset + 1];
            result->I_trajectory[traj_offset] = y[offset + 2];
            result->R_trajectory[traj_offset] = y[offset + 3];
            result->D_trajectory[traj_offset] = y[offset + 4];
        }
        result->time_points[step] = t;

        // Evolve system
        double t_next = t + dt;
        int status = gsl_odeiv2_driver_apply(model->ode_driver, &t, t_next, y);

        if (status != GSL_SUCCESS) {
            fprintf(stderr, "GSL ODE solver failed at step %d: %s\n",
                   step, gsl_strerror(status));
            free(y);
            seir_result_destroy(result);
            return NULL;
        }
    }

    free(y);
    return result;
}

/**
 * @brief Run ensemble simulation
 */
seir_result_t **seir_model_ensemble_simulate(seir_model_t *model,
                                           int num_ensemble, int time_steps, double dt) {
    if (!model || num_ensemble <= 0) {
        return NULL;
    }

    seir_result_t **results = (seir_result_t **)malloc(num_ensemble * sizeof(seir_result_t *));
    if (!results) {
        return NULL;
    }

    // Run simulations in parallel
#pragma omp parallel for
    for (int i = 0; i < num_ensemble; i++) {
        // Create a copy of the model for this ensemble member
        seir_model_t *model_copy = seir_model_clone(model);
        if (!model_copy) {
            results[i] = NULL;
            continue;
        }

        // Add parameter uncertainty
        model_copy->beta *= (0.9 + 0.2 * (rand() / (double)RAND_MAX));
        model_copy->gamma *= (0.9 + 0.2 * (rand() / (double)RAND_MAX));

        // Run simulation
        results[i] = seir_model_simulate(model_copy, time_steps, dt);

        seir_model_destroy(model_copy);
    }

    return results;
}

/**
 * @brief Clone SEIR model
 */
seir_model_t *seir_model_clone(const seir_model_t *model) {
    if (!model) return NULL;

    seir_model_t *clone = seir_model_create(
        model->population_size, model->num_regions,
        model->beta, model->gamma, model->mu);

    if (!clone) return NULL;

    // Copy all parameters and state
    clone->sigma = model->sigma;
    clone->intervention_strength = model->intervention_strength;
    clone->vaccine_efficacy = model->vaccine_efficacy;
    clone->mask_effectiveness = model->mask_effectiveness;

    memcpy(clone->contact_matrix, model->contact_matrix,
           model->num_regions * model->num_regions * sizeof(double));
    memcpy(clone->region_populations, model->region_populations,
           model->num_regions * sizeof(double));
    memcpy(clone->S, model->S, model->num_regions * sizeof(double));
    memcpy(clone->E, model->E, model->num_regions * sizeof(double));
    memcpy(clone->I, model->I, model->num_regions * sizeof(double));
    memcpy(clone->R, model->R, model->num_regions * sizeof(double));
    memcpy(clone->D, model->D, model->num_regions * sizeof(double));

    return clone;
}

/**
 * @brief Get current model state
 */
void seir_model_get_state(const seir_model_t *model,
                         double *S, double *E, double *I, double *R, double *D) {
    if (!model) return;

    if (S) memcpy(S, model->S, model->num_regions * sizeof(double));
    if (E) memcpy(E, model->E, model->num_regions * sizeof(double));
    if (I) memcpy(I, model->I, model->num_regions * sizeof(double));
    if (R) memcpy(R, model->R, model->num_regions * sizeof(double));
    if (D) memcpy(D, model->D, model->num_regions * sizeof(double));
}

/**
 * @brief Calculate basic reproduction number
 */
double seir_model_calculate_r0(const seir_model_t *model) {
    if (!model) return 0.0;

    // R0 = beta / gamma for basic SEIR model
    return model->beta / model->gamma;
}

/**
 * @brief Calculate effective reproduction number
 */
double seir_model_calculate_r_effective(const seir_model_t *model) {
    if (!model) return 0.0;

    double r0 = seir_model_calculate_r0(model);

    // Apply intervention effects
    double intervention_factor = (1.0 - model->intervention_strength) *
                               (1.0 - model->vaccine_efficacy) *
                               (1.0 - model->mask_effectiveness);

    return r0 * intervention_factor;
}

/**
 * @brief SEIR result functions
 */

/**
 * @brief Create SEIR result structure
 */
seir_result_t *seir_result_create(int time_steps, int num_regions) {
    seir_result_t *result = (seir_result_t *)malloc(sizeof(seir_result_t));
    if (!result) return NULL;

    result->time_steps = time_steps;
    result->num_regions = num_regions;

    size_t trajectory_size = time_steps * num_regions * sizeof(double);
    result->S_trajectory = (double *)malloc(trajectory_size);
    result->E_trajectory = (double *)malloc(trajectory_size);
    result->I_trajectory = (double *)malloc(trajectory_size);
    result->R_trajectory = (double *)malloc(trajectory_size);
    result->D_trajectory = (double *)malloc(trajectory_size);
    result->time_points = (double *)malloc(time_steps * sizeof(double));

    if (!result->S_trajectory || !result->E_trajectory || !result->I_trajectory ||
        !result->R_trajectory || !result->D_trajectory || !result->time_points) {
        seir_result_destroy(result);
        return NULL;
    }

    return result;
}

/**
 * @brief Destroy SEIR result structure
 */
void seir_result_destroy(seir_result_t *result) {
    if (result) {
        free(result->S_trajectory);
        free(result->E_trajectory);
        free(result->I_trajectory);
        free(result->R_trajectory);
        free(result->D_trajectory);
        free(result->time_points);
        free(result);
    }
}

/**
 * @brief Get peak infection values
 */
void seir_result_get_peaks(const seir_result_t *result,
                          double *peak_S, double *peak_E,
                          double *peak_I, double *peak_R, double *peak_D) {
    if (!result) return;

    if (peak_S) *peak_S = 0.0;
    if (peak_E) *peak_E = 0.0;
    if (peak_I) *peak_I = 0.0;
    if (peak_R) *peak_R = 0.0;
    if (peak_D) *peak_D = 0.0;

    for (int t = 0; t < result->time_steps; t++) {
        for (int r = 0; r < result->num_regions; r++) {
            int idx = t * result->num_regions + r;

            if (peak_S && result->S_trajectory[idx] > *peak_S) {
                *peak_S = result->S_trajectory[idx];
            }
            if (peak_E && result->E_trajectory[idx] > *peak_E) {
                *peak_E = result->E_trajectory[idx];
            }
            if (peak_I && result->I_trajectory[idx] > *peak_I) {
                *peak_I = result->I_trajectory[idx];
            }
            if (peak_R && result->R_trajectory[idx] > *peak_R) {
                *peak_R = result->R_trajectory[idx];
            }
            if (peak_D && result->D_trajectory[idx] > *peak_D) {
                *peak_D = result->D_trajectory[idx];
            }
        }
    }
}

/**
 * @brief Get final epidemic size
 */
void seir_result_get_final_size(const seir_result_t *result,
                               double *final_S, double *final_E,
                               double *final_I, double *final_R, double *final_D) {
    if (!result || result->time_steps == 0) return;

    int last_step = result->time_steps - 1;

    if (final_S) {
        *final_S = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            *final_S += result->S_trajectory[last_step * result->num_regions + r];
        }
    }

    if (final_E) {
        *final_E = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            *final_E += result->E_trajectory[last_step * result->num_regions + r];
        }
    }

    if (final_I) {
        *final_I = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            *final_I += result->I_trajectory[last_step * result->num_regions + r];
        }
    }

    if (final_R) {
        *final_R = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            *final_R += result->R_trajectory[last_step * result->num_regions + r];
        }
    }

    if (final_D) {
        *final_D = 0.0;
        for (int r = 0; r < result->num_regions; r++) {
            *final_D += result->D_trajectory[last_step * result->num_regions + r];
        }
    }
}