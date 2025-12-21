/**
 * @file catastrophe_model.h
 * @brief Catastrophe loss modeling functions
 *
 * Header file for catastrophe loss calculation implementations.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef CATASTROPHE_MODEL_H
#define CATASTROPHE_MODEL_H

#include "catastrophe_engine.h"

// Hurricane loss calculation
catastrophe_error_t calculate_hurricane_loss(const catastrophe_event_t *event,
                                           const location_t *location,
                                           double *loss_amount);

// Flood loss calculation
catastrophe_error_t calculate_flood_loss(const catastrophe_event_t *event,
                                        const location_t *location,
                                        double *loss_amount);

// Earthquake loss calculation
catastrophe_error_t calculate_earthquake_loss(const catastrophe_event_t *event,
                                             const location_t *location,
                                             double *loss_amount);

// Generic catastrophe loss calculation
catastrophe_error_t calculate_catastrophe_loss(catastrophe_type_t type,
                                              const catastrophe_event_t *event,
                                              const location_t *location,
                                              double *loss_amount);

#endif // CATASTROPHE_MODEL_H