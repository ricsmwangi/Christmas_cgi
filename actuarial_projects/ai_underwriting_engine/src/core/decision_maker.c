/**
 * @file decision_maker.c
 * @brief Decision Making Orchestration
 *
 * Orchestrates the complete underwriting decision process including
 * feature extraction, model inference, rule evaluation, and final decision.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <string.h>
#include <math.h>

// Forward declarations
static au_error_t au_apply_business_rules(const underwriting_application_t *application,
                                        underwriting_decision_t *decision);
static double au_calculate_premium_simple(const underwriting_application_t *application,
                                        double risk_score);
static void au_generate_decision_explanation(const underwriting_application_t *application,
                                           underwriting_decision_t *decision,
                                           double risk_score);

// External declarations (from engine.c)
extern bool engine_initialized;
extern bool engine_running;

// Process a single underwriting application
au_error_t au_process_application(const underwriting_application_t *application,
                                underwriting_decision_t *decision) {
    if (!application || !decision) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (!engine_running) {
        au_log_error("Engine not running");
        return AU_ERROR_INVALID_INPUT;
    }

    au_log_info("Processing application %llu for product %s",
               (unsigned long long)application->id,
               au_product_string(application->product_type));

    // Initialize decision
    memset(decision, 0, sizeof(underwriting_decision_t));
    decision->application_id = application->id;
    decision->decision_time = time(NULL);

    // Step 1: Extract features using the feature engineering pipeline
    // Initialize feature engineering if needed
    static bool fe_initialized = false;
    if (!fe_initialized) {
        if (au_initialize_feature_engineering() != AU_SUCCESS) {
            au_log_error("Failed to initialize feature engineering");
            decision->decision = DECISION_DENIED;
            strcpy(decision->explanation, "Feature engineering initialization failed");
            return AU_ERROR_CONFIGURATION_ERROR;
        }
        fe_initialized = true;
    }

    feature_vector_t features;
    au_error_t result = au_extract_features(application, &features);
    if (result != AU_SUCCESS) {
        au_log_error("Feature extraction failed for application %llu",
                    (unsigned long long)application->id);
        decision->decision = DECISION_DENIED;
        strcpy(decision->explanation, "Feature extraction failed");
        return result;
    }

    // Step 2: Run inference using the AI models
    double risk_score = 0.0;
    double confidence = 0.0;

    result = au_run_inference(0, &features, &risk_score, &confidence); // Use model ID 0
    if (result != AU_SUCCESS) {
        au_log_warning("Model inference failed, using rule-based decision");
        risk_score = 0.5; // Default medium risk
        confidence = 0.5;
    }

    decision->risk_score = risk_score;
    decision->confidence_score = confidence;

    // Step 3: Apply risk level classification
    if (risk_score < 0.3) {
        decision->risk_level = RISK_LOW;
    } else if (risk_score < 0.7) {
        decision->risk_level = RISK_MEDIUM;
    } else {
        decision->risk_level = RISK_HIGH;
    }

    // Step 4: Apply business rules
    result = au_apply_business_rules(application, decision);
    if (result != AU_SUCCESS) {
        au_log_warning("Business rule application failed");
    }

    // Step 5: Calculate premium
    decision->calculated_premium = au_calculate_premium_simple(application, risk_score);

    // Step 6: Generate explanation
    au_generate_decision_explanation(application, decision, risk_score);

    au_log_info("Application %llu processed: %s (risk=%.3f, premium=$%.2f)",
               (unsigned long long)application->id,
               au_decision_string(decision->decision),
               decision->risk_score,
               decision->calculated_premium);

    return AU_SUCCESS;
}

// Process batch applications
au_error_t au_process_batch_applications(const underwriting_application_t *applications,
                                       size_t num_applications,
                                       underwriting_decision_t *decisions) {
    if (!applications || !decisions || num_applications == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    au_log_info("Processing batch of %zu applications", num_applications);

    au_error_t final_result = AU_SUCCESS;

    for (size_t i = 0; i < num_applications; i++) {
        au_error_t result = au_process_application(&applications[i], &decisions[i]);
        if (result != AU_SUCCESS) {
            final_result = result;  // Continue processing but track error
        }
    }

    au_log_info("Batch processing complete");
    return final_result;
}

// Apply business rules (simplified)
au_error_t au_apply_business_rules(const underwriting_application_t *application,
                                  underwriting_decision_t *decision) {
    // Simple rule: High coverage amounts get referred for manual review
    if (application->requested_coverage > 500000.0) {
        decision->decision = DECISION_REFERRED;
        strcpy(decision->explanation, "High coverage amount requires manual review");
        return AU_SUCCESS;
    }

    // Rule: Very high risk scores get denied
    if (decision->risk_score > 0.8) {
        decision->decision = DECISION_DENIED;
        strcpy(decision->explanation, "Risk score too high for automatic approval");
        return AU_SUCCESS;
    }

    // Rule: Low risk scores get approved
    if (decision->risk_score < 0.4) {
        decision->decision = DECISION_APPROVED;
        strcpy(decision->explanation, "Low risk application approved");
        return AU_SUCCESS;
    }

    // Default: Refer for manual review
    decision->decision = DECISION_REFERRED;
    strcpy(decision->explanation, "Application requires manual underwriting review");

    return AU_SUCCESS;
}

// Calculate premium (simplified)
double au_calculate_premium_simple(const underwriting_application_t *application, double risk_score) {
    // Base premium calculation
    double base_premium = application->requested_coverage * 0.001; // 0.1% of coverage

    // Risk adjustment
    double risk_multiplier = 1.0 + (risk_score - 0.5) * 0.5; // +/- 50% based on risk

    // Deductible adjustment
    double deductible_factor = 1.0;
    if (application->requested_deductible > 0) {
        deductible_factor = 0.9; // 10% discount for having deductible
    }

    return base_premium * risk_multiplier * deductible_factor;
}

// Generate decision explanation
void au_generate_decision_explanation(const underwriting_application_t *application,
                                    underwriting_decision_t *decision,
                                    double risk_score) {
    char explanation[1024] = "";

    sprintf(explanation, "Application for $%.0f coverage with $%.0f deductible. ",
            application->requested_coverage, application->requested_deductible);

    switch (decision->risk_level) {
        case RISK_LOW:
            strcat(explanation, "Low risk profile. ");
            break;
        case RISK_MEDIUM:
            strcat(explanation, "Medium risk profile. ");
            break;
        case RISK_HIGH:
        case RISK_VERY_HIGH:
            strcat(explanation, "High risk profile. ");
            break;
    }

    sprintf(explanation + strlen(explanation),
            "Risk score: %.1f%%. Confidence: %.1f%%. ",
            risk_score * 100, decision->confidence_score * 100);

    switch (decision->decision) {
        case DECISION_APPROVED:
            strcat(explanation, "Automatically approved.");
            break;
        case DECISION_DENIED:
            strcat(explanation, "Automatically denied.");
            break;
        case DECISION_REFERRED:
            strcat(explanation, "Referred for manual review.");
            break;
        default:
            strcat(explanation, "Pending review.");
            break;
    }

    strcpy(decision->explanation, explanation);
}