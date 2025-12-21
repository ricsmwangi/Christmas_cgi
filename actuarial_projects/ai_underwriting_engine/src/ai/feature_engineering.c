/**
 * @file feature_engineering.c
 * @brief Feature Engineering Pipeline for AI Underwriting Engine
 *
 * Extracts and transforms raw application data into ML-ready features
 * for underwriting risk assessment and pricing.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <math.h>
#include <string.h>
#include <time.h>

// Feature engineering configuration
#define MAX_FEATURES 256
#define MAX_CATEGORICAL_VALUES 1000
#define AGE_NORMALIZATION_FACTOR 100.0f
#define INCOME_NORMALIZATION_FACTOR 100000.0f
#define VEHICLE_VALUE_NORMALIZATION_FACTOR 100000.0f

// Feature metadata structure
typedef struct {
    char name[64];
    feature_type_t type;
    bool is_normalized;
    float min_val;
    float max_val;
    float mean_val;
    float std_val;
} feature_metadata_t;

// Feature engineering context
typedef struct {
    feature_metadata_t features[MAX_FEATURES];
    size_t num_features;
    categorical_encoder_t *categorical_encoders;
    size_t num_encoders;
    feature_cache_t *cache;
    pthread_mutex_t mutex;
} feature_engineering_context_t;

// Global context
static feature_engineering_context_t *fe_context = NULL;

// Forward declarations
static au_error_t extract_applicant_features(const underwriting_application_t *app, feature_vector_t *features);
static au_error_t extract_product_features(const underwriting_application_t *app, feature_vector_t *features);
static au_error_t extract_risk_features(const underwriting_application_t *app, feature_vector_t *features);
static au_error_t normalize_features(feature_vector_t *features);
static au_error_t encode_categorical_features(feature_vector_t *features);
static au_error_t validate_features(const feature_vector_t *features);

/**
 * @brief Initialize the feature engineering pipeline
 */
