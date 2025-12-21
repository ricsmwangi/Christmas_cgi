#ifndef CATASTROPHE_ENGINE_H
#define CATASTROPHE_ENGINE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

// Version information
#define CATASTROPHE_ENGINE_VERSION "1.0.0"
#define CATASTROPHE_ENGINE_BUILD_DATE __DATE__
#define CATASTROPHE_ENGINE_BUILD_TIME __TIME__

// Configuration constants
#define MAX_PORTFOLIO_SIZE 1000000
#define MAX_CATASTROPHE_EVENTS 10000
#define MAX_CLUSTER_NODES 256
#define MAX_DATA_SOURCES 100

// Performance tuning
#define DEFAULT_BUFFER_SIZE (64 * 1024 * 1024)  // 64MB
#define MAX_CONCURRENT_REQUESTS 1000
#define WORKER_THREAD_POOL_SIZE 64

// Network configuration
#define DEFAULT_PORT 8080
#define MAX_CONNECTION_BACKLOG 1024
#define HEARTBEAT_INTERVAL 30  // seconds
#define CONNECTION_TIMEOUT 300 // seconds

// Data types
typedef uint64_t policy_id_t;
typedef uint32_t catastrophe_id_t;
typedef double risk_score_t;
typedef double loss_amount_t;

// Forward declarations
struct catastrophe_event;
struct portfolio_policy;
struct risk_calculation;
struct cluster_node;

// Error codes
typedef enum {
    CE_SUCCESS = 0,
    CE_ERROR_INVALID_INPUT = -1,
    CE_ERROR_MEMORY_ALLOCATION = -2,
    CE_ERROR_NETWORK_FAILURE = -3,
    CE_ERROR_FILE_IO = -4,
    CE_ERROR_CALCULATION_ERROR = -5,
    CE_ERROR_CLUSTER_FAILURE = -6,
    CE_ERROR_TIMEOUT = -7,
    CE_ERROR_AUTHENTICATION = -8,
    CE_ERROR_UNKNOWN = -99
} catastrophe_error_t;

// Catastrophe types
typedef enum {
    CATASTROPHE_HURRICANE = 1,
    CATASTROPHE_EARTHQUAKE = 2,
    CATASTROPHE_FLOOD = 3,
    CATASTROPHE_WILDFIRE = 4,
    CATASTROPHE_TORNADO = 5,
    CATASTROPHE_HAILSTORM = 6,
    CATASTROPHE_WINTER_STORM = 7,
    CATASTROPHE_TSUNAMI = 8,
    CATASTROPHE_VOLCANIC_ERUPTION = 9,
    CATASTROPHE_MANMADE_DISASTER = 10
} catastrophe_type_t;

// Risk calculation modes
typedef enum {
    RISK_MODE_EXPECTED_LOSS = 1,
    RISK_MODE_PROBABLE_MAXIMUM_LOSS = 2,
    RISK_MODE_RETURN_PERIOD_ANALYSIS = 3,
    RISK_MODE_STRESS_TEST = 4,
    RISK_MODE_REAL_TIME_MONITORING = 5
} risk_calculation_mode_t;

// Data structures

// Geographic location
typedef struct {
    double latitude;
    double longitude;
    double elevation;  // meters above sea level
    char country_code[3];
    char region_code[10];
} geo_location_t;

// Catastrophe event parameters
typedef struct catastrophe_event {
    catastrophe_id_t id;
    catastrophe_type_t type;
    char name[256];
    geo_location_t epicenter;
    time_t start_time;
    time_t end_time;
    double intensity;  // Scale-specific (e.g., Richter, Saffir-Simpson)
    double radius;     // Affected radius in kilometers
    double wind_speed; // km/h (for hurricanes)
    double magnitude;  // Richter scale (for earthquakes)
    double rainfall;   // mm (for floods)
    bool is_active;
    struct catastrophe_event *next;
} catastrophe_event_t;

// Insurance policy
typedef struct portfolio_policy {
    policy_id_t id;
    char policy_number[64];
    geo_location_t location;
    double coverage_amount;
    double deductible;
    char peril_type[32];  // hurricane, earthquake, flood, etc.
    char construction_type[32];  // wood, concrete, etc.
    int year_built;
    double building_value;
    double contents_value;
    struct portfolio_policy *next;
} portfolio_policy_t;

// Risk calculation result
typedef struct risk_calculation {
    policy_id_t policy_id;
    catastrophe_id_t catastrophe_id;
    risk_score_t expected_loss;
    risk_score_t probable_max_loss;
    double loss_probability;
    time_t calculation_time;
    char calculation_method[64];
    bool is_realtime;
} risk_calculation_t;

// Cluster node information
typedef struct cluster_node {
    char hostname[256];
    char ip_address[64];
    uint16_t port;
    bool is_active;
    time_t last_heartbeat;
    uint32_t active_connections;
    double cpu_usage;
    double memory_usage;
    struct cluster_node *next;
} cluster_node_t;

// Main engine configuration
typedef struct {
    char config_file[1024];
    char log_file[1024];
    char data_directory[1024];
    uint16_t port;
    bool enable_clustering;
    uint32_t max_connections;
    uint32_t worker_threads;
    size_t buffer_size;
    bool enable_ssl;
    char ssl_cert_file[1024];
    char ssl_key_file[1024];
    bool enable_metrics;
    uint32_t metrics_port;
} engine_config_t;

// Additional data structures for new components

// Location (simplified version for calculations)
typedef struct {
    double latitude;
    double longitude;
} location_t;

