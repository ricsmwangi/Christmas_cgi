/**
 * @file catastrophe_model.c
 * @brief Catastrophe loss modeling implementation
 *
 * Implementation of catastrophe loss calculation models.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "catastrophe_engine.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>

// Hurricane loss calculation
catastrophe_error_t calculate_hurricane_loss(const catastrophe_event_t *event,
                                           const location_t *location,
                                           double *loss_amount) {
    if (!event || !location || !loss_amount) {
        return CE_ERROR_INVALID_INPUT;
    }

    // Simple distance-based loss calculation for hurricanes
    // This is a placeholder - real models would use detailed damage functions
    double distance = calculate_distance((const geo_location_t *)location,
                                       &event->epicenter);

    if (distance > event->radius) {
        *loss_amount = 0.0;
        return CE_SUCCESS;
    }

    // Basic wind damage model
    double wind_damage = event->wind_speed * event->wind_speed / 10000.0;  // quadratic wind damage
    double distance_factor = 1.0 - (distance / event->radius);

    *loss_amount = 100000.0 * wind_damage * distance_factor;  // Base loss of $100k

    return CE_SUCCESS;
}

// Flood loss calculation
catastrophe_error_t calculate_flood_loss(const catastrophe_event_t *event,
                                        const location_t *location,
                                        double *loss_amount) {
    if (!event || !location || !loss_amount) {
        return CE_ERROR_INVALID_INPUT;
    }

    // Simple distance-based loss calculation for floods
    double distance = calculate_distance((const geo_location_t *)location,
                                       &event->epicenter);

    if (distance > event->radius) {
        *loss_amount = 0.0;
        return CE_SUCCESS;
    }

    // Basic flood damage model based on rainfall
    double rainfall_factor = event->rainfall / 200.0;  // Normalize to 200mm
    double distance_factor = 1.0 - (distance / event->radius);

    *loss_amount = 75000.0 * rainfall_factor * distance_factor;  // Base loss of $75k

    return CE_SUCCESS;
}

// Earthquake loss calculation
catastrophe_error_t calculate_earthquake_loss(const catastrophe_event_t *event,
                                             const location_t *location,
                                             double *loss_amount) {
    if (!event || !location || !loss_amount) {
        return CE_ERROR_INVALID_INPUT;
    }

    // Simple distance-based loss calculation for earthquakes
    double distance = calculate_distance((const geo_location_t *)location,
                                       &event->epicenter);

    if (distance > event->radius) {
        *loss_amount = 0.0;
        return CE_SUCCESS;
    }

    // Basic earthquake damage model based on magnitude
    double magnitude_factor = event->magnitude / 7.0;  // Normalize to magnitude 7
    double distance_factor = 1.0 - (distance / event->radius);

    *loss_amount = 200000.0 * magnitude_factor * distance_factor;  // Base loss of $200k

    return CE_SUCCESS;
}

// Generic catastrophe loss calculation
catastrophe_error_t calculate_catastrophe_loss(catastrophe_type_t type,
                                              const catastrophe_event_t *event,
                                              const location_t *location,
                                              double *loss_amount) {
    switch (type) {
        case CATASTROPHE_HURRICANE:
            return calculate_hurricane_loss(event, location, loss_amount);
        case CATASTROPHE_FLOOD:
            return calculate_flood_loss(event, location, loss_amount);
        case CATASTROPHE_EARTHQUAKE:
            return calculate_earthquake_loss(event, location, loss_amount);
        case CATASTROPHE_WILDFIRE:
        case CATASTROPHE_TORNADO:
        case CATASTROPHE_HAILSTORM:
        case CATASTROPHE_WINTER_STORM:
        case CATASTROPHE_TSUNAMI:
        case CATASTROPHE_VOLCANIC_ERUPTION:
        case CATASTROPHE_MANMADE_DISASTER:
            // Placeholder for other catastrophe types
            *loss_amount = 50000.0;  // Fixed loss for unimplemented types
            return CE_SUCCESS;
        default:
            return CE_ERROR_INVALID_INPUT;
    }
}