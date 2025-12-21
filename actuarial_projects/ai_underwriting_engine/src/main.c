/**
 * @file main.c
 * @brief AI Underwriting Engine Main Executable
 *
 * Command-line interface and main entry point for the AI underwriting engine.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#define _GNU_SOURCE
#include "ai_underwriting_engine.h"
#include <getopt.h>
#include <signal.h>
#include <stdlib.h>

// Global variables for signal handling
static volatile bool running = true;

// Signal handler
static void signal_handler(int signum) {
    (void)signum;
    running = false;
    au_log_info("Received shutdown signal");
}

// Print usage information
static void print_usage(const char *program_name) {
    printf("AI Underwriting Engine v%s\n", AI_UNDERWRITING_ENGINE_VERSION);
    printf("Usage: %s [OPTIONS]\n\n", program_name);
    printf("Options:\n");
    printf("  -c, --config FILE       Configuration file path\n");
    printf("  -p, --port PORT         API server port (default: 8080)\n");
    printf("  -d, --data-dir DIR      Data directory path\n");
    printf("  -m, --model-dir DIR     Model directory path\n");
    printf("  -t, --test              Run in test mode\n");
    printf("  -v, --verbose           Enable verbose logging\n");
    printf("  -h, --help              Show this help message\n");
    printf("\nExamples:\n");
    printf("  %s --config config.json --port 9090\n", program_name);
    printf("  %s --test --verbose\n", program_name);
}

// Parse command line arguments
static au_error_t parse_arguments(int argc, char *argv[], au_config_t *config, bool *test_mode) {
    static struct option long_options[] = {
        {"config", required_argument, 0, 'c'},
        {"port", required_argument, 0, 'p'},
        {"data-dir", required_argument, 0, 'd'},
        {"model-dir", required_argument, 0, 'm'},
        {"test", no_argument, 0, 't'},
        {"verbose", no_argument, 0, 'v'},
        {"help", no_argument, 0, 'h'},
        {0, 0, 0, 0}
    };

    // Set defaults
    memset(config, 0, sizeof(au_config_t));
    strcpy(config->config_file, "config.json");
    strcpy(config->log_file, "ai_underwriting.log");
    strcpy(config->data_directory, "./data");
    strcpy(config->model_directory, "./models");
    config->api_port = 8080;
    config->enable_gpu = false;
    config->enable_monitoring = true;
    config->max_batch_size = 100;
    config->inference_timeout_ms = 5000;
    config->worker_threads = 4;
    config->model_cache_size = DEFAULT_MODEL_CACHE_SIZE;
    config->feature_cache_size = DEFAULT_FEATURE_CACHE_SIZE;
    config->enable_rule_engine = true;
    config->enable_explainability = true;

    *test_mode = false;

    int opt;
    while ((opt = getopt_long(argc, argv, "c:p:d:m:tvh", long_options, NULL)) != -1) {
        switch (opt) {
            case 'c':
                strcpy(config->config_file, optarg);
                break;
            case 'p':
                config->api_port = atoi(optarg);
                break;
            case 'd':
                strcpy(config->data_directory, optarg);
                break;
            case 'm':
                strcpy(config->model_directory, optarg);
                break;
            case 't':
                *test_mode = true;
                break;
            case 'v':
                setenv("AU_LOG_LEVEL", "DEBUG", 1);
                break;
            case 'h':
            default:
                print_usage(argv[0]);
                return AU_ERROR_INVALID_INPUT;
        }
    }

    return AU_SUCCESS;
}

// Run test mode
static au_error_t run_test_mode(void) {
    au_log_info("Running AI Underwriting Engine in test mode");

    // Create sample application
    underwriting_application_t application = {0};
    application.id = 1;
    application.product_type = PRODUCT_AUTO;
    strcpy(application.applicant_name, "John Doe");
    strcpy(application.date_of_birth, "1990");
    strcpy(application.gender, "M");
    application.requested_coverage = 500000.0;
    application.requested_deductible = 1000.0;

    // Auto-specific data
    application.product_data.auto_data.vehicle_year = 2018;
    application.product_data.auto_data.vehicle_value = 25000.0;
    application.product_data.auto_data.annual_mileage = 12000;
    strcpy(application.product_data.auto_data.license_number, "DL123456");

    strcpy(application.address, "123 Main St");
    strcpy(application.city, "Anytown");
    strcpy(application.state, "CA");
    strcpy(application.zip_code, "12345");

    au_log_info("Created test application for %s", application.applicant_name);

    // Process application
    underwriting_decision_t decision;
    au_error_t result = au_process_application(&application, &decision);

    if (result == AU_SUCCESS) {
        au_log_info("Test application processed successfully:");
        au_log_info("  Decision: %s", au_decision_string(decision.decision));
        au_log_info("  Risk Score: %.3f", decision.risk_score);
        au_log_info("  Risk Level: %s", au_risk_level_string(decision.risk_level));
        au_log_info("  Premium: $%.2f", decision.calculated_premium);
        au_log_info("  Confidence: %.1f%%", decision.confidence_score * 100);
        au_log_info("  Explanation: %s", decision.explanation);
    } else {
        au_log_error("Test application processing failed: %s", au_error_string(result));
        return result;
    }

    // Test batch processing
    au_log_info("Testing batch processing...");

    underwriting_application_t batch[3];
    underwriting_decision_t batch_decisions[3];

    // Create batch of applications
    for (int i = 0; i < 3; i++) {
        memcpy(&batch[i], &application, sizeof(underwriting_application_t));
        batch[i].id = i + 2;
        batch[i].requested_coverage = 300000.0 + (i * 100000.0);
    }

    result = au_process_batch_applications(batch, 3, batch_decisions);
    if (result == AU_SUCCESS) {
        au_log_info("Batch processing completed successfully");
        for (int i = 0; i < 3; i++) {
            au_log_info("  Application %llu: %s ($%.2f)",
                       (unsigned long long)batch_decisions[i].application_id,
                       au_decision_string(batch_decisions[i].decision),
                       batch_decisions[i].calculated_premium);
        }
    } else {
        au_log_error("Batch processing failed: %s", au_error_string(result));
    }

    return AU_SUCCESS;
}

// Main function
int main(int argc, char *argv[]) {
    // Set up signal handlers
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

    au_log_info("Starting AI Underwriting Engine v%s", AI_UNDERWRITING_ENGINE_VERSION);

    // Parse command line arguments
    au_config_t config;
    bool test_mode;

    au_error_t result = parse_arguments(argc, argv, &config, &test_mode);
    if (result != AU_SUCCESS) {
        return EXIT_FAILURE;
    }

    // Initialize engine
    result = au_engine_init(&config);
    if (result != AU_SUCCESS) {
        au_log_error("Engine initialization failed: %s", au_error_string(result));
        return EXIT_FAILURE;
    }

    // Start engine
    result = au_engine_start();
    if (result != AU_SUCCESS) {
        au_log_error("Engine start failed: %s", au_error_string(result));
        au_engine_shutdown();
        return EXIT_FAILURE;
    }

    // Initialize inference engine
    result = au_initialize_inference_engine();
    if (result != AU_SUCCESS) {
        au_log_error("Inference engine initialization failed: %s", au_error_string(result));
        au_engine_stop();
        au_engine_shutdown();
        return EXIT_FAILURE;
    }

    if (test_mode) {
        // Run test mode
        result = run_test_mode();
        if (result != AU_SUCCESS) {
            au_log_error("Test mode failed: %s", au_error_string(result));
        }
    } else {
        // Run main server loop (placeholder)
        au_log_info("AI Underwriting Engine running (press Ctrl+C to stop)");

        while (running) {
            // Main server loop would go here
            // For now, just sleep and check for shutdown
            sleep(1);

            // Periodic health check
            static time_t last_health_check = 0;
            time_t now = time(NULL);
            if (now - last_health_check >= 60) {  // Every minute
                bool healthy;
                au_inference_engine_health_check(&healthy);
                if (!healthy) {
                    au_log_warning("Health check failed");
                }
                last_health_check = now;
            }
        }
    }

    // Shutdown
    au_log_info("Shutting down AI Underwriting Engine...");

    au_shutdown_inference_engine();
    au_engine_stop();
    au_engine_shutdown();

    au_log_info("AI Underwriting Engine shutdown complete");
    return result == AU_SUCCESS ? EXIT_SUCCESS : EXIT_FAILURE;
}