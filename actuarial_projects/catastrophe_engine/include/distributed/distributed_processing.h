/**
 * @file distributed_processing.h
 * @brief Distributed processing framework
 *
 * Header file for distributed catastrophe modeling.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef DISTRIBUTED_PROCESSING_H
#define DISTRIBUTED_PROCESSING_H

#include "catastrophe_engine.h"

// Initialize distributed processing framework
catastrophe_error_t initialize_distributed_processing(distributed_config_t *config);

// Start worker threads
catastrophe_error_t start_worker_threads(void);

// Stop worker threads
catastrophe_error_t stop_worker_threads(void);

// Parallel catastrophe simulation
catastrophe_error_t run_parallel_simulation(const catastrophe_scenario_t *scenario,
                                          simulation_result_t *result);

// Distributed data aggregation
catastrophe_error_t aggregate_distributed_results(const simulation_result_t *local_results,
                                                size_t num_nodes,
                                                simulation_result_t *global_result);

// Cleanup distributed processing resources
void cleanup_distributed_processing(void);

#endif // DISTRIBUTED_PROCESSING_H