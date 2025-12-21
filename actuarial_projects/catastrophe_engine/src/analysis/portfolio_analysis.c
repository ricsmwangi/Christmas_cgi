/**
 * @file portfolio_analysis.c
 * @brief Portfolio risk analysis and aggregation
 *
 * Implementation of portfolio-level catastrophe risk assessment.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "catastrophe_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

// Forward declarations
double calculate_haversine_distance(location_t loc1, location_t loc2);
double calculate_property_loss(const property_t *property,
                              const catastrophe_event_t *event,
                              double distance);

// Initialize portfolio with sample data
catastrophe_error_t initialize_portfolio(portfolio_t *portfolio) {
    portfolio->num_properties = 100;
    portfolio->total_value = 1000000000.0;  // $1B portfolio

    // Allocate property array
    portfolio->properties = calloc(portfolio->num_properties, sizeof(property_t));
    if (!portfolio->properties) {
        catastrophe_log_error("Failed to allocate portfolio properties");
        return CE_ERROR_MEMORY_ALLOCATION;
    }

    // Generate sample properties across different regions
    for (size_t i = 0; i < portfolio->num_properties; i++) {
        property_t *prop = &portfolio->properties[i];

        // Random location (US coastal areas)
        prop->location.latitude = 25.0 + (rand() % 20);   // Florida to Maine
        prop->location.longitude = -100.0 + (rand() % 30); // Gulf to Atlantic

        // Random property value ($1M - $50M)
        prop->value = 1000000.0 + (rand() % 49000000);

        // Random construction type (affects vulnerability)
        prop->construction_type = rand() % 3;  // 0=wood, 1=concrete, 2=steel

        // Random occupancy type
        prop->occupancy_type = rand() % 4;  // residential, commercial, industrial, mixed
    }

    catastrophe_log_info("Initialized portfolio with %zu properties, total value: $%.0f",
                        portfolio->num_properties, portfolio->total_value);

    return CE_SUCCESS;
}

// Calculate portfolio loss for a given catastrophe event
catastrophe_error_t calculate_portfolio_loss(const portfolio_t *portfolio,
                                           const catastrophe_event_t *event,
                                           portfolio_loss_t *loss) {
    loss->total_loss = 0.0;
    loss->num_affected_properties = 0;
    loss->max_single_loss = 0.0;

    for (size_t i = 0; i < portfolio->num_properties; i++) {
        const property_t *prop = &portfolio->properties[i];

        // Calculate distance from catastrophe epicenter
        double distance = calculate_haversine_distance(prop->location, (location_t){event->epicenter.latitude, event->epicenter.longitude});

        // Skip if property is outside catastrophe radius
        if (distance > event->radius) {
            continue;
        }

        // Calculate loss for this property
        double property_loss = calculate_property_loss(prop, event, distance);

        if (property_loss > 0.0) {
            loss->total_loss += property_loss;
            loss->num_affected_properties++;

            if (property_loss > loss->max_single_loss) {
                loss->max_single_loss = property_loss;
            }
        }
    }

    // Calculate loss ratio
    loss->loss_ratio = loss->total_loss / portfolio->total_value;

    catastrophe_log_info("Portfolio loss calculation: $%.0f total loss (%.2f%% ratio), "
                        "%zu properties affected",
                        loss->total_loss, loss->loss_ratio * 100.0,
                        loss->num_affected_properties);

    return CE_SUCCESS;
}

// Calculate distance between two geographic points (Haversine formula)
double calculate_haversine_distance(location_t loc1, location_t loc2) {
    const double R = 6371.0;  // Earth's radius in kilometers

    double lat1_rad = loc1.latitude * M_PI / 180.0;
    double lat2_rad = loc2.latitude * M_PI / 180.0;
    double delta_lat = (loc2.latitude - loc1.latitude) * M_PI / 180.0;
    double delta_lon = (loc2.longitude - loc1.longitude) * M_PI / 180.0;

    double a = sin(delta_lat / 2) * sin(delta_lat / 2) +
               cos(lat1_rad) * cos(lat2_rad) *
               sin(delta_lon / 2) * sin(delta_lon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));

    return R * c;
}

// Calculate property-specific loss based on catastrophe type and distance
double calculate_property_loss(const property_t *property,
                              const catastrophe_event_t *event,
                              double distance) {
    double base_vulnerability = 0.0;

    // Base vulnerability by construction type
    switch (property->construction_type) {
        case 0: base_vulnerability = 0.8; break;  // Wood - high vulnerability
        case 1: base_vulnerability = 0.4; break;  // Concrete - medium vulnerability
        case 2: base_vulnerability = 0.2; break;  // Steel - low vulnerability
        default: base_vulnerability = 0.5; break;
    }

    // Adjust vulnerability by occupancy type
    switch (property->occupancy_type) {
        case 0: base_vulnerability *= 1.0; break;  // Residential
        case 1: base_vulnerability *= 1.2; break;  // Commercial (higher value density)
        case 2: base_vulnerability *= 0.8; break;  // Industrial (more robust)
        case 3: base_vulnerability *= 1.1; break;  // Mixed
    }

    // Distance-based attenuation
    double distance_factor = 1.0 - (distance / event->radius);
    if (distance_factor < 0.0) distance_factor = 0.0;

    // Event-specific intensity factors
    double intensity_factor = 1.0;
    switch (event->type) {
        case CATASTROPHE_HURRICANE:
            // Wind damage scales with square of wind speed
            intensity_factor = event->wind_speed / 50.0;
            intensity_factor = intensity_factor * intensity_factor;
            break;

        case CATASTROPHE_FLOOD:
            // Flood damage scales with rainfall intensity
            intensity_factor = event->rainfall / 200.0;
            break;

        case CATASTROPHE_EARTHQUAKE:
            // Earthquake damage scales with magnitude
            intensity_factor = event->magnitude / 7.0;
            break;

        default:
            intensity_factor = 1.0;
            break;
    }

    // Calculate final loss
    double vulnerability = base_vulnerability * distance_factor * intensity_factor;
    if (vulnerability > 1.0) vulnerability = 1.0;

    return property->value * vulnerability;
}

// Aggregate losses across multiple catastrophe events
catastrophe_error_t aggregate_portfolio_losses(const portfolio_loss_t *losses,
                                             size_t num_events,
                                             portfolio_risk_metrics_t *metrics) {
    if (num_events == 0) {
        memset(metrics, 0, sizeof(*metrics));
        return CE_SUCCESS;
    }

    // Calculate average loss
    metrics->expected_loss = 0.0;
    metrics->max_loss = 0.0;
    metrics->loss_volatility = 0.0;

    for (size_t i = 0; i < num_events; i++) {
        metrics->expected_loss += losses[i].total_loss;
        if (losses[i].total_loss > metrics->max_loss) {
            metrics->max_loss = losses[i].total_loss;
        }
    }
    metrics->expected_loss /= num_events;

    // Calculate volatility (standard deviation)
    double variance = 0.0;
    for (size_t i = 0; i < num_events; i++) {
        double diff = losses[i].total_loss - metrics->expected_loss;
        variance += diff * diff;
    }
    variance /= num_events;
    metrics->loss_volatility = sqrt(variance);

    // Calculate Value at Risk (95% confidence)
    // Assuming normal distribution for simplicity
    metrics->var_95 = metrics->expected_loss + 1.645 * metrics->loss_volatility;

    catastrophe_log_info("Portfolio risk metrics: Expected loss $%.0f, "
                        "Max loss $%.0f, VaR95 $%.0f",
                        metrics->expected_loss, metrics->max_loss, metrics->var_95);

    return CE_SUCCESS;
}

// Clean up portfolio resources
void cleanup_portfolio(portfolio_t *portfolio) {
    if (portfolio->properties) {
        free(portfolio->properties);
        portfolio->properties = NULL;
    }
    portfolio->num_properties = 0;
    portfolio->total_value = 0.0;
}