// Weather data structure
typedef struct {
    time_t timestamp;
    location_t location;
    double wind_speed;  // knots
    double temperature; // Celsius
    double humidity;    // percentage
    double pressure;    // hPa
} weather_data_t;

// Economic data structure
typedef struct {
    time_t timestamp;
    double gdp_growth;      // percentage
    double inflation_rate;  // percentage
    double unemployment_rate; // percentage
    double market_index;    // index value
} economic_data_t;

// Satellite data structure
typedef struct {
    time_t timestamp;
    double cloud_cover;     // percentage
    double vegetation_index; // NDVI
    double soil_moisture;   // percentage
} satellite_data_t;

// Property information for portfolio analysis
typedef struct {
    location_t location;
    double value;           // property value in dollars
    int construction_type;  // 0=wood, 1=concrete, 2=steel
    int occupancy_type;     // 0=residential, 1=commercial, 2=industrial, 3=mixed
} property_t;

// Portfolio structure
typedef struct {
    size_t num_properties;
    double total_value;
    property_t *properties;
} portfolio_t;

// Portfolio loss calculation result
typedef struct {
    double total_loss;
    size_t num_affected_properties;
    double max_single_loss;
    double loss_ratio;  // loss as percentage of total portfolio value
} portfolio_loss_t;

// Portfolio risk metrics
typedef struct {
    double expected_loss;
    double max_loss;
    double loss_volatility;
    double var_95;  // Value at Risk at 95% confidence
} portfolio_risk_metrics_t;

// Distributed processing configuration
typedef struct {
    int num_nodes;
    int num_threads;
    bool use_mpi;
    bool use_cuda;
} distributed_config_t;

// Catastrophe scenario for simulation
typedef struct {
    catastrophe_event_t base_event;
    size_t num_simulations;
    double confidence_level;
} catastrophe_scenario_t;

// Simulation result
typedef struct {
    size_t total_scenarios;
    size_t completed_scenarios;
    double average_loss;
    double max_loss;
} simulation_result_t;

// Function prototypes

// Core engine functions
catastrophe_error_t catastrophe_engine_init(const engine_config_t *config);
catastrophe_error_t catastrophe_engine_start(void);
catastrophe_error_t catastrophe_engine_stop(void);
catastrophe_error_t catastrophe_engine_shutdown(void);

// Catastrophe management
catastrophe_error_t catastrophe_add_event(const catastrophe_event_t *event);
catastrophe_error_t catastrophe_update_event(catastrophe_id_t id, const catastrophe_event_t *updates);
catastrophe_error_t catastrophe_remove_event(catastrophe_id_t id);
catastrophe_error_t catastrophe_get_event(catastrophe_id_t id, catastrophe_event_t *event);

// Portfolio management
catastrophe_error_t portfolio_add_policy(const portfolio_policy_t *policy);
catastrophe_error_t portfolio_update_policy(policy_id_t id, const portfolio_policy_t *updates);
catastrophe_error_t portfolio_remove_policy(policy_id_t id);
catastrophe_error_t portfolio_get_policy(policy_id_t id, portfolio_policy_t *policy);

// Risk calculation
catastrophe_error_t calculate_portfolio_risk(policy_id_t policy_id,
                                           catastrophe_id_t catastrophe_id,
                                           risk_calculation_mode_t mode,
                                           risk_calculation_t *result);
catastrophe_error_t calculate_bulk_risk(policy_id_t *policy_ids,
                                       size_t num_policies,
                                       catastrophe_id_t catastrophe_id,
                                       risk_calculation_mode_t mode,
                                       risk_calculation_t **results,
                                       size_t *num_results);

// Cluster management
catastrophe_error_t cluster_join(const char *master_host, uint16_t master_port);
catastrophe_error_t cluster_leave(void);
catastrophe_error_t cluster_get_nodes(cluster_node_t **nodes, size_t *num_nodes);
catastrophe_error_t cluster_distribute_work(const void *work_data, size_t data_size);

// Data ingestion
catastrophe_error_t data_ingest_weather_data(const char *data_source, const void *data, size_t size);
catastrophe_error_t data_ingest_satellite_data(const char *data_source, const void *data, size_t size);
catastrophe_error_t data_ingest_economic_data(const char *data_source, const void *data, size_t size);

// Real-time monitoring
catastrophe_error_t realtime_subscribe_alerts(const char *callback_url);
catastrophe_error_t realtime_unsubscribe_alerts(const char *callback_url);
catastrophe_error_t realtime_get_live_data(const char *data_type, void **data, size_t *size);

// Utility functions
const char *catastrophe_error_string(catastrophe_error_t error);
const char *catastrophe_type_string(catastrophe_type_t type);
double calculate_distance(const geo_location_t *loc1, const geo_location_t *loc2);
bool is_location_affected(const geo_location_t *location, const catastrophe_event_t *event);

// Logging and debugging
void catastrophe_log_error(const char *format, ...);
void catastrophe_log_warning(const char *format, ...);
void catastrophe_log_info(const char *format, ...);
void catastrophe_log_debug(const char *format, ...);

// Memory management helpers
void *catastrophe_malloc(size_t size);
void *catastrophe_calloc(size_t nmemb, size_t size);
void *catastrophe_realloc(void *ptr, size_t size);
void catastrophe_free(void *ptr);

// Thread safety
void catastrophe_mutex_lock(void);
void catastrophe_mutex_unlock(void);
bool catastrophe_mutex_try_lock(void);

#endif // CATASTROPHE_ENGINE_H