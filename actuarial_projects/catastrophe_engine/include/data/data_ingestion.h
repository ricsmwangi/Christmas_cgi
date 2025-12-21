/**
 * @file data_ingestion.h
 * @brief Data ingestion and processing functions
 *
 * Header file for real-time data ingestion from various sources.
 *
 * @author Catastrophe Engine Team
 * @date 2024
 * @version 1.0.0
 */

#ifndef DATA_INGESTION_H
#define DATA_INGESTION_H

#include "catastrophe_engine.h"

// Weather data ingestion
catastrophe_error_t ingest_weather_data(const char *source_url, weather_data_t *data);

// Economic data ingestion
catastrophe_error_t ingest_economic_data(const char *source_url, economic_data_t *data);

// Satellite data ingestion
catastrophe_error_t ingest_satellite_data(const char *source_url, satellite_data_t *data);

// Process ingested data into catastrophe events
catastrophe_error_t process_ingested_data(const weather_data_t *weather,
                                        const economic_data_t *economic,
                                        const satellite_data_t *satellite,
                                        catastrophe_event_t *event);

#endif // DATA_INGESTION_H