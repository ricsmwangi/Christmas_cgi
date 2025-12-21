/**
 * @file model_manager.c
 * @brief ML Model Management
 *
 * Manages loading, caching, and lifecycle of machine learning models
 * for real-time inference in underwriting decisions.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <string.h>
#include <dirent.h>

// Forward declarations for static functions
static au_error_t au_load_model_metadata(const char *model_path, model_metadata_t *metadata);
static void *au_load_model_binary(const char *model_path);
static void au_free_model_handle(void *model_handle);
static size_t au_estimate_model_memory(const model_metadata_t *metadata);
static model_id_t au_generate_model_id(void);
static au_error_t au_run_model_inference_internal(const model_instance_t *model,
                                                const feature_vector_t *features,
                                                double *prediction,
                                                double *confidence);

static model_instance_t loaded_models[MAX_MODELS];
static size_t num_loaded_models = 0;
static pthread_mutex_t loaded_models_mutex = PTHREAD_MUTEX_INITIALIZER;

// Load a model from file
au_error_t au_load_model(const char *model_path, model_id_t *model_id) {
    if (!model_path || !model_id) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&loaded_models_mutex);

    if (num_loaded_models >= MAX_MODELS) {
        pthread_mutex_unlock(&loaded_models_mutex);
        au_log_error("Maximum number of models reached (%d)", MAX_MODELS);
        return AU_ERROR_INVALID_INPUT;
    }

    // Generate model ID
    *model_id = au_generate_model_id();

    // Check if model already loaded
    for (size_t i = 0; i < num_loaded_models; i++) {
        if (strcmp(loaded_models[i].metadata.model_path, model_path) == 0) {
            *model_id = loaded_models[i].id;
            pthread_mutex_unlock(&loaded_models_mutex);
            au_log_info("Model already loaded: %s (ID: %u)", model_path, *model_id);
            return AU_SUCCESS;
        }
    }

    // Load model metadata
    model_metadata_t metadata;
    au_error_t result = au_load_model_metadata(model_path, &metadata);
    if (result != AU_SUCCESS) {
        pthread_mutex_unlock(&loaded_models_mutex);
        return result;
    }

    metadata.id = *model_id;
    strcpy(metadata.model_path, model_path);

    // Load actual model (placeholder)
    void *model_handle = au_load_model_binary(model_path);
    if (!model_handle) {
        pthread_mutex_unlock(&loaded_models_mutex);
        au_log_error("Failed to load model binary: %s", model_path);
        return AU_ERROR_MODEL_LOAD_FAILED;
    }

    // Add to loaded models
    loaded_models[num_loaded_models].id = *model_id;
    loaded_models[num_loaded_models].metadata = metadata;
    loaded_models[num_loaded_models].model_handle = model_handle;
    loaded_models[num_loaded_models].load_time = time(NULL);
    loaded_models[num_loaded_models].memory_usage = au_estimate_model_memory(&metadata);
    loaded_models[num_loaded_models].is_loaded = true;

    num_loaded_models++;

    pthread_mutex_unlock(&loaded_models_mutex);

    au_log_info("Loaded model '%s' (ID: %u, type: %s)",
               metadata.name, *model_id, au_product_string(metadata.product_type));

    return AU_SUCCESS;
}

// Unload a model
au_error_t au_unload_model(model_id_t model_id) {
    pthread_mutex_lock(&loaded_models_mutex);

    for (size_t i = 0; i < num_loaded_models; i++) {
        if (loaded_models[i].id == model_id) {
            // Cleanup model resources
            if (loaded_models[i].model_handle) {
                au_free_model_handle(loaded_models[i].model_handle);
                loaded_models[i].model_handle = NULL;
            }

            // Remove from array
            for (size_t j = i; j < num_loaded_models - 1; j++) {
                loaded_models[j] = loaded_models[j + 1];
            }
            num_loaded_models--;

            pthread_mutex_unlock(&loaded_models_mutex);
            au_log_info("Unloaded model ID %u", model_id);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&loaded_models_mutex);
    au_log_error("Model ID %u not found", model_id);
    return AU_ERROR_INVALID_INPUT;
}

// Get model information
au_error_t au_get_model_info(model_id_t model_id, model_metadata_t *metadata) {
    if (!metadata) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&loaded_models_mutex);

    for (size_t i = 0; i < num_loaded_models; i++) {
        if (loaded_models[i].id == model_id) {
            *metadata = loaded_models[i].metadata;

            pthread_mutex_unlock(&loaded_models_mutex);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&loaded_models_mutex);
    return AU_ERROR_INVALID_INPUT;
}

// List all loaded models
au_error_t au_list_models(model_metadata_t *models, size_t *num_models_out) {
    if (!models || !num_models_out) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&loaded_models_mutex);

    size_t copy_count = *num_models_out < num_loaded_models ? *num_models_out : num_loaded_models;

    for (size_t i = 0; i < copy_count; i++) {
        models[i] = loaded_models[i].metadata;
    }

    *num_models_out = copy_count;

    pthread_mutex_unlock(&loaded_models_mutex);

    return AU_SUCCESS;
}

// Run inference on a model
au_error_t au_run_inference(model_id_t model_id,
                          const feature_vector_t *features,
                          double *prediction,
                          double *confidence) {
    if (!features || !prediction || !confidence) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&loaded_models_mutex);

    // Find model
    model_instance_t *model = NULL;
    for (size_t i = 0; i < num_loaded_models; i++) {
        if (loaded_models[i].id == model_id) {
            model = &loaded_models[i];
            break;
        }
    }

    if (!model || !model->is_loaded) {
        pthread_mutex_unlock(&loaded_models_mutex);
        au_log_error("Model ID %u not found or not loaded", model_id);
        return AU_ERROR_MODEL_LOAD_FAILED;
    }

    // Validate feature count
    if (features->num_features != model->metadata.input_features) {
        pthread_mutex_unlock(&loaded_models_mutex);
        au_log_error("Feature count mismatch for model %u: expected %zu, got %zu",
                    model_id, model->metadata.input_features, features->num_features);
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_unlock(&loaded_models_mutex);

    // Run inference (placeholder implementation)
    au_error_t result = au_run_model_inference_internal(model, features, prediction, confidence);

    if (result == AU_SUCCESS) {
        au_log_debug("Inference completed for model %u: prediction=%.3f, confidence=%.3f",
                    model_id, *prediction, *confidence);
    }

    return result;
}

// Load model metadata from file (placeholder)
static au_error_t au_load_model_metadata(const char *model_path, model_metadata_t *metadata) {
    // In a real implementation, this would read metadata from a config file
    // or extract it from the model file itself

    memset(metadata, 0, sizeof(model_metadata_t));

    // Extract model name from path
    const char *filename = strrchr(model_path, '/');
    if (filename) {
        filename++;  // Skip the '/'
    } else {
        filename = model_path;
    }

    // Remove extension
    char name[128];
    strcpy(name, filename);
    char *dot = strrchr(name, '.');
    if (dot) *dot = '\0';

    strcpy(metadata->name, name);
    strcpy(metadata->version, "1.0");
    metadata->created_time = time(NULL);
    metadata->last_updated = time(NULL);
    metadata->accuracy_score = 0.85;  // Placeholder
    metadata->input_features = 20;    // Placeholder
    metadata->output_classes = 1;     // Regression for risk score

    // Determine product type from filename
    if (strstr(name, "auto")) {
        metadata->product_type = PRODUCT_AUTO;
    } else if (strstr(name, "home")) {
        metadata->product_type = PRODUCT_HOME;
    } else if (strstr(name, "life")) {
        metadata->product_type = PRODUCT_LIFE;
    } else {
        metadata->product_type = PRODUCT_AUTO;  // Default
    }

    return AU_SUCCESS;
}

// Load model binary (placeholder)
static void *au_load_model_binary(const char *model_path) {
    // In a real implementation, this would load TensorFlow, PyTorch, or ONNX models
    // For now, just return a dummy handle

    FILE *file = fopen(model_path, "rb");
    if (!file) {
        au_log_error("Cannot open model file: %s", model_path);
        return NULL;
    }

    // Get file size
    fseek(file, 0, SEEK_END);
    long file_size = ftell(file);
    fseek(file, 0, SEEK_SET);

    // Allocate memory for "model"
    void *model_handle = au_malloc(file_size + sizeof(size_t));
    if (!model_handle) {
        fclose(file);
        return NULL;
    }

    // Store size and read data
    *(size_t *)model_handle = file_size;
    size_t bytes_read = fread((char *)model_handle + sizeof(size_t), 1, file_size, file);
    fclose(file);

    if (bytes_read != (size_t)file_size) {
        au_free(model_handle);
        au_log_error("Failed to read model file completely");
        return NULL;
    }

    return model_handle;
}

// Free model handle
static void au_free_model_handle(void *model_handle) {
    if (model_handle) {
        au_free(model_handle);
    }
}

// Estimate model memory usage
static size_t au_estimate_model_memory(const model_metadata_t *metadata) {
    // Rough estimation: features * sizeof(float) * layers
    return metadata->input_features * sizeof(float) * 1000;  // 1KB per feature as estimate
}

// Generate unique model ID
static model_id_t au_generate_model_id(void) {
    static model_id_t next_id = 1;
    return next_id++;
}

// Internal inference implementation (placeholder)
static au_error_t au_run_model_inference_internal(const model_instance_t *model,
                                                const feature_vector_t *features,
                                                double *prediction,
                                                double *confidence) {
    // Placeholder inference logic
    // In a real implementation, this would call TensorFlow, PyTorch, or other ML framework

    // Simple weighted sum of features as placeholder
    double weighted_sum = 0.0;
    double total_weight = 0.0;

    for (size_t i = 0; i < features->num_features; i++) {
        double weight = 1.0;  // Equal weights for all features

        // Special weights for known features
        if (strcmp(features->features[i].name, "age") == 0) {
            weight = 0.8;
        } else if (strcmp(features->features[i].name, "vehicle_age") == 0) {
            weight = 0.6;
        } else if (strcmp(features->features[i].name, "annual_mileage") == 0) {
            weight = 0.4;
        }

        weighted_sum += features->features[i].value * weight;
        total_weight += weight;
    }

    if (total_weight > 0) {
        *prediction = weighted_sum / total_weight;
        // Normalize to [0, 1] range
        *prediction = *prediction / 100.0;  // Assume features are in 0-100 range
        if (*prediction > 1.0) *prediction = 1.0;
        if (*prediction < 0.0) *prediction = 0.0;
    } else {
        *prediction = 0.5;  // Default
    }

    // Calculate confidence based on feature completeness
    *confidence = (double)features->num_features / model->metadata.input_features;
    if (*confidence > 1.0) *confidence = 1.0;

    return AU_SUCCESS;
}

// Get model performance metrics
au_error_t au_get_model_performance(model_id_t model_id, double *accuracy,
                                  double *latency_ms, size_t *requests_served) {
    // Placeholder - would track real performance metrics
    (void)model_id;
    *accuracy = 0.85;
    *latency_ms = 15.0;
    *requests_served = 1000;
    return AU_SUCCESS;
}

// Reload model from disk
au_error_t au_reload_model(model_id_t model_id) {
    pthread_mutex_lock(&loaded_models_mutex);

    for (size_t i = 0; i < num_loaded_models; i++) {
        if (loaded_models[i].id == model_id) {
            const char *model_path = loaded_models[i].metadata.model_path;

            // Unload current model
            if (loaded_models[i].model_handle) {
                au_free_model_handle(loaded_models[i].model_handle);
            }

            // Reload model
            void *new_handle = au_load_model_binary(model_path);
            if (!new_handle) {
                pthread_mutex_unlock(&loaded_models_mutex);
                au_log_error("Failed to reload model %u", model_id);
                return AU_ERROR_MODEL_LOAD_FAILED;
            }

            loaded_models[i].model_handle = new_handle;
            loaded_models[i].load_time = time(NULL);

            pthread_mutex_unlock(&loaded_models_mutex);
            au_log_info("Reloaded model %u", model_id);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&loaded_models_mutex);
    return AU_ERROR_INVALID_INPUT;
}