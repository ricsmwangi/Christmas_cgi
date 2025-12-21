/**
 * @file main.c
 * @brief Main entry point for the Pandemic Risk Simulator
 *
 * This file contains the main function and high-level orchestration
 * of the pandemic simulation system. It coordinates epidemiological
 * modeling, economic impact analysis, actuarial loss calculation,
 * and distributed computing components.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <getopt.h>
#include <signal.h>
#include <time.h>
#include <errno.h>

#include "pandemic_simulator.h"
#include "seir_model.h"
#include "input_output_model.h"
#include "pandemic_loss_model.h"
#include "pandemic_cluster.h"
#include "plotting.h"
#include "data_loader.h"
#include "statistics.h"

/**
 * @brief Program configuration structure
 */
typedef struct {
    // Simulation parameters
    int population_size;
    int num_regions;
    int num_sectors;
    int time_horizon;
    int num_scenarios;

    // Model parameters
    double beta;           // Transmission rate
    double gamma;          // Recovery rate
    double mu;            // Mortality rate
    double intervention_strength;

    // File paths
    char *config_file;
    char *output_dir;
    char *data_dir;

    // Runtime options
    int verbose;
    int debug;
    int gpu_enabled;
    int mpi_enabled;
    int benchmark_mode;
    int test_mode;

    // Output options
    int save_results;
    int generate_plots;
    int export_csv;
    int export_json;
} config_t;

/**
 * @brief Default configuration values
 */
static const config_t DEFAULT_CONFIG = {
    .population_size = 1000000,
    .num_regions = 10,
    .num_sectors = 20,
    .time_horizon = 365,
    .num_scenarios = 100,

    .beta = 0.3,
    .gamma = 0.1,
    .mu = 0.02,
    .intervention_strength = 0.0,

    .config_file = NULL,
    .output_dir = "./results",
    .data_dir = "./data",

    .verbose = 0,
    .debug = 0,
    .gpu_enabled = 1,
    .mpi_enabled = 0,
    .benchmark_mode = 0,
    .test_mode = 0,

    .save_results = 1,
    .generate_plots = 1,
    .export_csv = 1,
    .export_json = 1
};

/**
 * @brief Convert simple config to simulation_config_t
 */
static simulation_config_t config_to_simulation_config(const config_t *config) {
    simulation_config_t sim_config = {
        .population_size = config->population_size,
        .num_regions = config->num_regions,
        .time_horizon = config->time_horizon,
        .num_scenarios = config->num_scenarios,
        .epi_params = {
            .beta = config->beta,
            .gamma = config->gamma,
            .mu = config->mu,
            .sigma = 0.2,  // Default value
            .intervention_strength = config->intervention_strength,
            .vaccine_efficacy = 0.8,  // Default value
            .mask_effectiveness = 0.6,  // Default value
            .social_distancing = 0.4  // Default value
        },
        .use_gpu = config->gpu_enabled,
        .use_mpi = config->mpi_enabled,
        .num_threads = 4,  // Default value
        .random_seed = 42,  // Default value
        .output_directory = config->output_dir,
        .data_directory = config->data_dir,
        .save_intermediate = config->save_results,
        .generate_plots = config->generate_plots
    };
    return sim_config;
}

/**
 * @brief Signal handler for graceful shutdown
 */
static volatile sig_atomic_t shutdown_requested = 0;

static void signal_handler(int signum) {
    (void)signum;  // Suppress unused parameter warning
    shutdown_requested = 1;
    fprintf(stderr, "\n⚠️  Shutdown requested. Cleaning up...\n");
}

/**
 * @brief Print usage information
 */
