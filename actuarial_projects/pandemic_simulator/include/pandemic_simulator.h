/**
 * @file pandemic_simulator.h
 * @brief Main header file for the Pandemic Risk Simulator
 *
 * This header defines the core interfaces, data structures, and function
 * declarations for the pandemic simulation system. It provides the main
 * API for running comprehensive pandemic risk assessments.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef PANDEMIC_SIMULATOR_H
#define PANDEMIC_SIMULATOR_H

#include <stdint.h>
#include <stdbool.h>
#include <time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Version information
 */
#define PANDEMIC_SIMULATOR_VERSION_MAJOR 1
#define PANDEMIC_SIMULATOR_VERSION_MINOR 0
#define PANDEMIC_SIMULATOR_VERSION_PATCH 0
#define PANDEMIC_SIMULATOR_VERSION_STRING "1.0.0"

/**
 * @brief Maximum dimensions for arrays
 */
#define MAX_REGIONS 1000
#define MAX_SECTORS 200
#define MAX_TIME_STEPS 10000
#define MAX_SCENARIOS 100000

/**
 * @brief Epidemiological model parameters
 */
typedef struct {
    double beta;                    /**< Transmission rate */
    double gamma;                   /**< Recovery rate */
    double mu;                      /**< Mortality rate */
    double sigma;                   /**< Incubation rate */
    double intervention_strength;   /**< Intervention effectiveness (0-1) */
    double vaccine_efficacy;        /**< Vaccine effectiveness (0-1) */
    double mask_effectiveness;      /**< Mask effectiveness (0-1) */
    double social_distancing;       /**< Social distancing effectiveness (0-1) */
} epidemiological_params_t;

/**
 * @brief Economic model parameters
 */
typedef struct {
    double *sector_vulnerabilities;     /**< Vulnerability by sector */
    double *inter_sectoral_effects;     /**< Inter-sectoral dependencies */
    double *labor_intensities;          /**< Labor intensity by sector */
    double *value_added;                /**< Value added by sector */
    double fiscal_stimulus;             /**< Government fiscal stimulus */
    double monetary_easing;             /**< Monetary policy easing */
    int num_sectors;                    /**< Number of economic sectors */
} economic_params_t;

/**
 * @brief Actuarial model parameters
 */
typedef struct {
    double mortality_multiplier;        /**< Pandemic mortality multiplier */
    double morbidity_multiplier;        /**< Pandemic morbidity multiplier */
    double business_interruption_rate;  /**< Business interruption rate */
    double reinsurance_retention;       /**< Reinsurance retention level */
    double catastrophe_fund_size;       /**< Catastrophe fund size */
    double regulatory_capital;          /**< Required regulatory capital */
} actuarial_params_t;

/**
 * @brief Simulation configuration
 */
typedef struct {
    int population_size;                /**< Total population */
    int num_regions;                    /**< Number of geographic regions */
    int time_horizon;                   /**< Simulation time horizon (days) */
    int num_scenarios;                  /**< Number of Monte Carlo scenarios */

    epidemiological_params_t epi_params;   /**< Epidemiological parameters */
    economic_params_t econ_params;         /**< Economic parameters */
    actuarial_params_t actuarial_params;   /**< Actuarial parameters */

    bool use_gpu;                      /**< Enable GPU acceleration */
    bool use_mpi;                      /**< Enable MPI distributed computing */
    int num_threads;                   /**< Number of CPU threads */
    int random_seed;                   /**< Random number seed */

    char *output_directory;            /**< Output directory path */
    char *data_directory;              /**< Data directory path */
    bool save_intermediate;            /**< Save intermediate results */
    bool generate_plots;               /**< Generate visualization plots */
} simulation_config_t;

/**
 * @brief Epidemiological state at a point in time
 */
typedef struct {
    double *susceptible;               /**< Susceptible population by region */
    double *exposed;                   /**< Exposed population by region */
    double *infected;                  /**< Infected population by region */
    double *recovered;                 /**< Recovered population by region */
    double *deaths;                    /**< Deaths by region */
    double *r_effective;               /**< Effective reproduction number by region */
    int num_regions;                   /**< Number of regions */
} epidemiological_state_t;

