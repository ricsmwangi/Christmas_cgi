/**
 * @file pandemic_loss_model.h
 * @brief Pandemic loss model header
 *
 * Header file for actuarial loss modeling.
 *
 * @author Pandemic Risk Simulator Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef PANDEMIC_LOSS_MODEL_H
#define PANDEMIC_LOSS_MODEL_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

// Insurance line types
typedef enum {
    LINE_LIFE_INSURANCE,
    LINE_HEALTH_INSURANCE,
    LINE_PROPERTY_INSURANCE,
    LINE_BUSINESS_INTERRUPTION,
    LINE_EVENT_CANCELLATION,
    LINE_COUNT
} insurance_line_t;

// Loss calculation structure
typedef struct {
    double mortality_rate;           // Excess mortality rate
    double morbidity_rate;           // Excess morbidity rate
    double economic_impact;          // Economic loss multiplier
    double insured_portfolio;        // Total insured value
    double reinsurance_coverage;     // Reinsurance percentage
    size_t num_policies;
} pandemic_loss_params_t;

// Loss result structure
typedef struct {
    double total_losses[LINE_COUNT];     // Losses by insurance line
    double net_losses[LINE_COUNT];       // Net losses after reinsurance
    double aggregate_loss;               // Total portfolio loss
    double reinsurance_cost;             // Reinsurance premium cost
    double risk_adjusted_loss;           // Risk-adjusted loss measure
} pandemic_loss_result_t;

// Function declarations
int pandemic_loss_model_calculate(const pandemic_loss_params_t *params,
                                 const double *epidemic_data,
                                 pandemic_loss_result_t *results);
int pandemic_loss_model_validate_params(const pandemic_loss_params_t *params);
double pandemic_loss_model_estimate_risk(const pandemic_loss_params_t *params);

typedef struct pandemic_loss_model_s pandemic_loss_model_t;

pandemic_loss_model_t *pandemic_loss_model_create(int num_regions, int num_sectors);
void pandemic_loss_model_destroy(pandemic_loss_model_t *model);

#ifdef __cplusplus
}
#endif

#endif /* PANDEMIC_LOSS_MODEL_H */