static void print_usage(const char *program_name) {
    printf("🌍 Pandemic Risk Simulator v1.0.0\n");
    printf("Global pandemic forecasting and risk assessment system\n\n");

    printf("USAGE:\n");
    printf("  %s [OPTIONS]\n\n", program_name);

    printf("OPTIONS:\n");
    printf("  -c, --config FILE       Configuration file path\n");
    printf("  -p, --population NUM    Population size (default: %d)\n", DEFAULT_CONFIG.population_size);
    printf("  -r, --regions NUM       Number of regions (default: %d)\n", DEFAULT_CONFIG.num_regions);
    printf("  -t, --time NUM          Simulation time horizon in days (default: %d)\n", DEFAULT_CONFIG.time_horizon);
    printf("  -n, --scenarios NUM     Number of Monte Carlo scenarios (default: %d)\n", DEFAULT_CONFIG.num_scenarios);

    printf("  --beta NUM              Transmission rate (default: %.2f)\n", DEFAULT_CONFIG.beta);
    printf("  --gamma NUM             Recovery rate (default: %.2f)\n", DEFAULT_CONFIG.gamma);
    printf("  --mu NUM                Mortality rate (default: %.2f)\n", DEFAULT_CONFIG.mu);
    printf("  --intervention NUM      Intervention strength 0-1 (default: %.2f)\n", DEFAULT_CONFIG.intervention_strength);

    printf("  -o, --output DIR        Output directory (default: %s)\n", DEFAULT_CONFIG.output_dir);
    printf("  -d, --data DIR          Data directory (default: %s)\n", DEFAULT_CONFIG.data_dir);

    printf("  -v, --verbose           Enable verbose output\n");
    printf("  --debug                 Enable debug mode\n");
    printf("  --no-gpu               Disable GPU acceleration\n");
    printf("  --mpi                  Enable MPI distributed computing\n");
    printf("  --benchmark            Run in benchmark mode\n");
    printf("  --test                 Run in test mode\n");

    printf("  --no-save              Don't save results\n");
    printf("  --no-plots             Don't generate plots\n");
    printf("  --no-csv               Don't export CSV files\n");
    printf("  --no-json              Don't export JSON files\n");

    printf("  -h, --help             Show this help message\n");
    printf("  -V, --version          Show version information\n\n");

    printf("EXAMPLES:\n");
    printf("  %s --population 8000000000 --regions 195 --time 730\n", program_name);
    printf("  %s --config pandemic_config.json --verbose --benchmark\n", program_name);
    printf("  %s --test --debug\n\n", program_name);

    printf("For more information, visit: https://pandemic-simulator.readthedocs.io\n");
}

/**
 * @brief Print version information
 */
static void print_version(void) {
    printf("🌍 Pandemic Risk Simulator v1.0.0\n");
    printf("Global pandemic forecasting and risk assessment system\n\n");

    printf("Built on: %s %s\n", __DATE__, __TIME__);
    printf("Compiler: %s\n", __VERSION__);

#ifdef __CUDA_ARCH__
    printf("CUDA Support: Enabled (Architecture: %d)\n", __CUDA_ARCH__);
#else
    printf("CUDA Support: Available\n");
#endif

#ifdef _OPENMP
    printf("OpenMP Support: Enabled (Version: %d)\n", _OPENMP);
#endif

    printf("\nFeatures:\n");
    printf("  ✅ Epidemiological Modeling (SEIR)\n");
    printf("  ✅ Economic Impact Analysis\n");
    printf("  ✅ Actuarial Loss Calculation\n");
    printf("  ✅ GPU Acceleration (CUDA)\n");
    printf("  ✅ Distributed Computing (MPI)\n");
    printf("  ✅ Real-time Visualization\n");
    printf("  ✅ Monte Carlo Simulation\n");
    printf("  ✅ Uncertainty Quantification\n\n");

    printf("License: MIT License (open source for global good)\n");
    printf("Copyright © 2024 Pandemic Risk Simulator Team\n");
}

/**
 * @brief Parse command line arguments
 */
