/**
 * @file data_processor.c
 * @brief Data Processing Module for AI Underwriting Engine
 *
 * Handles data ingestion, validation, preprocessing, and storage
 * for underwriting applications and historical data.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>

// Data processor configuration
#define MAX_DATA_SOURCES 10
#define MAX_BATCH_SIZE 1000
#define DATA_VALIDATION_TIMEOUT_MS 5000
#define MAX_DATA_FILE_SIZE (10 * 1024 * 1024) // 10MB

// Data source types
typedef enum {
    DATA_SOURCE_FILE,
    DATA_SOURCE_DATABASE,
    DATA_SOURCE_API,
    DATA_SOURCE_STREAM
} data_source_type_t;

// Data source configuration
typedef struct {
    char name[64];
    data_source_type_t type;
    char connection_string[256];
    bool enabled;
    time_t last_access;
    uint64_t records_processed;
} data_source_t;

// Data processor context
typedef struct {
    data_source_t sources[MAX_DATA_SOURCES];
    size_t num_sources;
    data_validation_rules_t *validation_rules;
    data_transformation_pipeline_t *transform_pipeline;
    data_storage_t *storage;
    pthread_mutex_t mutex;
    bool initialized;
} data_processor_context_t;

// Global context
static data_processor_context_t *dp_context = NULL;

// Forward declarations
static au_error_t validate_application_data(const underwriting_application_t *application);
static au_error_t transform_application_data(underwriting_application_t *application);
static au_error_t load_data_source_config(const char *config_file);
static au_error_t process_batch_data(underwriting_application_t *applications, size_t count);
static au_error_t store_application_data(const underwriting_application_t *application);

/**
 * @brief Initialize the data processor
 */