/**
 * @brief Economic state at a point in time
 */
typedef struct {
    double *gdp_by_sector;             /**< GDP by sector */
    double *employment_by_sector;      /**< Employment by sector */
    double *supply_chain_disruption;   /**< Supply chain disruption by sector */
    double *consumer_spending;         /**< Consumer spending by sector */
    double total_gdp;                  /**< Total GDP */
    double unemployment_rate;          /**< Overall unemployment rate */
    int num_sectors;                   /**< Number of sectors */
} economic_state_t;

/**
 * @brief Insurance loss breakdown
 */
typedef struct {
    double mortality_losses;           /**< Losses from increased mortality */
    double morbidity_losses;           /**< Losses from increased morbidity */
    double business_interruption;      /**< Business interruption losses */
    double event_cancellation;         /**< Event cancellation losses */
    double total_losses;               /**< Total insurance losses */
    double *losses_by_line;            /**< Losses by line of business */
    double *losses_by_region;          /**< Losses by region */
    int num_lines;                     /**< Number of lines of business */
    int num_regions;                   /**< Number of regions */
} insurance_losses_t;

/**
 * @brief Risk metrics for a simulation scenario
 */
typedef struct {
    double value_at_risk_95;           /**< 95% Value at Risk */
    double value_at_risk_99;           /**< 99% Value at Risk */
    double expected_shortfall_95;      /**< 95% Expected Shortfall */
    double expected_shortfall_99;      /**< 99% Expected Shortfall */
    double maximum_loss;               /**< Maximum possible loss */
    double expected_loss;              /**< Expected loss */
    double loss_volatility;            /**< Loss volatility (standard deviation) */
} risk_metrics_t;

/**
 * @brief Complete simulation results
 */
typedef struct {
    // Metadata
    time_t simulation_start_time;      /**< Simulation start timestamp */
    time_t simulation_end_time;        /**< Simulation end timestamp */
    double execution_time;             /**< Total execution time (seconds) */
    int num_scenarios_completed;       /**< Number of scenarios completed */

    // Epidemiological results
    epidemiological_state_t *epi_trajectory;  /**< Time series of epi states */
    double peak_infections;            /**< Peak infection count */
    double total_deaths;               /**< Total deaths across all scenarios */
    double infection_duration;         /**< Average infection duration */

    // Economic results
    economic_state_t *econ_trajectory; /**< Time series of economic states */
    double economic_loss;              /**< Total economic loss */
    double gdp_impact;                 /**< GDP impact percentage */
    double recovery_time;              /**< Economic recovery time (months) */

    // Insurance results
    insurance_losses_t *loss_breakdown; /**< Insurance loss breakdown */
    double insurance_losses;           /**< Total insurance losses */
    risk_metrics_t risk_metrics;       /**< Risk metrics */

    // Uncertainty quantification
    double *confidence_intervals;      /**< Confidence intervals for key metrics */
    double *sensitivity_analysis;      /**< Parameter sensitivity analysis */

    // Performance metrics
    double memory_usage;               /**< Peak memory usage (MB) */
    double cpu_utilization;            /**< Average CPU utilization (%) */
    double gpu_utilization;            /**< Average GPU utilization (%) */
} simulation_result_t;

/**
 * @brief Intervention strategy
 */
typedef struct {
    char *name;                        /**< Intervention name */
    double *effectiveness;             /**< Effectiveness over time */
    double *cost;                      /**< Cost over time */
    double *timeline;                  /**< Implementation timeline */
    int duration;                      /**< Intervention duration (days) */
} intervention_t;

/**
 * @brief Optimization results for intervention strategies
 */
typedef struct {
    intervention_t *optimal_strategy;  /**< Optimal intervention mix */
    double expected_deaths;            /**< Expected deaths with optimal strategy */
    double expected_economic_loss;     /**< Expected economic loss */
    double strategy_cost;              /**< Total strategy cost */
    double cost_effectiveness;         /**< Cost-effectiveness ratio */
} optimization_result_t;

/**
 * @brief Real-time monitoring data
 */
