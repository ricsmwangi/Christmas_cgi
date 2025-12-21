/**
 * @file test_ai_underwriting.c
 * @brief Simple test program for AI Underwriting Engine
 *
 * Demonstrates the core functionality of the AI underwriting engine
 * with a simplified implementation.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Simplified data structures
typedef enum {
    AU_SUCCESS = 0,
    AU_ERROR_INVALID_INPUT = -1,
    AU_ERROR_MEMORY_ALLOCATION = -2
} au_error_t;

typedef enum {
    DECISION_APPROVED = 1,
    DECISION_DENIED = 2,
    DECISION_REFERRED = 3
} decision_type_t;

typedef enum {
    RISK_LOW = 1,
    RISK_MEDIUM = 2,
    RISK_HIGH = 3
} risk_level_t;

typedef enum {
    PRODUCT_AUTO = 1,
    PRODUCT_HOME = 2,
    PRODUCT_LIFE = 3
} product_type_t;

// Feature structure
typedef struct {
    char name[64];
    float value;
} feature_t;

// Feature vector
typedef struct {
    size_t num_features;
    feature_t features[256];
} feature_vector_t;

// Underwriting application
typedef struct {
    unsigned long long id;
    product_type_t product_type;
    char applicant_name[256];
    double requested_coverage;
    double requested_deductible;
    char date_of_birth[16];
    char gender[16];

    union {
        struct {
            int vehicle_year;
            double vehicle_value;
            int annual_mileage;
        } auto_data;
    } product_data;
} underwriting_application_t;

// Underwriting decision
typedef struct {
    unsigned long long application_id;
    decision_type_t decision;
    risk_level_t risk_level;
    double risk_score;
    double calculated_premium;
    char explanation[1024];
} underwriting_decision_t;

// Function prototypes
au_error_t extract_features(const underwriting_application_t *application, feature_vector_t *features);
double calculate_risk_score(const feature_vector_t *features);
double calculate_premium(const underwriting_application_t *application, double risk_score);
decision_type_t make_decision(double risk_score);
const char *decision_string(decision_type_t decision);
const char *risk_level_string(risk_level_t risk);

// Main test function
int main() {
    printf("AI Underwriting Engine - Test Program\n");
    printf("=====================================\n\n");

    // Create test application
    underwriting_application_t application = {0};
    application.id = 1;
    application.product_type = PRODUCT_AUTO;
    strcpy(application.applicant_name, "John Doe");
    application.requested_coverage = 500000.0;
    application.requested_deductible = 1000.0;
    strcpy(application.date_of_birth, "1990");
    strcpy(application.gender, "M");

    // Auto-specific data
    application.product_data.auto_data.vehicle_year = 2018;
    application.product_data.auto_data.vehicle_value = 25000.0;
    application.product_data.auto_data.annual_mileage = 12000;

    printf("Test Application:\n");
    printf("  ID: %llu\n", application.id);
    printf("  Name: %s\n", application.applicant_name);
    printf("  Product: Auto Insurance\n");
    printf("  Coverage: $%.0f\n", application.requested_coverage);
    printf("  Deductible: $%.0f\n", application.requested_deductible);
    printf("  Vehicle: %d model, $%.0f value, %d miles/year\n\n",
           application.product_data.auto_data.vehicle_year,
           application.product_data.auto_data.vehicle_value,
           application.product_data.auto_data.annual_mileage);

    // Extract features
    feature_vector_t features;
    if (extract_features(&application, &features) != AU_SUCCESS) {
        printf("ERROR: Feature extraction failed\n");
        return 1;
    }

    printf("Extracted Features (%zu):\n", features.num_features);
    for (size_t i = 0; i < features.num_features; i++) {
        printf("  %s: %.3f\n", features.features[i].name, features.features[i].value);
    }
    printf("\n");

    // Calculate risk score
    double risk_score = calculate_risk_score(&features);
    printf("Risk Assessment:\n");
    printf("  Risk Score: %.3f (%.1f%%)\n", risk_score, risk_score * 100);

    // Determine risk level
    risk_level_t risk_level;
    if (risk_score < 0.3) risk_level = RISK_LOW;
    else if (risk_score < 0.7) risk_level = RISK_MEDIUM;
    else risk_level = RISK_HIGH;

    printf("  Risk Level: %s\n\n", risk_level_string(risk_level));

    // Make decision
    decision_type_t decision = make_decision(risk_score);
    printf("Underwriting Decision:\n");
    printf("  Decision: %s\n", decision_string(decision));

    // Calculate premium
    double premium = calculate_premium(&application, risk_score);
    printf("  Calculated Premium: $%.2f\n", premium);

    // Generate explanation
    printf("\nExplanation:\n");
    printf("  Application for $%.0f auto insurance coverage.\n", application.requested_coverage);
    printf("  Risk assessment shows %s risk profile (score: %.1f%%).\n",
           risk_level_string(risk_level), risk_score * 100);
    printf("  Decision: %s with premium of $%.2f.\n",
           decision_string(decision), premium);

    printf("\nAI Underwriting Engine test completed successfully!\n");
    return 0;
}

// Extract features from application
au_error_t extract_features(const underwriting_application_t *application, feature_vector_t *features) {
    memset(features, 0, sizeof(feature_vector_t));

    size_t idx = 0;

    // Age feature
    int birth_year = atoi(application->date_of_birth);
    if (birth_year > 0) {
        int current_year = 2024; // Simplified
        float age = (float)(current_year - birth_year);
        strcpy(features->features[idx].name, "applicant_age");
        features->features[idx].value = age / 100.0f; // Normalize
        idx++;
    }

    // Coverage amount (normalized)
    strcpy(features->features[idx].name, "coverage_amount");
    features->features[idx].value = application->requested_coverage / 1000000.0f;
    idx++;

    // Deductible ratio
    strcpy(features->features[idx].name, "deductible_ratio");
    features->features[idx].value = application->requested_deductible / application->requested_coverage;
    idx++;

    // Product-specific features
    if (application->product_type == PRODUCT_AUTO) {
        // Vehicle age
        int current_year = 2024;
        float vehicle_age = (float)(current_year - application->product_data.auto_data.vehicle_year);
        strcpy(features->features[idx].name, "vehicle_age");
        features->features[idx].value = vehicle_age / 20.0f; // Normalize
        idx++;

        // Vehicle value
        strcpy(features->features[idx].name, "vehicle_value");
        features->features[idx].value = application->product_data.auto_data.vehicle_value / 50000.0f;
        idx++;

        // Annual mileage
        strcpy(features->features[idx].name, "annual_mileage");
        features->features[idx].value = application->product_data.auto_data.annual_mileage / 15000.0f;
        idx++;
    }

    features->num_features = idx;
    return AU_SUCCESS;
}

// Calculate risk score using simple ML-like logic
double calculate_risk_score(const feature_vector_t *features) {
    double risk_score = 0.5; // Base risk

    for (size_t i = 0; i < features->num_features; i++) {
        const feature_t *feature = &features->features[i];

        if (strcmp(feature->name, "applicant_age") == 0) {
            // Younger applicants have higher risk
            float age = feature->value * 100.0f;
            if (age < 25) risk_score += 0.2;
            else if (age > 60) risk_score -= 0.1;
        } else if (strcmp(feature->name, "vehicle_age") == 0) {
            // Older vehicles have higher risk
            float vehicle_age = feature->value * 20.0f;
            if (vehicle_age > 10) risk_score += 0.15;
        } else if (strcmp(feature->name, "annual_mileage") == 0) {
            // Higher mileage increases risk
            float mileage_factor = feature->value;
            if (mileage_factor > 1.0) risk_score += 0.1;
        } else if (strcmp(feature->name, "deductible_ratio") == 0) {
            // Higher deductible reduces risk (better risk management)
            if (feature->value > 0.01) risk_score -= 0.05;
        }
    }

    // Clamp to [0, 1]
    if (risk_score < 0.0) risk_score = 0.0;
    if (risk_score > 1.0) risk_score = 1.0;

    return risk_score;
}

// Calculate premium based on risk and coverage
double calculate_premium(const underwriting_application_t *application, double risk_score) {
    // Base premium as percentage of coverage
    double base_premium = application->requested_coverage * 0.001; // 0.1%

    // Risk adjustment
    double risk_multiplier = 1.0 + (risk_score - 0.5) * 0.5; // +/- 50% based on risk

    // Deductible discount
    double deductible_factor = 1.0;
    if (application->requested_deductible > 0) {
        deductible_factor = 0.9; // 10% discount for deductible
    }

    return base_premium * risk_multiplier * deductible_factor;
}

// Make underwriting decision based on risk score
decision_type_t make_decision(double risk_score) {
    if (risk_score > 0.8) {
        return DECISION_DENIED;
    } else if (risk_score > 0.6) {
        return DECISION_REFERRED;
    } else {
        return DECISION_APPROVED;
    }
}

// Utility functions
const char *decision_string(decision_type_t decision) {
    switch (decision) {
        case DECISION_APPROVED: return "APPROVED";
        case DECISION_DENIED: return "DENIED";
        case DECISION_REFERRED: return "REFERRED";
        default: return "UNKNOWN";
    }
}

const char *risk_level_string(risk_level_t risk) {
    switch (risk) {
        case RISK_LOW: return "LOW";
        case RISK_MEDIUM: return "MEDIUM";
        case RISK_HIGH: return "HIGH";
        default: return "UNKNOWN";
    }
}