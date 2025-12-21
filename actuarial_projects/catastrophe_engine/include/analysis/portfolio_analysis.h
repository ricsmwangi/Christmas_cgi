/**
 * @file portfolio_analysis.h
 * @brief Portfolio risk analysis and aggregation
 *
 * Header file for portfolio-level catastrophe risk assessment.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef PORTFOLIO_ANALYSIS_H
#define PORTFOLIO_ANALYSIS_H

#include "catastrophe_engine.h"

// Initialize portfolio with sample data
catastrophe_error_t initialize_portfolio(portfolio_t *portfolio);

// Calculate portfolio loss for a given catastrophe event
catastrophe_error_t calculate_portfolio_loss(const portfolio_t *portfolio,
                                           const catastrophe_event_t *event,
                                           portfolio_loss_t *loss);

// Calculate distance between two geographic points
double calculate_haversine_distance(location_t loc1, location_t loc2);

// Calculate property-specific loss
double calculate_property_loss(const property_t *property,
                              const catastrophe_event_t *event,
                              double distance);

// Aggregate losses across multiple catastrophe events
catastrophe_error_t aggregate_portfolio_losses(const portfolio_loss_t *losses,
                                             size_t num_events,
                                             portfolio_risk_metrics_t *metrics);

// Clean up portfolio resources
void cleanup_portfolio(portfolio_t *portfolio);

#endif // PORTFOLIO_ANALYSIS_H