typedef struct {
    epidemiological_state_t current_state;    /**< Current epidemiological state */
    economic_state_t current_economy;         /**< Current economic state */
    insurance_losses_t current_losses;        /**< Current insurance losses */
    time_t last_update;               /**< Last data update timestamp */
    double data_quality_score;        /**< Data quality score (0-1) */
} realtime_data_t;

/**
 * @brief Early warning indicators
 */
typedef struct {
    double outbreak_probability;       /**< Probability of major outbreak */
    double severity_index;             /**< Outbreak severity index */
    char **high_risk_regions;          /**< High-risk regions */
    char **warning_messages;           /**< Warning messages */
    time_t warning_timestamp;          /**< Warning timestamp */
    int num_warnings;                  /**< Number of warnings */
} early_warning_t;

/**
 * @brief API Functions
 */

/**
 * @brief Initialize the pandemic simulator
 *
 * @param config Simulation configuration
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_init(const simulation_config_t *config);

/**
 * @brief Run a complete pandemic simulation
 *
 * @param epi_model Epidemiological model instance
 * @param econ_model Economic model instance
 * @param loss_model Actuarial loss model instance
 * @param config Simulation configuration
 * @return Simulation results, NULL on failure
 */
simulation_result_t *pandemic_simulator_run(
    void *epi_model,
    void *econ_model,
    void *loss_model,
    const simulation_config_t *config
);

/**
 * @brief Run Monte Carlo uncertainty analysis
 *
 * @param config Base simulation configuration
 * @param num_samples Number of Monte Carlo samples
 * @return Uncertainty analysis results, NULL on failure
 */
simulation_result_t *pandemic_simulator_monte_carlo(
    const simulation_config_t *config,
    int num_samples
);

/**
 * @brief Optimize intervention strategies
 *
 * @param config Simulation configuration
 * @param interventions Array of available interventions
 * @param num_interventions Number of interventions
 * @param objectives Optimization objectives
 * @return Optimization results, NULL on failure
 */
optimization_result_t *pandemic_simulator_optimize_interventions(
    const simulation_config_t *config,
    const intervention_t *interventions,
    int num_interventions,
    const char **objectives
);

/**
 * @brief Run real-time pandemic monitoring
 *
 * @param config Simulation configuration
 * @return Real-time monitoring results, NULL on failure
 */
realtime_data_t *pandemic_simulator_realtime_monitor(
    const simulation_config_t *config
);

/**
 * @brief Generate early warnings
 *
 * @param current_data Current monitoring data
 * @param historical_data Historical simulation data
 * @return Early warning indicators, NULL on failure
 */
early_warning_t *pandemic_simulator_early_warnings(
    const realtime_data_t *current_data,
    const simulation_result_t *historical_data
);

/**
 * @brief Save simulation results to file
 *
 * @param results Simulation results
 * @param filename Output filename
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_save_results(
    const simulation_result_t *results,
    const char *filename
);

/**
 * @brief Load simulation results from file
 *
 * @param filename Input filename
 * @return Loaded simulation results, NULL on failure
 */
simulation_result_t *pandemic_simulator_load_results(
    const char *filename
);

/**
 * @brief Generate visualization plots
 *
 * @param results Simulation results
 * @param output_dir Output directory for plots
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_generate_plots(
    const simulation_result_t *results,
    const char *output_dir
);

/**
 * @brief Export results to CSV format
 *
 * @param results Simulation results
 * @param filename Output CSV filename
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_export_csv(
    const simulation_result_t *results,
    const char *filename
);

/**
 * @brief Export results to JSON format
 *
 * @param results Simulation results
 * @param filename Output JSON filename
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_export_json(
    const simulation_result_t *results,
    const char *filename
);

/**
 * @brief Clean up and shutdown the simulator
 */
void pandemic_simulator_shutdown(void);

/**
 * @brief Memory management functions
 */

/**
 * @brief Allocate epidemiological state
 *
 * @param num_regions Number of regions
 * @return Allocated epidemiological state, NULL on failure
 */
epidemiological_state_t *epidemiological_state_alloc(int num_regions);

/**
 * @brief Free epidemiological state
 *
 * @param state State to free
 */
void epidemiological_state_free(epidemiological_state_t *state);