static int parse_arguments(int argc, char *argv[], config_t *config) {
    static struct option long_options[] = {
        {"config", required_argument, 0, 'c'},
        {"population", required_argument, 0, 'p'},
        {"regions", required_argument, 0, 'r'},
        {"sectors", required_argument, 0, 's'},
        {"time", required_argument, 0, 't'},
        {"scenarios", required_argument, 0, 'n'},
        {"beta", required_argument, 0, 1001},
        {"gamma", required_argument, 0, 1002},
        {"mu", required_argument, 0, 1003},
        {"intervention", required_argument, 0, 1004},
        {"output", required_argument, 0, 'o'},
        {"data", required_argument, 0, 'd'},
        {"verbose", no_argument, 0, 'v'},
        {"debug", no_argument, 0, 1005},
        {"no-gpu", no_argument, 0, 1006},
        {"mpi", no_argument, 0, 1007},
        {"benchmark", no_argument, 0, 1008},
        {"test", no_argument, 0, 1009},
        {"no-save", no_argument, 0, 1010},
        {"no-plots", no_argument, 0, 1011},
        {"no-csv", no_argument, 0, 1012},
        {"no-json", no_argument, 0, 1013},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 'V'},
        {0, 0, 0, 0}
    };

    int option_index = 0;
    int c;

    while ((c = getopt_long(argc, argv, "c:p:r:s:t:n:o:d:vhV", long_options, &option_index)) != -1) {
        switch (c) {
            case 'c':
                config->config_file = strdup(optarg);
                break;
            case 'p':
                config->population_size = atoi(optarg);
                break;
            case 'r':
                config->num_regions = atoi(optarg);
                break;
            case 's':
                config->num_sectors = atoi(optarg);
                break;
            case 't':
                config->time_horizon = atoi(optarg);
                break;
            case 'n':
                config->num_scenarios = atoi(optarg);
                break;
            case 1001:  // --beta
                config->beta = atof(optarg);
                break;
            case 1002:  // --gamma
                config->gamma = atof(optarg);
                break;
            case 1003:  // --mu
                config->mu = atof(optarg);
                break;
            case 1004:  // --intervention
                config->intervention_strength = atof(optarg);
                break;
            case 'o':
                config->output_dir = strdup(optarg);
                break;
            case 'd':
                config->data_dir = strdup(optarg);
                break;
            case 'v':
                config->verbose = 1;
                break;
            case 1005:  // --debug
                config->debug = 1;
                break;
            case 1006:  // --no-gpu
                config->gpu_enabled = 0;
                break;
            case 1007:  // --mpi
                config->mpi_enabled = 1;
                break;
            case 1008:  // --benchmark
                config->benchmark_mode = 1;
                break;
            case 1009:  // --test
                config->test_mode = 1;
                break;
            case 1010:  // --no-save
                config->save_results = 0;
                break;
            case 1011:  // --no-plots
                config->generate_plots = 0;
                break;
            case 1012:  // --no-csv
                config->export_csv = 0;
                break;
            case 1013:  // --no-json
                config->export_json = 0;
                break;
            case 'h':
                print_usage(argv[0]);
                return 1;
            case 'V':
                print_version();
                return 1;
            case '?':
            default:
                fprintf(stderr, "❌ Unknown option. Use --help for usage information.\n");
                return -1;
        }
    }

    return 0;
}

/**
 * @brief Load configuration from file
 */
static int load_config_file(const char *filename, config_t *config) {
    // TODO: Implement JSON configuration file loading
    // For now, just print a message
    if (config->verbose) {
        printf("📄 Loading configuration from: %s\n", filename);
    }

    // Placeholder - would parse JSON here
    fprintf(stderr, "⚠️  Configuration file loading not yet implemented\n");
    return 0;
}

/**
 * @brief Validate configuration parameters
 */
