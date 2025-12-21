/**
 * @file pandemic_loss_model.c
 * @brief Pandemic loss model implementation
 *
 * Implementation of actuarial loss modeling for pandemic scenarios.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#include "pandemic_loss_model.h"
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>

// Model structure
typedef struct {
    int num_regions;
    int num_sectors;
    double base_loss_rates[LINE_COUNT];  // Base loss rates by line
} pandemic_loss_model_impl_t;

// Base loss rates for different insurance lines (as % of portfolio value)
static const double BASE_LOSS_RATES[LINE_COUNT] = {
    [LINE_LIFE_INSURANCE] = 0.15,      // 15% mortality increase
    [LINE_HEALTH_INSURANCE] = 0.25,    // 25% morbidity increase
    [LINE_PROPERTY_INSURANCE] = 0.05,  // 5% business interruption
    [LINE_BUSINESS_INTERRUPTION] = 0.30, // 30% lockdown impact
    [LINE_EVENT_CANCELLATION] = 0.80,  // 80% event cancellations
};

// Calculate losses for each insurance line
int pandemic_loss_model_calculate(const pandemic_loss_params_t *params,
                                 const double *epidemic_data,
                                 pandemic_loss_result_t *results) {
    if (!params || !epidemic_data || !results) return -1;

    // epidemic_data[0] = mortality_rate, [1] = morbidity_rate, [2] = economic_impact

    double mortality_rate = epidemic_data[0];
    double morbidity_rate = epidemic_data[1];
    double economic_impact = epidemic_data[2];

    // Calculate gross losses by line
    results->total_losses[LINE_LIFE_INSURANCE] =
        params->insured_portfolio * BASE_LOSS_RATES[LINE_LIFE_INSURANCE] * mortality_rate;

    results->total_losses[LINE_HEALTH_INSURANCE] =
        params->insured_portfolio * BASE_LOSS_RATES[LINE_HEALTH_INSURANCE] * morbidity_rate;

    results->total_losses[LINE_PROPERTY_INSURANCE] =
        params->insured_portfolio * BASE_LOSS_RATES[LINE_PROPERTY_INSURANCE] * economic_impact;

    results->total_losses[LINE_BUSINESS_INTERRUPTION] =
        params->insured_portfolio * BASE_LOSS_RATES[LINE_BUSINESS_INTERRUPTION] * economic_impact;

    results->total_losses[LINE_EVENT_CANCELLATION] =
        params->insured_portfolio * BASE_LOSS_RATES[LINE_EVENT_CANCELLATION] * economic_impact;

    // Calculate net losses after reinsurance
    double reinsurance_recovery = params->reinsurance_coverage;
    for (int i = 0; i < LINE_COUNT; i++) {
        results->net_losses[i] = results->total_losses[i] * (1.0 - reinsurance_recovery);
    }

    // Aggregate results
    results->aggregate_loss = 0.0;
    for (int i = 0; i < LINE_COUNT; i++) {
        results->aggregate_loss += results->net_losses[i];
    }

    results->reinsurance_cost = results->aggregate_loss * 0.05;  // 5% reinsurance premium
    results->risk_adjusted_loss = results->aggregate_loss * 1.1; // 10% risk loading

    return 0;
}

// Validate parameters
int pandemic_loss_model_validate_params(const pandemic_loss_params_t *params) {
    if (!params) return -1;

    if (params->mortality_rate < 0.0 || params->mortality_rate > 1.0) return -1;
    if (params->morbidity_rate < 0.0 || params->morbidity_rate > 1.0) return -1;
    if (params->economic_impact < 0.0) return -1;
    if (params->insured_portfolio <= 0.0) return -1;
    if (params->reinsurance_coverage < 0.0 || params->reinsurance_coverage > 1.0) return -1;

    return 0;
}

// Estimate risk metrics
double pandemic_loss_model_estimate_risk(const pandemic_loss_params_t *params) {
    if (!params) return 0.0;

    // Simple risk estimation based on portfolio size and rates
    double base_risk = params->insured_portfolio * 0.001;  // 0.1% base risk
    double mortality_risk = params->mortality_rate * params->insured_portfolio * 0.1;
    double morbidity_risk = params->morbidity_rate * params->insured_portfolio * 0.05;

    return base_risk + mortality_risk + morbidity_risk;
}

// Create/destroy functions
pandemic_loss_model_t *pandemic_loss_model_create(int num_regions, int num_sectors) {
    pandemic_loss_model_impl_t *model = malloc(sizeof(pandemic_loss_model_impl_t));
    if (!model) return NULL;

    model->num_regions = num_regions;
    model->num_sectors = num_sectors;

    // Initialize base loss rates
    memcpy(model->base_loss_rates, BASE_LOSS_RATES, sizeof(BASE_LOSS_RATES));

    return (pandemic_loss_model_t *)model;
}

void pandemic_loss_model_destroy(pandemic_loss_model_t *model) {
    if (model) {
        free(model);
    }
}