/**
 * @brief Allocate economic state
 *
 * @param num_sectors Number of sectors
 * @return Allocated economic state, NULL on failure
 */
economic_state_t *economic_state_alloc(int num_sectors);

/**
 * @brief Free economic state
 *
 * @param state State to free
 */
void economic_state_free(economic_state_t *state);

/**
 * @brief Allocate simulation results
 *
 * @param time_horizon Time horizon
 * @param num_scenarios Number of scenarios
 * @return Allocated simulation results, NULL on failure
 */
simulation_result_t *simulation_result_alloc(int time_horizon, int num_scenarios);

/**
 * @brief Free simulation results
 *
 * @param results Results to free
 */
void simulation_result_free(simulation_result_t *results);

/**
 * @brief Utility functions
 */

/**
 * @brief Get version string
 *
 * @return Version string
 */
const char *pandemic_simulator_version(void);

/**
 * @brief Get build information
 *
 * @return Build information string
 */
const char *pandemic_simulator_build_info(void);

/**
 * @brief Validate simulation configuration
 *
 * @param config Configuration to validate
 * @return 0 if valid, -1 if invalid
 */
int pandemic_simulator_validate_config(const simulation_config_t *config);

/**
 * @brief Set random seed for reproducible results
 *
 * @param seed Random seed
 */
void pandemic_simulator_set_seed(uint32_t seed);

/**
 * @brief Get last error message
 *
 * @return Error message string
 */
const char *pandemic_simulator_get_error(void);

/**
 * @brief Enable/disable verbose logging
 *
 * @param enable Enable verbose logging
 */
void pandemic_simulator_set_verbose(bool enable);

/**
 * @brief Performance monitoring functions
 */

/**
 * @brief Start performance profiling
 */
void pandemic_simulator_start_profiling(void);

/**
 * @brief Stop performance profiling
 */
void pandemic_simulator_stop_profiling(void);

/**
 * @brief Get performance statistics
 *
 * @return Performance statistics string
 */
const char *pandemic_simulator_get_performance_stats(void);

/**
 * @brief Benchmarking functions
 */

/**
 * @brief Run performance benchmark
 *
 * @param config Benchmark configuration
 * @return Benchmark results, NULL on failure
 */
simulation_result_t *pandemic_simulator_benchmark(
    const simulation_config_t *config
);

/**
 * @brief Compare performance across configurations
 *
 * @param configs Array of configurations to compare
 * @param num_configs Number of configurations
 * @return Performance comparison results
 */
const char *pandemic_simulator_compare_performance(
    const simulation_config_t *configs,
    int num_configs
);

/**
 * @brief GPU management functions (when CUDA is available)
 */

#ifdef __CUDACC__
/**
 * @brief Initialize CUDA device
 *
 * @param device_id CUDA device ID
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_cuda_init(int device_id);

/**
 * @brief Get CUDA device properties
 *
 * @return CUDA device properties string
 */
const char *pandemic_simulator_cuda_get_device_info(void);

/**
 * @brief Synchronize CUDA device
 */
void pandemic_simulator_cuda_synchronize(void);

/**
 * @brief Get CUDA memory usage
 *
 * @return Memory usage in MB
 */
double pandemic_simulator_cuda_get_memory_usage(void);
#endif

/**
 * @brief MPI management functions (when MPI is available)
 */

#ifdef USE_MPI
/**
 * @brief Initialize MPI
 *
 * @param argc Command line argument count
 * @param argv Command line arguments
 * @return 0 on success, -1 on failure
 */
int pandemic_simulator_mpi_init(int *argc, char ***argv);

/**
 * @brief Finalize MPI
 */
void pandemic_simulator_mpi_finalize(void);

/**
 * @brief Get MPI rank
 *
 * @return MPI rank
 */
int pandemic_simulator_mpi_get_rank(void);

/**
 * @brief Get MPI size
 *
 * @return MPI size (number of processes)
 */
int pandemic_simulator_mpi_get_size(void);

/**
 * @brief Synchronize MPI processes
 */
void pandemic_simulator_mpi_barrier(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* PANDEMIC_SIMULATOR_H */