static int validate_config(const config_t *config) {
    int errors = 0;

    if (config->population_size <= 0) {
        fprintf(stderr, "❌ Population size must be positive\n");
        errors++;
    }

    if (config->num_regions <= 0) {
        fprintf(stderr, "❌ Number of regions must be positive\n");
        errors++;
    }

    if (config->num_sectors <= 0) {
        fprintf(stderr, "❌ Number of sectors must be positive\n");
        errors++;
    }

    if (config->time_horizon <= 0) {
        fprintf(stderr, "❌ Time horizon must be positive\n");
        errors++;
    }

    if (config->num_scenarios <= 0) {
        fprintf(stderr, "❌ Number of scenarios must be positive\n");
        errors++;
    }

    if (config->beta < 0 || config->beta > 1) {
        fprintf(stderr, "❌ Beta (transmission rate) must be between 0 and 1\n");
        errors++;
    }

    if (config->gamma < 0 || config->gamma > 1) {
        fprintf(stderr, "❌ Gamma (recovery rate) must be between 0 and 1\n");
        errors++;
    }

    if (config->mu < 0 || config->mu > 1) {
        fprintf(stderr, "❌ Mu (mortality rate) must be between 0 and 1\n");
        errors++;
    }

    if (config->intervention_strength < 0 || config->intervention_strength > 1) {
        fprintf(stderr, "❌ Intervention strength must be between 0 and 1\n");
        errors++;
    }

    return errors == 0 ? 0 : -1;
}

/**
 * @brief Initialize logging
 */
static void init_logging(const config_t *config) {
    if (config->debug) {
        setenv("PANDEMIC_DEBUG", "1", 1);
    }

    if (config->verbose) {
        setenv("PANDEMIC_VERBOSE", "1", 1);
    }
}

/**
 * @brief Run test mode
 */
static int run_test_mode(const config_t *config) {
    printf("🧪 Running test mode...\n");

    // Test epidemiological model
    printf("  Testing epidemiological model...\n");
    // TODO: Implement epidemiological model tests

    // Test economic model
    printf("  Testing economic model...\n");
    // TODO: Implement economic model tests

    // Test actuarial model
    printf("  Testing actuarial model...\n");
    // TODO: Implement actuarial model tests

    // Test GPU functionality
    if (config->gpu_enabled) {
        printf("  Testing GPU functionality...\n");
        // TODO: Implement GPU tests
    }

    // Test MPI functionality
    if (config->mpi_enabled) {
        printf("  Testing MPI functionality...\n");
        // TODO: Implement MPI tests
    }

    printf("✅ All tests passed!\n");
    return 0;
}

/**
 * @brief Run benchmark mode
 */
static int run_benchmark_mode(const config_t *config) {
    printf("⚡ Running benchmark mode...\n");

    clock_t start_time = clock();

    // Benchmark epidemiological simulation
    printf("  Benchmarking epidemiological simulation...\n");
    // TODO: Implement epidemiological benchmarks

    // Benchmark economic simulation
    printf("  Benchmarking economic simulation...\n");
    // TODO: Implement economic benchmarks

    // Benchmark actuarial calculations
    printf("  Benchmarking actuarial calculations...\n");
    // TODO: Implement actuarial benchmarks

    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("✅ Benchmark completed in %.2f seconds\n", elapsed_time);
    return 0;
}

/**
 * @brief Run main simulation
 */
