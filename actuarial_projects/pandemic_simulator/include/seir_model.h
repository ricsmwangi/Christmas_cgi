/**
 * @file seir_model.h
 * @brief SEIR epidemiological model header
 *
 * Header file defining the SEIR model interface for pandemic simulation.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef SEIR_MODEL_H
#define SEIR_MODEL_H

#include <stdbool.h>
#include "pandemic_simulator.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief SEIR model instance
 */
typedef struct seir_model_s seir_model_t;

/**
 * @brief SEIR simulation results
 */
typedef struct {
    int time_steps;                    /**< Number of time steps */
    int num_regions;                   /**< Number of regions */

    double *S_trajectory;              /**< Susceptible population trajectory */
    double *E_trajectory;              /**< Exposed population trajectory */
    double *I_trajectory;              /**< Infected population trajectory */
    double *R_trajectory;              /**< Recovered population trajectory */
    double *D_trajectory;              /**< Deaths trajectory */
    double *time_points;               /**< Time points */
} seir_result_t;

/**
 * @brief Create SEIR model instance
 *
 * @param population_size Total population size
 * @param num_regions Number of geographic regions
 * @param beta Transmission rate
 * @param gamma Recovery rate
 * @param mu Mortality rate
 * @return SEIR model instance, NULL on failure
 */
seir_model_t *seir_model_create(int population_size, int num_regions,
                               double beta, double gamma, double mu);

/**
 * @brief Destroy SEIR model instance
 *
 * @param model Model instance to destroy
 */
void seir_model_destroy(seir_model_t *model);

/**
 * @brief Set model parameters
 *
 * @param model SEIR model instance
 * @param params Epidemiological parameters
 */
void seir_model_set_parameters(seir_model_t *model,
                              const epidemiological_params_t *params);

/**
 * @brief Set contact matrix for inter-region mixing
 *
 * @param model SEIR model instance
 * @param contact_matrix Contact matrix (num_regions x num_regions)
 */
void seir_model_set_contact_matrix(seir_model_t *model, const double *contact_matrix);

/**
 * @brief Set initial conditions
 *
 * @param model SEIR model instance
 * @param S Initial susceptible populations by region
 * @param E Initial exposed populations by region
 * @param I Initial infected populations by region
 * @param R Initial recovered populations by region
 * @param D Initial deaths by region
 */
void seir_model_set_initial_conditions(seir_model_t *model,
                                     const double *S, const double *E,
                                     const double *I, const double *R,
                                     const double *D);

/**
 * @brief Run SEIR simulation
 *
 * @param model SEIR model instance
 * @param time_steps Number of time steps to simulate
 * @param dt Time step size (days)
 * @return Simulation results, NULL on failure
 */
seir_result_t *seir_model_simulate(seir_model_t *model, int time_steps, double dt);

/**
 * @brief Run ensemble simulation with parameter uncertainty
 *
 * @param model Base SEIR model instance
 * @param num_ensemble Number of ensemble members
 * @param time_steps Number of time steps to simulate
 * @param dt Time step size (days)
 * @return Array of simulation results, NULL on failure
 */
seir_result_t **seir_model_ensemble_simulate(seir_model_t *model,
                                           int num_ensemble, int time_steps, double dt);

/**
 * @brief Clone SEIR model instance
 *
 * @param model Model instance to clone
 * @return Cloned model instance, NULL on failure
 */
seir_model_t *seir_model_clone(const seir_model_t *model);

/**
 * @brief Get current model state
 *
 * @param model SEIR model instance
 * @param S Output buffer for susceptible populations (can be NULL)
 * @param E Output buffer for exposed populations (can be NULL)
 * @param I Output buffer for infected populations (can be NULL)
 * @param R Output buffer for recovered populations (can be NULL)
 * @param D Output buffer for deaths (can be NULL)
 */
void seir_model_get_state(const seir_model_t *model,
                         double *S, double *E, double *I, double *R, double *D);

/**
 * @brief Calculate basic reproduction number (R0)
 *
 * @param model SEIR model instance
 * @return Basic reproduction number
 */
double seir_model_calculate_r0(const seir_model_t *model);

/**
 * @brief Calculate effective reproduction number (Rt)
 *
 * @param model SEIR model instance
 * @return Effective reproduction number
 */
double seir_model_calculate_r_effective(const seir_model_t *model);

/**
 * @brief SEIR result utility functions
 */

/**
 * @brief Create SEIR result structure
 *
 * @param time_steps Number of time steps
 * @param num_regions Number of regions
 * @return SEIR result structure, NULL on failure
 */
seir_result_t *seir_result_create(int time_steps, int num_regions);

/**
 * @brief Destroy SEIR result structure
 *
 * @param result Result structure to destroy
 */
void seir_result_destroy(seir_result_t *result);

/**
 * @brief Get peak values from simulation results
 *
 * @param result SEIR simulation results
 * @param peak_S Peak susceptible population (output)
 * @param peak_E Peak exposed population (output)
 * @param peak_I Peak infected population (output)
 * @param peak_R Peak recovered population (output)
 * @param peak_D Peak deaths (output)
 */
void seir_result_get_peaks(const seir_result_t *result,
                          double *peak_S, double *peak_E,
                          double *peak_I, double *peak_R, double *peak_D);

/**
 * @brief Get final epidemic size from simulation results
 *
 * @param result SEIR simulation results
 * @param final_S Final susceptible population (output)
 * @param final_E Final exposed population (output)
 * @param final_I Final infected population (output)
 * @param final_R Final recovered population (output)
 * @param final_D Final deaths (output)
 */
void seir_result_get_final_size(const seir_result_t *result,
                               double *final_S, double *final_E,
                               double *final_I, double *final_R, double *final_D);

#ifdef __cplusplus
}
#endif

#endif /* SEIR_MODEL_H */