au_error_t au_initialize_data_processor(const char *config_file) {
    if (dp_context != NULL) {
        return AU_ERROR_ALREADY_INITIALIZED;
    }

    dp_context = calloc(1, sizeof(data_processor_context_t));
    if (dp_context == NULL) {
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    // Initialize mutex
    if (pthread_mutex_init(&dp_context->mutex, NULL) != 0) {
        free(dp_context);
        dp_context = NULL;
        return AU_ERROR_THREADING;
    }

    // Load data source configuration
    au_error_t result = load_data_source_config(config_file);
    if (result != AU_SUCCESS) {
        pthread_mutex_destroy(&dp_context->mutex);
        free(dp_context);
        dp_context = NULL;
        return result;
    }

    // Initialize validation rules
    dp_context->validation_rules = au_create_validation_rules();
    if (dp_context->validation_rules == NULL) {
        pthread_mutex_destroy(&dp_context->mutex);
        free(dp_context);
        dp_context = NULL;
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    // Initialize transformation pipeline
    dp_context->transform_pipeline = au_create_transformation_pipeline();
    if (dp_context->transform_pipeline == NULL) {
        au_destroy_validation_rules(dp_context->validation_rules);
        pthread_mutex_destroy(&dp_context->mutex);
        free(dp_context);
        dp_context = NULL;
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    // Initialize data storage
    dp_context->storage = au_create_data_storage();
    if (dp_context->storage == NULL) {
        au_destroy_transformation_pipeline(dp_context->transform_pipeline);
        au_destroy_validation_rules(dp_context->validation_rules);
        pthread_mutex_destroy(&dp_context->mutex);
        free(dp_context);
        dp_context = NULL;
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    dp_context->initialized = true;
    au_log_info("Data processor initialized with %zu data sources", dp_context->num_sources);

    return AU_SUCCESS;
}

/**
 * @brief Shutdown the data processor
 */
au_error_t au_shutdown_data_processor(void) {
    if (dp_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    pthread_mutex_lock(&dp_context->mutex);

    // Clean up components
    au_destroy_data_storage(dp_context->storage);
    au_destroy_transformation_pipeline(dp_context->transform_pipeline);
    au_destroy_validation_rules(dp_context->validation_rules);

    pthread_mutex_unlock(&dp_context->mutex);
    pthread_mutex_destroy(&dp_context->mutex);

    free(dp_context);
    dp_context = NULL;

    au_log_info("Data processor shutdown");
    return AU_SUCCESS;
}

/**
 * @brief Process underwriting application data
 */
au_error_t au_process_application_data(underwriting_application_t *application) {
    if (dp_context == NULL || !dp_context->initialized) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (application == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&dp_context->mutex);

    au_error_t result;

    // Validate application data
    result = validate_application_data(application);
    if (result != AU_SUCCESS) {
        au_log_warning("Application data validation failed: %s", au_error_string(result));
        pthread_mutex_unlock(&dp_context->mutex);
        return result;
    }

    // Transform application data
    result = transform_application_data(application);
    if (result != AU_SUCCESS) {
        au_log_warning("Application data transformation failed: %s", au_error_string(result));
        pthread_mutex_unlock(&dp_context->mutex);
        return result;
    }

    // Store application data
    result = store_application_data(application);
    if (result != AU_SUCCESS) {
        au_log_warning("Application data storage failed: %s", au_error_string(result));
        // Don't fail the entire process for storage errors
    }

    pthread_mutex_unlock(&dp_context->mutex);

    au_log_debug("Processed application data for ID %llu", (unsigned long long)application->id);

    return AU_SUCCESS;
}

/**
 * @brief Process batch of underwriting applications data
 */
au_error_t au_process_batch_application_data(underwriting_application_t *applications, size_t count) {
    if (dp_context == NULL || !dp_context->initialized) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (applications == NULL || count == 0 || count > MAX_BATCH_SIZE) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&dp_context->mutex);

    au_error_t result = process_batch_data(applications, count);

    pthread_mutex_unlock(&dp_context->mutex);

    if (result == AU_SUCCESS) {
        au_log_debug("Processed batch of %zu applications", count);
    }

    return result;
}

/**
 * @brief Load data from external source
 */
au_error_t au_load_external_data(const char *source_name, const char *query,
                               underwriting_application_t **applications, size_t *count) {
    if (dp_context == NULL || !dp_context->initialized) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (source_name == NULL || applications == NULL || count == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&dp_context->mutex);

    // Find data source
    data_source_t *source = NULL;
    for (size_t i = 0; i < dp_context->num_sources; i++) {
        if (strcmp(dp_context->sources[i].name, source_name) == 0) {
            source = &dp_context->sources[i];
            break;
        }
    }

    if (source == NULL || !source->enabled) {
        pthread_mutex_unlock(&dp_context->mutex);
        return AU_ERROR_NOT_FOUND;
    }

    au_error_t result;

    // Load data based on source type
    switch (source->type) {
        case DATA_SOURCE_FILE:
            result = au_load_data_from_file(source->connection_string, query, applications, count);
            break;
        case DATA_SOURCE_DATABASE:
            result = au_load_data_from_database(source->connection_string, query, applications, count);
            break;
        case DATA_SOURCE_API:
            result = au_load_data_from_api(source->connection_string, query, applications, count);
            break;
        default:
            result = AU_ERROR_NOT_SUPPORTED;
            break;
    }

    if (result == AU_SUCCESS) {
        source->last_access = time(NULL);
        source->records_processed += *count;
        au_log_info("Loaded %zu records from data source '%s'", *count, source_name);
    }

    pthread_mutex_unlock(&dp_context->mutex);
    return result;
}

/**
 * @brief Validate application data
 */
static au_error_t validate_application_data(const underwriting_application_t *application) {
    // Basic validation rules
    if (application->id == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (strlen(application->applicant_name) == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (application->requested_coverage <= 0.0f) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (application->requested_deductible < 0.0f ||
        application->requested_deductible >= application->requested_coverage) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Product-specific validation
    switch (application->product_type) {
        case PRODUCT_AUTO:
            if (application->product_data.auto_data.vehicle_year < 1900 ||
                application->product_data.auto_data.vehicle_year > 2030) {
                return AU_ERROR_INVALID_INPUT;
            }
            if (application->product_data.auto_data.vehicle_value <= 0.0f) {
                return AU_ERROR_INVALID_INPUT;
            }
            break;

        case PRODUCT_HOME:
            // Home-specific validation would go here
            break;

        case PRODUCT_LIFE:
            // Life-specific validation would go here
            break;

        default:
            return AU_ERROR_INVALID_INPUT;
    }

    // Address validation
    if (strlen(application->address) == 0 ||
        strlen(application->city) == 0 ||
        strlen(application->state) == 0 ||
        strlen(application->zip_code) == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    return AU_SUCCESS;
}

/**
 * @brief Transform application data
 */
static au_error_t transform_application_data(underwriting_application_t *application) {
    // Data normalization and cleaning

    // Normalize name (trim whitespace, capitalize)
    au_normalize_string(application->applicant_name, sizeof(application->applicant_name));

    // Normalize address
    au_normalize_string(application->address, sizeof(application->address));
    au_normalize_string(application->city, sizeof(application->city));
    au_normalize_string(application->state, sizeof(application->state));
    au_normalize_string(application->zip_code, sizeof(application->zip_code));

    // Normalize gender
    if (strcasecmp(application->gender, "male") == 0 ||
        strcasecmp(application->gender, "m") == 0) {
        strcpy(application->gender, "M");
    } else if (strcasecmp(application->gender, "female") == 0 ||
               strcasecmp(application->gender, "f") == 0) {
        strcpy(application->gender, "F");
    } else {
        strcpy(application->gender, "O"); // Other
    }

    // Product-specific transformations
    switch (application->product_type) {
        case PRODUCT_AUTO:
            // Normalize license number
            au_normalize_string(application->product_data.auto_data.license_number,
                              sizeof(application->product_data.auto_data.license_number));
            break;

        default:
            break;
    }

    return AU_SUCCESS;
}

/**
 * @brief Process batch data
 */
static au_error_t process_batch_data(underwriting_application_t *applications, size_t count) {
    size_t success_count = 0;
    au_error_t last_error = AU_SUCCESS;

    for (size_t i = 0; i < count; i++) {
        au_error_t result = au_process_application_data(&applications[i]);
        if (result == AU_SUCCESS) {
            success_count++;
        } else {
            last_error = result;
            au_log_warning("Failed to process application %zu in batch: %s",
                          i, au_error_string(result));
        }
    }

    if (success_count == 0) {
        return last_error;
    }

    if (success_count < count) {
        au_log_warning("Batch processing completed with %zu/%zu successes", success_count, count);
    }

    return AU_SUCCESS;
}

/**
 * @brief Store application data
 */
static au_error_t store_application_data(const underwriting_application_t *application) {
    // In a real implementation, this would store to database/file system
    // For now, just log the storage operation

    au_log_debug("Storing application data for ID %llu", (unsigned long long)application->id);

    // Placeholder for actual storage implementation
    return AU_SUCCESS;
}

/**
 * @brief Load data source configuration
 */
static au_error_t load_data_source_config(const char *config_file) {
    // Placeholder implementation - in real system would parse JSON/XML config
    // For now, set up default data sources

    dp_context->num_sources = 0;

    // Add default file data source
    data_source_t *file_source = &dp_context->sources[dp_context->num_sources++];
    strcpy(file_source->name, "applications_file");
    file_source->type = DATA_SOURCE_FILE;
    strcpy(file_source->connection_string, "data/applications.csv");
    file_source->enabled = true;

    // Add database data source
    data_source_t *db_source = &dp_context->sources[dp_context->num_sources++];
    strcpy(db_source->name, "applications_db");
    db_source->type = DATA_SOURCE_DATABASE;
    strcpy(db_source->connection_string, "postgresql://localhost/underwriting");
    db_source->enabled = false; // Disabled by default

    return AU_SUCCESS;
}

/**
 * @brief Create validation rules
 */
data_validation_rules_t *au_create_validation_rules(void) {
    data_validation_rules_t *rules = calloc(1, sizeof(data_validation_rules_t));
    if (rules == NULL) {
        return NULL;
    }

    // Initialize with default validation rules
    rules->min_coverage = 10000.0f;
    rules->max_coverage = 10000000.0f;
    rules->min_deductible = 0.0f;
    rules->max_age = 100;
    rules->min_age = 18;

    return rules;
}

/**
 * @brief Destroy validation rules
 */
void au_destroy_validation_rules(data_validation_rules_t *rules) {
    free(rules);
}

/**
 * @brief Create transformation pipeline
 */
data_transformation_pipeline_t *au_create_transformation_pipeline(void) {
    data_transformation_pipeline_t *pipeline = calloc(1, sizeof(data_transformation_pipeline_t));
    if (pipeline == NULL) {
        return NULL;
    }

    // Initialize transformation steps
    pipeline->num_steps = 0;

    return pipeline;
}

/**
 * @brief Destroy transformation pipeline
 */
void au_destroy_transformation_pipeline(data_transformation_pipeline_t *pipeline) {
    free(pipeline);
}

/**
 * @brief Create data storage
 */
data_storage_t *au_create_data_storage(void) {
    data_storage_t *storage = calloc(1, sizeof(data_storage_t));
    if (storage == NULL) {
        return NULL;
    }

    // Initialize storage configuration
    strcpy(storage->storage_path, "./data/storage");
    storage->max_file_size = MAX_DATA_FILE_SIZE;
    storage->compression_enabled = true;

    return storage;
}

/**
 * @brief Destroy data storage
 */
void au_destroy_data_storage(data_storage_t *storage) {
    free(storage);
}

/**
 * @brief Load data from file
 */
au_error_t au_load_data_from_file(const char *filename, const char *query,
                                underwriting_application_t **applications, size_t *count) {
    // Placeholder implementation for file data loading
    // In real implementation would parse CSV/JSON files

    *applications = NULL;
    *count = 0;

    au_log_info("Loading data from file: %s", filename);
    return AU_ERROR_NOT_IMPLEMENTED; // Placeholder
}

/**
 * @brief Load data from database
 */
au_error_t au_load_data_from_database(const char *connection_string, const char *query,
                                    underwriting_application_t **applications, size_t *count) {
    // Placeholder implementation for database loading
    // In real implementation would use database connectors

    *applications = NULL;
    *count = 0;

    au_log_info("Loading data from database: %s", connection_string);
    return AU_ERROR_NOT_IMPLEMENTED; // Placeholder
}

/**
 * @brief Load data from API
 */
au_error_t au_load_data_from_api(const char *endpoint, const char *query,
                               underwriting_application_t **applications, size_t *count) {
    // Placeholder implementation for API data loading
    // In real implementation would make HTTP requests

    *applications = NULL;
    *count = 0;

    au_log_info("Loading data from API: %s", endpoint);
    return AU_ERROR_NOT_IMPLEMENTED; // Placeholder
}

/**
 * @brief Normalize string (trim whitespace, capitalize)
 */
au_error_t au_normalize_string(char *str, size_t max_len) {
    if (str == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Trim leading whitespace
    char *start = str;
    while (*start && isspace(*start)) {
        start++;
    }

    // Trim trailing whitespace
    char *end = start + strlen(start) - 1;
    while (end > start && isspace(*end)) {
        *end-- = '\0';
    }

    // Move string to beginning if needed
    if (start != str) {
        memmove(str, start, strlen(start) + 1);
    }

    // Capitalize first letter of each word
    bool capitalize = true;
    for (char *p = str; *p && (p - str) < (int)max_len; p++) {
        if (isspace(*p)) {
            capitalize = true;
        } else if (capitalize) {
            *p = toupper(*p);
            capitalize = false;
        } else {
            *p = tolower(*p);
        }
    }

    return AU_SUCCESS;
}