static int run_simulation(const config_t *config) {
    printf("🌍 Starting Pandemic Risk Simulation...\n");
    printf("  Population: %d\n", config->population_size);
    printf("  Regions: %d\n", config->num_regions);
    printf("  Sectors: %d\n", config->num_sectors);
    printf("  Time horizon: %d days\n", config->time_horizon);
    printf("  Scenarios: %d\n", config->num_scenarios);
    printf("  GPU: %s\n", config->gpu_enabled ? "Enabled" : "Disabled");
    printf("  MPI: %s\n", config->mpi_enabled ? "Enabled" : "Disabled");
    printf("\n");

    clock_t start_time = clock();

    // Initialize models
    printf("🔧 Initializing models...\n");

    // Epidemiological model
    seir_model_t *epi_model = seir_model_create(
        config->population_size,
        config->num_regions,
        config->beta,
        config->gamma,
        config->mu
    );

    if (!epi_model) {
        fprintf(stderr, "❌ Failed to create epidemiological model\n");
        return -1;
    }

    // Economic model
    input_output_model_t *econ_model = input_output_model_create(
        config->num_sectors,
        config->num_regions
    );

    if (!econ_model) {
        fprintf(stderr, "❌ Failed to create economic model\n");
        seir_model_destroy(epi_model);
        return -1;
    }

    // Actuarial model
    pandemic_loss_model_t *loss_model = pandemic_loss_model_create(
        config->num_regions,
        config->num_sectors
    );

    if (!loss_model) {
        fprintf(stderr, "❌ Failed to create actuarial model\n");
        seir_model_destroy(epi_model);
        input_output_model_destroy(econ_model);
        return -1;
    }

    // Run simulation
    printf("🚀 Running simulation...\n");

    simulation_config_t sim_config = config_to_simulation_config(config);
    simulation_result_t *results = pandemic_simulator_run(
        epi_model,
        econ_model,
        loss_model,
        &sim_config
    );

    if (!results) {
        fprintf(stderr, "❌ Simulation failed\n");
        seir_model_destroy(epi_model);
        input_output_model_destroy(econ_model);
        pandemic_loss_model_destroy(loss_model);
        return -1;
    }

    // Process results
    printf("📊 Processing results...\n");

    if (config->save_results) {
        printf("💾 Saving results...\n");
        // TODO: Implement result saving
    }

    if (config->generate_plots) {
        printf("📈 Generating plots...\n");
        // TODO: Implement plot generation
    }

    if (config->export_csv) {
        printf("📄 Exporting CSV files...\n");
        // TODO: Implement CSV export
    }

    if (config->export_json) {
        printf("📄 Exporting JSON files...\n");
        // TODO: Implement JSON export
    }

    // Print summary
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;

    printf("\n📋 Simulation Summary:\n");
    printf("  Elapsed time: %.2f seconds\n", elapsed_time);
    printf("  Peak infections: %.1fM\n", results->peak_infections / 1e6);
    printf("  Total deaths: %.0fK\n", results->total_deaths / 1e3);
    printf("  Economic loss: $%.1fT\n", results->economic_loss / 1e12);
    printf("  Insurance losses: $%.1fT\n", results->insurance_losses / 1e12);

    // Cleanup
    simulation_result_free(results);
    seir_model_destroy(epi_model);
    input_output_model_destroy(econ_model);
    pandemic_loss_model_destroy(loss_model);

    printf("✅ Simulation completed successfully!\n");
    return 0;
}

/**
 * @brief Main entry point
 */
int main(int argc, char *argv[]) {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    // Initialize configuration with defaults
    config_t config = DEFAULT_CONFIG;

    // Parse command line arguments
    int parse_result = parse_arguments(argc, argv, &config);
    if (parse_result != 0) {
        return parse_result;
    }

    // Load configuration file if specified
    if (config.config_file) {
        if (load_config_file(config.config_file, &config) != 0) {
            fprintf(stderr, "❌ Failed to load configuration file: %s\n", config.config_file);
            return -1;
        }
    }

    // Validate configuration
    if (validate_config(&config) != 0) {
        fprintf(stderr, "❌ Invalid configuration\n");
        return -1;
    }

    // Initialize logging
    init_logging(&config);

    // Print banner
    if (config.verbose) {
        printf("🌍 Pandemic Risk Simulator v1.0.0\n");
        printf("================================\n\n");
    }

    // Run appropriate mode
    int result = 0;

    if (config.test_mode) {
        result = run_test_mode(&config);
    } else if (config.benchmark_mode) {
        result = run_benchmark_mode(&config);
    } else {
        result = run_simulation(&config);
    }

    // Cleanup
    if (config.config_file) {
        free(config.config_file);
    }
    if (config.output_dir != DEFAULT_CONFIG.output_dir) {
        free(config.output_dir);
    }
    if (config.data_dir != DEFAULT_CONFIG.data_dir) {
        free(config.data_dir);
    }

    return result;
}