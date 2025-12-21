/**
 * @file data_ingestion.c
 * @brief Data ingestion and processing
 *
 * Implementation of real-time data ingestion from various sources.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "catastrophe_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Simple weather data ingestion (placeholder)
catastrophe_error_t ingest_weather_data(const char *source_url, weather_data_t *data) {
    (void)source_url;  // Not used in placeholder

    // Generate fake weather data for testing
    static int call_count = 0;
    call_count++;

    data->timestamp = time(NULL);
    data->location.latitude = 25.0 + (call_count % 10);
    data->location.longitude = -80.0 + (call_count % 10);
    data->wind_speed = 20.0 + (rand() % 50);  // 20-70 knots
    data->temperature = 20.0 + (rand() % 20); // 20-40°C
    data->humidity = 60.0 + (rand() % 40);    // 60-100%
    data->pressure = 1000.0 + (rand() % 50);  // 1000-1050 hPa

    catastrophe_log_info("Ingested weather data: wind=%.1f knots, temp=%.1f°C",
                        data->wind_speed, data->temperature);

    return CE_SUCCESS;
}

// Simple economic data ingestion (placeholder)
catastrophe_error_t ingest_economic_data(const char *source_url, economic_data_t *data) {
    (void)source_url;  // Not used in placeholder

    // Generate fake economic data
    static int call_count = 0;
    call_count++;

    data->timestamp = time(NULL);
    data->gdp_growth = 2.0 + (rand() % 4) - 2.0;  // -2% to +2%
    data->inflation_rate = 2.0 + (rand() % 6);     // 2-8%
    data->unemployment_rate = 3.0 + (rand() % 10); // 3-13%
    data->market_index = 3000.0 + (rand() % 1000); // 3000-4000

    catastrophe_log_info("Ingested economic data: GDP=%.1f%%, inflation=%.1f%%",
                        data->gdp_growth, data->inflation_rate);

    return CE_SUCCESS;
}

// Satellite data ingestion (placeholder)
catastrophe_error_t ingest_satellite_data(const char *source_url, satellite_data_t *data) {
    (void)source_url;  // Not used in placeholder

    // Generate fake satellite data
    data->timestamp = time(NULL);
    data->cloud_cover = (rand() % 100);      // 0-100%
    data->vegetation_index = 0.3 + (rand() % 70) / 100.0;  // 0.3-1.0
    data->soil_moisture = 0.1 + (rand() % 90) / 100.0;     // 0.1-1.0

    catastrophe_log_info("Ingested satellite data: clouds=%d%%, vegetation=%.2f",
                        data->cloud_cover, data->vegetation_index);

    return CE_SUCCESS;
}

// Process ingested data into catastrophe events
catastrophe_error_t process_ingested_data(const weather_data_t *weather,
                                        const economic_data_t *economic,
                                        const satellite_data_t *satellite,
                                        catastrophe_event_t *event) {
    (void)economic;  // Not used in current implementation

    // Simple logic to detect potential catastrophe conditions
    if (weather->wind_speed > 50.0 && weather->pressure < 1000.0) {
        // Potential hurricane
        event->type = CATASTROPHE_HURRICANE;
        event->epicenter.latitude = weather->location.latitude;
        event->epicenter.longitude = weather->location.longitude;
        event->epicenter.elevation = 0.0;
        strcpy(event->epicenter.country_code, "US");
        strcpy(event->epicenter.region_code, "FL");
        event->radius = 100.0;  // 100km radius
        event->intensity = weather->wind_speed / 10.0;  // Scale 0-10
        event->wind_speed = weather->wind_speed;
        event->start_time = weather->timestamp;
        event->is_active = true;

        catastrophe_log_warning("Potential hurricane detected at (%.2f, %.2f)",
                              event->epicenter.latitude, event->epicenter.longitude);
        return CE_SUCCESS;
    }

    if (satellite->soil_moisture > 0.8 && weather->humidity > 80.0) {
        // Potential flood
        event->type = CATASTROPHE_FLOOD;
        event->epicenter.latitude = weather->location.latitude;
        event->epicenter.longitude = weather->location.longitude;
        event->epicenter.elevation = 0.0;
        strcpy(event->epicenter.country_code, "US");
        strcpy(event->epicenter.region_code, "TX");
        event->radius = 50.0;  // 50km radius
        event->intensity = satellite->soil_moisture * 10.0;
        event->rainfall = 100.0 + (rand() % 200);  // 100-300mm
        event->start_time = weather->timestamp;
        event->is_active = true;

        catastrophe_log_warning("Potential flood detected at (%.2f, %.2f)",
                              event->epicenter.latitude, event->epicenter.longitude);
        return CE_SUCCESS;
    }

    // No catastrophe detected
    return CE_SUCCESS;
}