au_error_t au_initialize_feature_engineering(void) {
    if (fe_context != NULL) {
        return AU_ERROR_ALREADY_INITIALIZED;
    }

    fe_context = calloc(1, sizeof(feature_engineering_context_t));
    if (fe_context == NULL) {
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    // Initialize mutex
    if (pthread_mutex_init(&fe_context->mutex, NULL) != 0) {
        free(fe_context);
        fe_context = NULL;
        return AU_ERROR_THREADING;
    }

    // Initialize feature metadata
    fe_context->num_features = 0;

    // Initialize categorical encoders
    fe_context->categorical_encoders = NULL;
    fe_context->num_encoders = 0;

    // Initialize feature cache
    fe_context->cache = au_create_feature_cache(DEFAULT_FEATURE_CACHE_SIZE);
    if (fe_context->cache == NULL) {
        pthread_mutex_destroy(&fe_context->mutex);
        free(fe_context);
        fe_context = NULL;
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    au_log_info("Feature engineering pipeline initialized");
    return AU_SUCCESS;
}

/**
 * @brief Shutdown the feature engineering pipeline
 */
au_error_t au_shutdown_feature_engineering(void) {
    if (fe_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    pthread_mutex_lock(&fe_context->mutex);

    // Clean up categorical encoders
    for (size_t i = 0; i < fe_context->num_encoders; i++) {
        if (fe_context->categorical_encoders[i].values) {
            free(fe_context->categorical_encoders[i].values);
        }
    }
    free(fe_context->categorical_encoders);

    // Clean up feature cache
    au_destroy_feature_cache(fe_context->cache);

    pthread_mutex_unlock(&fe_context->mutex);
    pthread_mutex_destroy(&fe_context->mutex);

    free(fe_context);
    fe_context = NULL;

    au_log_info("Feature engineering pipeline shutdown");
    return AU_SUCCESS;
}

/**
 * @brief Extract features from underwriting application
 */
au_error_t au_extract_features(const underwriting_application_t *application,
                              feature_vector_t *features) {
    if (fe_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (application == NULL || features == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&fe_context->mutex);

    // Check cache first
    if (au_get_cached_features(fe_context->cache, application->id, features) == AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return AU_SUCCESS;
    }

    // Initialize feature vector
    memset(features, 0, sizeof(feature_vector_t));
    features->application_id = application->id;
    features->timestamp = time(NULL);

    au_error_t result;

    // Extract applicant features
    result = extract_applicant_features(application, features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Extract product-specific features
    result = extract_product_features(application, features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Extract risk assessment features
    result = extract_risk_features(application, features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Encode categorical features
    result = encode_categorical_features(features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Normalize numerical features
    result = normalize_features(features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Validate features
    result = validate_features(features);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&fe_context->mutex);
        return result;
    }

    // Cache the features
    au_cache_features(fe_context->cache, application->id, features);

    pthread_mutex_unlock(&fe_context->mutex);

    au_log_debug("Extracted %zu features for application %llu",
                features->num_features, (unsigned long long)application->id);

    return AU_SUCCESS;
}

/**
 * @brief Extract applicant demographic features
 */
static au_error_t extract_applicant_features(const underwriting_application_t *app,
                                           feature_vector_t *features) {
    // Age calculation
    time_t now = time(NULL);
    struct tm *current_time = localtime(&now);
    int current_year = current_time->tm_year + 1900;

    int birth_year = atoi(app->date_of_birth);
    if (birth_year < 1900 || birth_year > current_year) {
        birth_year = current_year - 35; // Default to 35 years old
    }

    float age = (float)(current_year - birth_year);
    au_add_feature(features, "applicant_age", age / AGE_NORMALIZATION_FACTOR, FEATURE_NUMERICAL);

    // Gender encoding (0 = Male, 1 = Female, 2 = Other)
    float gender_code = 2.0f; // Default to Other
    if (strcmp(app->gender, "M") == 0 || strcmp(app->gender, "Male") == 0) {
        gender_code = 0.0f;
    } else if (strcmp(app->gender, "F") == 0 || strcmp(app->gender, "Female") == 0) {
        gender_code = 1.0f;
    }
    au_add_feature(features, "applicant_gender", gender_code, FEATURE_CATEGORICAL);

    // Location features (simplified - in real system would use geocoding)
    // State risk factor (simplified mapping)
    float state_risk = 0.5f; // Default medium risk
    if (strcmp(app->state, "CA") == 0 || strcmp(app->state, "NY") == 0) {
        state_risk = 0.7f; // Higher risk due to population density
    } else if (strcmp(app->state, "TX") == 0 || strcmp(app->state, "FL") == 0) {
        state_risk = 0.8f; // Higher risk due to weather events
    }
    au_add_feature(features, "location_risk_factor", state_risk, FEATURE_NUMERICAL);

    return AU_SUCCESS;
}

/**
 * @brief Extract product-specific features
 */
static au_error_t extract_product_features(const underwriting_application_t *app,
                                         feature_vector_t *features) {
    // Coverage amount (normalized)
    float coverage_ratio = app->requested_coverage / 1000000.0f; // Normalize to millions
    au_add_feature(features, "requested_coverage_ratio", coverage_ratio, FEATURE_NUMERICAL);

    // Deductible ratio
    float deductible_ratio = app->requested_deductible / app->requested_coverage;
    au_add_feature(features, "deductible_ratio", deductible_ratio, FEATURE_NUMERICAL);

    // Product type encoding
    float product_code = 0.0f;
    switch (app->product_type) {
        case PRODUCT_AUTO:
            product_code = 0.0f;
            break;
        case PRODUCT_HOME:
            product_code = 1.0f;
            break;
        case PRODUCT_LIFE:
            product_code = 2.0f;
            break;
        default:
            product_code = 3.0f; // Other
            break;
    }
    au_add_feature(features, "product_type", product_code, FEATURE_CATEGORICAL);

    // Product-specific features
    switch (app->product_type) {
        case PRODUCT_AUTO:
            // Vehicle age
            time_t now = time(NULL);
            struct tm *current_time = localtime(&now);
            int current_year = current_time->tm_year + 1900;
            float vehicle_age = (float)(current_year - app->product_data.auto_data.vehicle_year);
            au_add_feature(features, "vehicle_age", vehicle_age / 20.0f, FEATURE_NUMERICAL);

            // Vehicle value ratio
            float vehicle_value_ratio = app->product_data.auto_data.vehicle_value / 100000.0f;
            au_add_feature(features, "vehicle_value_ratio", vehicle_value_ratio, FEATURE_NUMERICAL);

            // Annual mileage (normalized)
            float mileage_factor = app->product_data.auto_data.annual_mileage / 15000.0f;
            au_add_feature(features, "annual_mileage_factor", mileage_factor, FEATURE_NUMERICAL);
            break;

        case PRODUCT_HOME:
            // Home value would be extracted here
            au_add_feature(features, "home_value_ratio", 0.5f, FEATURE_NUMERICAL); // Placeholder
            au_add_feature(features, "home_age", 0.3f, FEATURE_NUMERICAL); // Placeholder
            break;

        case PRODUCT_LIFE:
            // Life insurance specific features
            au_add_feature(features, "coverage_term", 0.5f, FEATURE_NUMERICAL); // Placeholder
            break;

        default:
            break;
    }

    return AU_SUCCESS;
}

/**
 * @brief Extract risk assessment features
 */
static au_error_t extract_risk_features(const underwriting_application_t *app,
                                      feature_vector_t *features) {
    // Risk score based on coverage to income ratio (placeholder calculation)
    float risk_score = 0.5f; // Default medium risk

    // Higher coverage relative to typical values increases risk
    if (app->requested_coverage > 750000.0f) {
        risk_score += 0.2f;
    }

    // Higher deductible reduces risk (better risk management)
    if (app->requested_deductible > 5000.0f) {
        risk_score -= 0.1f;
    }

    au_add_feature(features, "calculated_risk_score", risk_score, FEATURE_NUMERICAL);

    // Temporal features
    time_t now = time(NULL);
    struct tm *time_info = localtime(&now);
    float month_factor = (float)time_info->tm_mon / 11.0f; // 0-1 scale
    au_add_feature(features, "application_month", month_factor, FEATURE_NUMERICAL);

    return AU_SUCCESS;
}

/**
 * @brief Normalize numerical features
 */
static au_error_t normalize_features(feature_vector_t *features) {
    for (size_t i = 0; i < features->num_features; i++) {
        feature_t *feature = &features->features[i];

        if (feature->type == FEATURE_NUMERICAL && !feature->is_normalized) {
            // Z-score normalization (placeholder - in production would use training set statistics)
            float mean = 0.5f; // Placeholder mean
            float std = 0.2f;  // Placeholder std

            feature->value = (feature->value - mean) / std;
            feature->is_normalized = true;
        }
    }

    return AU_SUCCESS;
}

/**
 * @brief Encode categorical features
 */
static au_error_t encode_categorical_features(feature_vector_t *features) {
    for (size_t i = 0; i < features->num_features; i++) {
        feature_t *feature = &features->features[i];

        if (feature->type == FEATURE_CATEGORICAL) {
            // One-hot encoding would be applied here
            // For now, keep as-is (assuming already encoded numerically)
            feature->is_encoded = true;
        }
    }

    return AU_SUCCESS;
}

/**
 * @brief Validate extracted features
 */
static au_error_t validate_features(const feature_vector_t *features) {
    if (features->num_features == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (features->num_features > MAX_FEATURES) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Check for NaN or infinite values
    for (size_t i = 0; i < features->num_features; i++) {
        const feature_t *feature = &features->features[i];

        if (isnan(feature->value) || isinf(feature->value)) {
            au_log_warning("Invalid feature value detected: %s = %f", feature->name, feature->value);
            return AU_ERROR_INVALID_INPUT;
        }

        // Check bounds
        if (feature->value < -10.0f || feature->value > 10.0f) {
            au_log_warning("Feature value out of bounds: %s = %f", feature->name, feature->value);
        }
    }

    return AU_SUCCESS;
}

/**
 * @brief Add feature to feature vector
 */
au_error_t au_add_feature(feature_vector_t *features, const char *name,
                         float value, feature_type_t type) {
    if (features->num_features >= MAX_FEATURES) {
        return AU_ERROR_CAPACITY_EXCEEDED;
    }

    feature_t *feature = &features->features[features->num_features++];
    strncpy(feature->name, name, sizeof(feature->name) - 1);
    feature->value = value;
    feature->type = type;
    feature->is_normalized = false;
    feature->is_encoded = false;

    return AU_SUCCESS;
}

/**
 * @brief Get feature value by name
 */
au_error_t au_get_feature_value(const feature_vector_t *features, const char *name, float *value) {
    for (size_t i = 0; i < features->num_features; i++) {
        if (strcmp(features->features[i].name, name) == 0) {
            *value = features->features[i].value;
            return AU_SUCCESS;
        }
    }

    return AU_ERROR_NOT_FOUND;
}

/**
 * @brief Create feature cache
 */
feature_cache_t *au_create_feature_cache(size_t capacity) {
    feature_cache_t *cache = calloc(1, sizeof(feature_cache_t));
    if (cache == NULL) {
        return NULL;
    }

    cache->capacity = capacity;
    cache->entries = calloc(capacity, sizeof(feature_cache_entry_t));
    if (cache->entries == NULL) {
        free(cache);
        return NULL;
    }

    if (pthread_mutex_init(&cache->mutex, NULL) != 0) {
        free(cache->entries);
        free(cache);
        return NULL;
    }

    return cache;
}

/**
 * @brief Destroy feature cache
 */
void au_destroy_feature_cache(feature_cache_t *cache) {
    if (cache == NULL) {
        return;
    }

    pthread_mutex_destroy(&cache->mutex);
    free(cache->entries);
    free(cache);
}

/**
 * @brief Cache features
 */
au_error_t au_cache_features(feature_cache_t *cache, uint64_t application_id,
                           const feature_vector_t *features) {
    if (cache == NULL || features == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&cache->mutex);

    // Simple hash-based caching (linear search for small cache)
    size_t slot = application_id % cache->capacity;

    // Check if already cached
    if (cache->entries[slot].application_id == application_id) {
        // Update existing entry
        memcpy(&cache->entries[slot].features, features, sizeof(feature_vector_t));
        cache->entries[slot].timestamp = time(NULL);
    } else {
        // Add new entry
        cache->entries[slot].application_id = application_id;
        memcpy(&cache->entries[slot].features, features, sizeof(feature_vector_t));
        cache->entries[slot].timestamp = time(NULL);
        cache->size++;
    }

    pthread_mutex_unlock(&cache->mutex);
    return AU_SUCCESS;
}

/**
 * @brief Get cached features
 */
au_error_t au_get_cached_features(feature_cache_t *cache, uint64_t application_id,
                                feature_vector_t *features) {
    if (cache == NULL || features == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&cache->mutex);

    size_t slot = application_id % cache->capacity;

    if (cache->entries[slot].application_id == application_id) {
        // Check if cache entry is still valid (not expired)
        time_t now = time(NULL);
        if (now - cache->entries[slot].timestamp < 3600) { // 1 hour TTL
            memcpy(features, &cache->entries[slot].features, sizeof(feature_vector_t));
            pthread_mutex_unlock(&cache->mutex);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&cache->mutex);
    return AU_ERROR_NOT_FOUND;
}