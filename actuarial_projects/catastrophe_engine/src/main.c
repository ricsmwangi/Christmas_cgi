#include "catastrophe_engine.h"
#include <getopt.h>
#include <signal.h>
#include <unistd.h>

// Global variables
static volatile bool running = true;
static engine_config_t global_config;

// Signal handler for graceful shutdown
static void signal_handler(int signum) {
    (void)signum;  // Suppress unused parameter warning
    catastrophe_log_info("Received signal %d, initiating shutdown...", signum);
    running = false;
}

// Print usage information
static void print_usage(const char *program_name) {
    printf("Catastrophe Engine v%s - Real-Time Catastrophe Risk Assessment\n\n", CATASTROPHE_ENGINE_VERSION);
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -c, --config FILE       Configuration file (default: config/engine.json)\n");
    printf("  -p, --port PORT         Server port (default: %d)\n", DEFAULT_PORT);
    printf("  -d, --data-dir DIR      Data directory (default: ./data)\n");
    printf("  -l, --log-file FILE     Log file (default: ./logs/engine.log)\n");
    printf("  -t, --test-mode         Run in test mode with sample data\n");
    printf("  -b, --benchmark         Run performance benchmarks\n");
    printf("  -v, --verbose           Enable verbose logging\n");
    printf("  -h, --help              Show this help message\n");
    printf("  --version               Show version information\n\n");
    printf("Examples:\n");
    printf("  %s --test-mode --verbose\n", program_name);
    printf("  %s -c /etc/catastrophe/engine.json -p 9090\n", program_name);
    printf("  %s --benchmark --data-dir /mnt/fast_storage\n\n", program_name);
}

// Print version information
static void print_version(void) {
    printf("Catastrophe Engine v%s\n", CATASTROPHE_ENGINE_VERSION);
    printf("Build date: %s %s\n", CATASTROPHE_ENGINE_BUILD_DATE, CATASTROPHE_ENGINE_BUILD_TIME);
    printf("Copyright (c) 2024 Catastrophe Risk Technologies\n");
}

// Parse command line arguments
static catastrophe_error_t parse_arguments(int argc, char *argv[], engine_config_t *config) {
    static struct option long_options[] = {
        {"config", required_argument, 0, 'c'},
        {"port", required_argument, 0, 'p'},
        {"data-dir", required_argument, 0, 'd'},
        {"log-file", required_argument, 0, 'l'},
        {"test-mode", no_argument, 0, 't'},
        {"benchmark", no_argument, 0, 'b'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {"version", no_argument, 0, 0},
        {0, 0, 0, 0}
    };

    // Set defaults
    strcpy(config->config_file, "config/engine.json");
    config->port = DEFAULT_PORT;
    strcpy(config->data_directory, "./data");
    strcpy(config->log_file, "./logs/engine.log");
    config->enable_clustering = false;
    config->max_connections = MAX_CONCURRENT_REQUESTS;
    config->worker_threads = WORKER_THREAD_POOL_SIZE;
    config->buffer_size = DEFAULT_BUFFER_SIZE;
    config->enable_ssl = false;
    config->enable_metrics = true;
    config->metrics_port = 9090;

    bool test_mode = false;
    bool benchmark_mode = false;
    bool verbose = false;

    int opt;
    int option_index = 0;

    while ((opt = getopt_long(argc, argv, "c:p:d:l:tbvh", long_options, &option_index)) != -1) {
        switch (opt) {
            case 'c':
                strncpy(config->config_file, optarg, sizeof(config->config_file) - 1);
                break;
            case 'p':
                config->port = (uint16_t)atoi(optarg);
                break;
            case 'd':
                strncpy(config->data_directory, optarg, sizeof(config->data_directory) - 1);
                break;
            case 'l':
                strncpy(config->log_file, optarg, sizeof(config->log_file) - 1);
                break;
            case 't':
                test_mode = true;
                break;
            case 'b':
                benchmark_mode = true;
                break;
            case 'v':
                verbose = true;
                break;
            case 'h':
                print_usage(argv[0]);
                exit(0);
            case 0:  // Long option without short equivalent
                if (strcmp(long_options[option_index].name, "version") == 0) {
                    print_version();
                    exit(0);
                }
                break;
            default:
                fprintf(stderr, "Unknown option. Use --help for usage information.\n");
                return CE_ERROR_INVALID_INPUT;
        }
    }

    // Handle mode flags
    if (test_mode) {
        catastrophe_log_info("Running in test mode with sample data");
        // Test mode configuration would go here
    }

    if (benchmark_mode) {
        catastrophe_log_info("Running performance benchmarks");
        // Benchmark mode configuration would go here
    }

    if (verbose) {
        // Enable verbose logging
        setenv("CATASTROPHE_LOG_LEVEL", "DEBUG", 1);
    }

    return CE_SUCCESS;
}

// Initialize signal handlers
static void setup_signal_handlers(void) {
    struct sigaction sa;

    // Set up signal handler
    memset(&sa, 0, sizeof(sa));
    sa.sa_handler = signal_handler;
    sigemptyset(&sa.sa_mask);

    // Register signal handlers
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    sigaction(SIGHUP, &sa, NULL);
}

// Main application loop
static catastrophe_error_t run_main_loop(void) {
    catastrophe_log_info("Catastrophe Engine starting main loop...");

    while (running) {
        // Main processing loop
        // This would contain the core engine logic

        // Sleep for a short time to prevent busy waiting
        usleep(100000);  // 100ms
    }

    catastrophe_log_info("Main loop terminated");
    return CE_SUCCESS;
}

// Main entry point
int main(int argc, char *argv[]) {
    catastrophe_error_t result;

    // Print banner
    printf("🌪️  Catastrophe Engine v%s - Real-Time Risk Assessment\n",
           CATASTROPHE_ENGINE_VERSION);
    printf("================================================\n\n");

    // Parse command line arguments
    result = parse_arguments(argc, argv, &global_config);
    if (result != CE_SUCCESS) {
        fprintf(stderr, "Failed to parse arguments: %s\n", catastrophe_error_string(result));
        return EXIT_FAILURE;
    }

    // Set up signal handlers for graceful shutdown
    setup_signal_handlers();

    // Initialize the catastrophe engine
    catastrophe_log_info("Initializing Catastrophe Engine...");
    result = catastrophe_engine_init(&global_config);
    if (result != CE_SUCCESS) {
        catastrophe_log_error("Failed to initialize engine: %s", catastrophe_error_string(result));
        return EXIT_FAILURE;
    }

    // Start the engine
    catastrophe_log_info("Starting Catastrophe Engine...");
    result = catastrophe_engine_start();
    if (result != CE_SUCCESS) {
        catastrophe_log_error("Failed to start engine: %s", catastrophe_error_string(result));
        catastrophe_engine_shutdown();
        return EXIT_FAILURE;
    }

    // Run the main processing loop
    result = run_main_loop();

    // Shutdown the engine
    catastrophe_log_info("Shutting down Catastrophe Engine...");
    catastrophe_engine_stop();
    catastrophe_engine_shutdown();

    catastrophe_log_info("Catastrophe Engine shutdown complete");
    return (result == CE_SUCCESS) ? EXIT_SUCCESS : EXIT_FAILURE;
}