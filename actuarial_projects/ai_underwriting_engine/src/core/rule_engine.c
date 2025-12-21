/**
 * @file rule_engine.c
 * @brief Business Rules Engine
 *
 * Implements a flexible rule engine for underwriting business logic,
 * supporting complex conditions and actions.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <string.h>

// Forward declarations for static functions
static void au_sort_rules_by_priority(void);
static bool au_evaluate_rule_condition_detailed(const underwriting_rule_t *rule,
                                               const underwriting_application_t *application,
                                               const underwriting_decision_t *decision);
static void au_apply_rule_action_detailed(const underwriting_rule_t *rule,
                                         underwriting_decision_t *decision);
static bool au_rule_stops_evaluation(const underwriting_rule_t *rule);
static double au_calculate_age_from_application(const underwriting_application_t *application);

// Add a new rule to the engine
au_error_t au_add_rule(const underwriting_rule_t *rule) {
    if (!rule) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&rule_mutex);

    if (num_rules >= MAX_RULES) {
        pthread_mutex_unlock(&rule_mutex);
        au_log_error("Maximum number of rules reached (%d)", MAX_RULES);
        return AU_ERROR_INVALID_INPUT;
    }

    // Check for duplicate rule ID
    for (size_t i = 0; i < num_rules; i++) {
        if (rule_registry[i].id == rule->id) {
            pthread_mutex_unlock(&rule_mutex);
            au_log_error("Rule with ID %u already exists", rule->id);
            return AU_ERROR_INVALID_INPUT;
        }
    }

    // Add rule
    rule_registry[num_rules] = *rule;
    num_rules++;

    pthread_mutex_unlock(&rule_mutex);

    au_log_info("Added rule '%s' (ID: %u)", rule->name, rule->id);
    return AU_SUCCESS;
}

// Remove a rule from the engine
au_error_t au_remove_rule(rule_id_t rule_id) {
    pthread_mutex_lock(&rule_mutex);

    for (size_t i = 0; i < num_rules; i++) {
        if (rule_registry[i].id == rule_id) {
            // Shift remaining rules
            for (size_t j = i; j < num_rules - 1; j++) {
                rule_registry[j] = rule_registry[j + 1];
            }
            num_rules--;

            pthread_mutex_unlock(&rule_mutex);
            au_log_info("Removed rule ID %u", rule_id);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&rule_mutex);
    au_log_error("Rule ID %u not found", rule_id);
    return AU_ERROR_INVALID_INPUT;
}

// Update an existing rule
au_error_t au_update_rule(const underwriting_rule_t *rule) {
    if (!rule) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&rule_mutex);

    for (size_t i = 0; i < num_rules; i++) {
        if (rule_registry[i].id == rule->id) {
            rule_registry[i] = *rule;

            pthread_mutex_unlock(&rule_mutex);
            au_log_info("Updated rule '%s' (ID: %u)", rule->name, rule->id);
            return AU_SUCCESS;
        }
    }

    pthread_mutex_unlock(&rule_mutex);
    au_log_error("Rule ID %u not found for update", rule->id);
    return AU_ERROR_INVALID_INPUT;
}

// List all rules
au_error_t au_list_rules(underwriting_rule_t *rules, size_t *num_rules_out) {
    if (!rules || !num_rules_out) {
        return AU_ERROR_INVALID_INPUT;
    }

    pthread_mutex_lock(&rule_mutex);

    size_t copy_count = *num_rules_out < num_rules ? *num_rules_out : num_rules;

    for (size_t i = 0; i < copy_count; i++) {
        rules[i] = rule_registry[i];
    }

    *num_rules_out = copy_count;

    pthread_mutex_unlock(&rule_mutex);

    return AU_SUCCESS;
}

// Evaluate rules for an application (internal function)
au_error_t au_evaluate_rules_for_application(const underwriting_application_t *application,
                                           underwriting_decision_t *decision) {
    if (!application || !decision) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (!current_config->enable_rule_engine) {
        return AU_SUCCESS;  // Rules disabled
    }

    pthread_mutex_lock(&rule_mutex);

    // Sort rules by priority (simple bubble sort for now)
    au_sort_rules_by_priority();

    // Evaluate rules in priority order
    bool rule_applied = false;

    for (size_t i = 0; i < num_rules; i++) {
        underwriting_rule_t *rule = &rule_registry[i];

        if (!rule->enabled || rule->product_type != application->product_type) {
            continue;
        }

        // Evaluate rule condition
        if (au_evaluate_rule_condition_detailed(rule, application, decision)) {
            // Apply rule action
            au_apply_rule_action_detailed(rule, decision);

            au_log_info("Applied rule '%s' to application %llu",
                       rule->name, (unsigned long long)application->id);

            rule_applied = true;

            // Check if this rule should stop further evaluation
            if (au_rule_stops_evaluation(rule)) {
                break;
            }
        }
    }

    pthread_mutex_unlock(&rule_mutex);

    if (rule_applied) {
        au_log_debug("Rules applied to application %llu",
                    (unsigned long long)application->id);
    }

    return AU_SUCCESS;
}

// Sort rules by priority (internal function)
static void au_sort_rules_by_priority(void) {
    // Simple bubble sort by priority (highest first)
    for (size_t i = 0; i < num_rules - 1; i++) {
        for (size_t j = 0; j < num_rules - i - 1; j++) {
            if (rule_registry[j].priority < rule_registry[j + 1].priority) {
                // Swap
                underwriting_rule_t temp = rule_registry[j];
                rule_registry[j] = rule_registry[j + 1];
                rule_registry[j + 1] = temp;
            }
        }
    }
}

// Detailed rule condition evaluation
static bool au_evaluate_rule_condition_detailed(const underwriting_rule_t *rule,
                                              const underwriting_application_t *application,
                                              const underwriting_decision_t *decision) {
    // Parse and evaluate the condition expression
    // This is a simplified implementation - a full rule engine would use
    // a proper expression parser

    const char *condition = rule->condition;

    // Age-based conditions
    if (strstr(condition, "age")) {
        double age = au_calculate_age_from_application(application);

        if (strstr(condition, "age <")) {
            int threshold = atoi(strstr(condition, "age <") + 5);
            if (age < threshold) return true;
        } else if (strstr(condition, "age >")) {
            int threshold = atoi(strstr(condition, "age >") + 5);
            if (age > threshold) return true;
        } else if (strstr(condition, "age ==")) {
            int threshold = atoi(strstr(condition, "age ==") + 6);
            if ((int)age == threshold) return true;
        }
    }

    // Risk score conditions
    if (strstr(condition, "risk_score") && decision) {
        if (strstr(condition, "risk_score >")) {
            double threshold = atof(strstr(condition, "risk_score >") + 13);
            if (decision->risk_score > threshold) return true;
        } else if (strstr(condition, "risk_score <")) {
            double threshold = atof(strstr(condition, "risk_score <") + 13);
            if (decision->risk_score < threshold) return true;
        }
    }

    // Coverage amount conditions
    if (strstr(condition, "coverage")) {
        if (strstr(condition, "coverage >")) {
            double threshold = atof(strstr(condition, "coverage >") + 11);
            if (application->requested_coverage > threshold) return true;
        } else if (strstr(condition, "coverage <")) {
            double threshold = atof(strstr(condition, "coverage <") + 11);
            if (application->requested_coverage < threshold) return true;
        }
    }

    // Vehicle-specific conditions
    if (application->product_type == PRODUCT_AUTO) {
        if (strstr(condition, "vehicle_age")) {
            double vehicle_age = 2024 - application->product_data.auto_data.vehicle_year;
            if (strstr(condition, "vehicle_age >")) {
                int threshold = atoi(strstr(condition, "vehicle_age >") + 14);
                if (vehicle_age > threshold) return true;
            }
        }

        if (strstr(condition, "mileage")) {
            double mileage = application->product_data.auto_data.annual_mileage;
            if (strstr(condition, "mileage >")) {
                int threshold = atoi(strstr(condition, "mileage >") + 10);
                if (mileage > threshold) return true;
            }
        }
    }

    // Location-based conditions
    if (strstr(condition, "state")) {
        if (strstr(condition, "state ==")) {
            const char *state_condition = strstr(condition, "state ==") + 9;
            char state[3] = {0};
            strncpy(state, state_condition, 2);
            if (strcmp(application->state, state) == 0) return true;
        }
    }

    return false;
}

// Apply detailed rule action
static void au_apply_rule_action_detailed(const underwriting_rule_t *rule,
                                        underwriting_decision_t *decision) {
    const char *action = rule->action;

    if (strcmp(action, "deny") == 0) {
        decision->decision = DECISION_DENIED;
        snprintf(decision->explanation, sizeof(decision->explanation),
                "Denied by rule: %s", rule->description);
    } else if (strcmp(action, "approve") == 0) {
        decision->decision = DECISION_APPROVED;
        snprintf(decision->explanation, sizeof(decision->explanation),
                "Approved by rule: %s", rule->description);
    } else if (strcmp(action, "refer") == 0) {
        decision->decision = DECISION_REFERRED;
        snprintf(decision->explanation, sizeof(decision->explanation),
                "Referred by rule: %s", rule->description);
    } else if (strcmp(action, "flag") == 0) {
        // Flag for manual review but don't change decision
        au_log_warning("Application flagged by rule: %s", rule->description);
    } else if (strstr(action, "premium")) {
        // Premium adjustment
        if (strstr(action, "premium *")) {
            double multiplier = atof(strstr(action, "premium *") + 10);
            decision->calculated_premium *= multiplier;
            au_log_info("Premium adjusted by rule: %s (multiplier: %.2f)",
                       rule->description, multiplier);
        }
    }
}

// Check if rule stops further evaluation
static bool au_rule_stops_evaluation(const underwriting_rule_t *rule) {
    // Rules that deny or approve applications stop further evaluation
    return strcmp(rule->action, "deny") == 0 ||
           strcmp(rule->action, "approve") == 0;
}

// Calculate age from application (helper)
static double au_calculate_age_from_application(const underwriting_application_t *application) {
    if (strlen(application->date_of_birth) >= 4) {
        int birth_year = atoi(application->date_of_birth);
        if (birth_year > 1900 && birth_year <= 2024) {
            return 2024 - birth_year;
        }
    }
    return 30.0;  // Default age
}

// Validate rule syntax
au_error_t au_validate_rule(const underwriting_rule_t *rule) {
    if (!rule) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Basic validation
    if (strlen(rule->name) == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (strlen(rule->condition) == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    if (strlen(rule->action) == 0) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Validate condition syntax (basic check)
    const char *valid_conditions[] = {
        "age", "risk_score", "coverage", "vehicle_age", "mileage", "state", NULL
    };

    bool has_valid_condition = false;
    for (int i = 0; valid_conditions[i]; i++) {
        if (strstr(rule->condition, valid_conditions[i])) {
            has_valid_condition = true;
            break;
        }
    }

    if (!has_valid_condition) {
        au_log_error("Rule '%s' has invalid condition syntax", rule->name);
        return AU_ERROR_INVALID_INPUT;
    }

    // Validate action syntax
    const char *valid_actions[] = {
        "deny", "approve", "refer", "flag", "premium", NULL
    };

    bool has_valid_action = false;
    for (int i = 0; valid_actions[i]; i++) {
        if (strstr(rule->action, valid_actions[i])) {
            has_valid_action = true;
            break;
        }
    }

    if (!has_valid_action) {
        au_log_error("Rule '%s' has invalid action syntax", rule->name);
        return AU_ERROR_INVALID_INPUT;
    }

    return AU_SUCCESS;
}

// Get rule statistics
au_error_t au_get_rule_statistics(rule_id_t rule_id, size_t *applications_evaluated,
                                size_t *applications_matched) {
    // Placeholder - would track rule execution statistics
    (void)rule_id;
    *applications_evaluated = 0;
    *applications_matched = 0;
    return AU_SUCCESS;
}