/**
 * @file ai_underwriting_engine.h
 * @brief AI-Augmented Underwriting Engine Header
 *
 * Main header file for the AI underwriting engine providing
 * real-time insurance risk assessment with machine learning.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef AI_UNDERWRITING_ENGINE_H
#define AI_UNDERWRITING_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include <pthread.h>
#include <unistd.h>
#include <stdarg.h>

// Version information
#define AI_UNDERWRITING_ENGINE_VERSION "1.0.0"
#define AI_UNDERWRITING_ENGINE_BUILD_DATE __DATE__
#define AI_UNDERWRITING_ENGINE_BUILD_TIME __TIME__

// Configuration constants
#define MAX_FEATURES 256
#define MAX_MODELS 16
#define MAX_RULES 1024
#define MAX_APPLICATION_SIZE (1024 * 1024)  // 1MB
#define MAX_BATCH_SIZE 1000
#define DEFAULT_INFERENCE_TIMEOUT_MS 5000

// Performance tuning
#define DEFAULT_MODEL_CACHE_SIZE (512 * 1024 * 1024)  // 512MB
#define DEFAULT_FEATURE_CACHE_SIZE (256 * 1024 * 1024)  // 256MB
#define MAX_CONCURRENT_INFERENCES 100
#define WORKER_THREAD_POOL_SIZE 32

// Data types
typedef uint64_t application_id_t;
typedef uint32_t model_id_t;
typedef uint32_t rule_id_t;
typedef double risk_score_t;
typedef double premium_amount_t;

// Forward declarations
struct underwriting_application;
struct underwriting_decision;
struct model_instance;
struct rule_engine;

// Error codes
typedef enum {
    AU_SUCCESS = 0,
    AU_ERROR_INVALID_INPUT = -1,
    AU_ERROR_MEMORY_ALLOCATION = -2,
    AU_ERROR_MODEL_LOAD_FAILED = -3,
    AU_ERROR_INFERENCE_FAILED = -4,
    AU_ERROR_RULE_ENGINE_ERROR = -5,
    AU_ERROR_DATA_VALIDATION_FAILED = -6,
    AU_ERROR_TIMEOUT = -7,
    AU_ERROR_CONFIGURATION_ERROR = -8,
    AU_ERROR_UNKNOWN = -99
} au_error_t;

// Underwriting decision types
typedef enum {
    DECISION_APPROVED = 1,
    DECISION_DENIED = 2,
    DECISION_REFERRED = 3,
    DECISION_PENDING = 4
} decision_type_t;

// Insurance product types
typedef enum {
    PRODUCT_AUTO = 1,
    PRODUCT_HOME = 2,
    PRODUCT_LIFE = 3,
    PRODUCT_HEALTH = 4,
    PRODUCT_COMMERCIAL = 5,
    PRODUCT_SPECIALTY = 6
} product_type_t;

// Risk levels
typedef enum {
    RISK_LOW = 1,
    RISK_MEDIUM = 2,
    RISK_HIGH = 3,
    RISK_VERY_HIGH = 4
} risk_level_t;

// Feature types
typedef enum {
    FEATURE_NUMERICAL = 1,
    FEATURE_CATEGORICAL = 2,
    FEATURE_TEXT = 3
} feature_type_t;

// Feature structure
typedef struct {
    char name[64];
    double value;
    feature_type_t type;
    bool is_normalized;
    bool is_encoded;
} feature_t;

// Enhanced feature vector
typedef struct {
    application_id_t application_id;
    size_t num_features;
    feature_t features[MAX_FEATURES];
    time_t timestamp;
} feature_vector_t;

// Feature cache
typedef struct {
    size_t capacity;
    size_t size;
    struct {
        application_id_t application_id;
        feature_vector_t features;
        time_t timestamp;
    } *entries;
    pthread_mutex_t mutex;
} feature_cache_t;

// Model metadata
typedef struct {
    model_id_t id;
    char name[128];
    char version[32];
    product_type_t product_type;
    char model_path[512];
    char config_path[512];
    time_t created_time;
    time_t last_updated;
    double accuracy_score;
    size_t input_features;
    size_t output_classes;
} model_metadata_t;

// Model instance (loaded ML model)
typedef struct {
    model_id_t id;
    char model_path[512];
    void *model_handle;
    model_metadata_t metadata;
    size_t memory_usage;
    time_t load_time;
    bool is_loaded;
} model_instance_t;

// Data validation rules
typedef struct {
    double min_coverage;
    double max_coverage;
    double min_deductible;
    int min_age;
    int max_age;
    // Additional validation rules...
} data_validation_rules_t;

// Data transformation pipeline
typedef struct {
    size_t num_steps;
    // Transformation steps would be defined here
} data_transformation_pipeline_t;

// Engine configuration
typedef struct {
    char config_file[512];
    char model_directory[512];
    char data_directory[512];
    char log_file[512];
    int api_port;
    bool enable_gpu;
    size_t max_batch_size;
    int inference_timeout_ms;
    size_t worker_threads;
    bool enable_monitoring;
    bool enable_rule_engine;
    bool enable_explainability;
    size_t model_cache_size;
    size_t feature_cache_size;
} au_config_t;

// Data storage
typedef struct {
    char storage_path[512];
    size_t max_file_size;
    bool compression_enabled;
    // Additional storage configuration...
} data_storage_t;

// Underwriting application
typedef struct underwriting_application {
    application_id_t id;
    product_type_t product_type;
    char applicant_name[256];
    char applicant_email[256];
    time_t application_date;
    double requested_coverage;
    double requested_deductible;

    // Personal information
    char date_of_birth[16];
    char gender[16];
    char marital_status[16];
    char occupation[64];

    // Location information
    char address[512];
    char city[128];
    char state[64];
    char zip_code[16];

    // Product-specific data (union for different product types)
    union {
        struct {
            char vehicle_make[64];
            char vehicle_model[64];
            int vehicle_year;
            double vehicle_value;
            int annual_mileage;
            char license_number[32];
        } auto_data;

        struct {
            char property_type[32];
            double property_value;
            int year_built;
            char construction_type[32];
            double square_footage;
        } home_data;

        struct {
            double annual_income;
            char health_conditions[1024];
            char smoker_status[16];
        } life_data;
    } product_data;

    // Additional data
    char additional_data[4096];  // JSON string for extensibility
} underwriting_application_t;

// Underwriting decision
typedef struct underwriting_decision {
    application_id_t application_id;
    decision_type_t decision;
    risk_level_t risk_level;
    risk_score_t risk_score;
    premium_amount_t calculated_premium;
    char explanation[2048];
    time_t decision_time;
    double confidence_score;
    char recommended_conditions[1024];
} underwriting_decision_t;

// Rule engine rule
typedef struct {
    rule_id_t id;
    char name[128];
    char description[512];
    product_type_t product_type;
    char condition[2048];  // Expression to evaluate
    char action[1024];     // Action to take if condition met
    int priority;
    bool enabled;
} underwriting_rule_t;

// Engine configuration
typedef struct {
    char config_file[512];
    char log_file[512];
    char model_directory[512];
    char data_directory[512];
    uint16_t api_port;
    bool enable_gpu;
    bool enable_monitoring;
    uint32_t max_batch_size;
    uint32_t inference_timeout_ms;
    uint32_t worker_threads;
    size_t model_cache_size;
    size_t feature_cache_size;
    bool enable_rule_engine;
    bool enable_explainability;
} engine_config_t;

// Function prototypes

// Core engine functions
au_error_t au_engine_init(const au_config_t *config);
au_error_t au_engine_start(void);
au_error_t au_engine_stop(void);
au_error_t au_engine_shutdown(void);

// Application processing
au_error_t au_process_application(const underwriting_application_t *application,
                                underwriting_decision_t *decision);
au_error_t au_process_batch_applications(const underwriting_application_t *applications,
                                       size_t num_applications,
                                       underwriting_decision_t *decisions);

// Model management
au_error_t au_load_model(const char *model_path, model_id_t *model_id);
au_error_t au_unload_model(model_id_t model_id);
au_error_t au_get_model_info(model_id_t model_id, model_metadata_t *metadata);
au_error_t au_list_models(model_metadata_t *models, size_t *num_models);

// Rule engine
au_error_t au_add_rule(const underwriting_rule_t *rule);
au_error_t au_remove_rule(rule_id_t rule_id);
au_error_t au_update_rule(const underwriting_rule_t *rule);
au_error_t au_list_rules(underwriting_rule_t *rules, size_t *num_rules);

// Feature engineering
au_error_t au_initialize_feature_engineering(void);
au_error_t au_shutdown_feature_engineering(void);
au_error_t au_extract_features(const underwriting_application_t *application,
                             feature_vector_t *features);
au_error_t au_add_feature(feature_vector_t *features, const char *name,
                         float value, feature_type_t type);
au_error_t au_get_feature_value(const feature_vector_t *features, const char *name, float *value);
feature_cache_t *au_create_feature_cache(size_t capacity);
void au_destroy_feature_cache(feature_cache_t *cache);
au_error_t au_cache_features(feature_cache_t *cache, uint64_t application_id,
                           const feature_vector_t *features);
au_error_t au_get_cached_features(feature_cache_t *cache, uint64_t application_id,
                                feature_vector_t *features);

// Data processing
au_error_t au_initialize_data_processor(const char *config_file);
au_error_t au_shutdown_data_processor(void);
au_error_t au_process_application_data(underwriting_application_t *application);
au_error_t au_process_batch_application_data(underwriting_application_t *applications, size_t count);
au_error_t au_load_external_data(const char *source_name, const char *query,
                               underwriting_application_t **applications, size_t *count);
au_error_t au_normalize_string(char *str, size_t max_len);

// Data processing internal functions
data_validation_rules_t *au_create_validation_rules(void);
void au_destroy_validation_rules(data_validation_rules_t *rules);
data_transformation_pipeline_t *au_create_transformation_pipeline(void);
void au_destroy_transformation_pipeline(data_transformation_pipeline_t *pipeline);
data_storage_t *au_create_data_storage(void);
void au_destroy_data_storage(data_storage_t *storage);
au_error_t au_load_data_from_file(const char *filename, const char *query,
                                underwriting_application_t **applications, size_t *count);
au_error_t au_load_data_from_database(const char *connection_string, const char *query,
                                    underwriting_application_t **applications, size_t *count);
au_error_t au_load_data_from_api(const char *endpoint, const char *query,
                               underwriting_application_t **applications, size_t *count);

// Inference engine
au_error_t au_initialize_inference_engine(void);
au_error_t au_shutdown_inference_engine(void);
au_error_t au_inference_engine_health_check(bool *healthy);

// API server
au_error_t au_initialize_api_server(int port);
au_error_t au_start_api_server(void);
au_error_t au_stop_api_server(void);
au_error_t au_shutdown_api_server(void);

// Inference
au_error_t au_run_inference(model_id_t model_id,
                          const feature_vector_t *features,
                          double *prediction,
                          double *confidence);

// Utility functions
const char *au_error_string(au_error_t error);
const char *au_decision_string(decision_type_t decision);
const char *au_risk_level_string(risk_level_t risk);
const char *au_product_string(product_type_t product);

// Memory management
void *au_malloc(size_t size);
void *au_calloc(size_t nmemb, size_t size);
void *au_realloc(void *ptr, size_t size);
void au_free(void *ptr);

// Logging
void au_log_error(const char *format, ...);
void au_log_warning(const char *format, ...);
void au_log_info(const char *format, ...);
void au_log_debug(const char *format, ...);

// Global variables (extern declarations)
extern pthread_mutex_t rule_mutex;
extern size_t num_rules;
extern underwriting_rule_t rule_registry[MAX_RULES];
extern au_config_t *current_config;

#endif // AI_UNDERWRITING_ENGINE_H