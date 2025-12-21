/**
 * @file api_server.c
 * @brief REST API Server for AI Underwriting Engine
 *
 * Provides HTTP REST API endpoints for underwriting application submission,
 * decision retrieval, and system monitoring.
 *
 * @author AI Underwriting Engine Team
 * @date 2024
 * @version 1.0.0
 */

#include "ai_underwriting_engine.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <signal.h>
#include <errno.h>

// API server configuration
#define MAX_CONNECTIONS 100
#define REQUEST_BUFFER_SIZE 8192
#define RESPONSE_BUFFER_SIZE 16384
#define MAX_WORKER_THREADS 10
#define REQUEST_TIMEOUT_SEC 30

// HTTP status codes
#define HTTP_200_OK "200 OK"
#define HTTP_201_CREATED "201 Created"
#define HTTP_400_BAD_REQUEST "400 Bad Request"
#define HTTP_404_NOT_FOUND "404 Not Found"
#define HTTP_405_METHOD_NOT_ALLOWED "405 Method Not Allowed"
#define HTTP_500_INTERNAL_ERROR "500 Internal Server Error"
#define HTTP_503_SERVICE_UNAVAILABLE "503 Service Unavailable"

// API endpoints
#define ENDPOINT_HEALTH "/health"
#define ENDPOINT_APPLICATIONS "/applications"
#define ENDPOINT_DECISIONS "/decisions"
#define ENDPOINT_METRICS "/metrics"

// HTTP request structure
typedef struct {
    char method[16];
    char path[256];
    char version[16];
    char headers[REQUEST_BUFFER_SIZE];
    char body[REQUEST_BUFFER_SIZE];
    size_t body_length;
} http_request_t;

// HTTP response structure
typedef struct {
    int status_code;
    char status_text[32];
    char content_type[64];
    char body[RESPONSE_BUFFER_SIZE];
    size_t body_length;
} http_response_t;

// Worker thread data
typedef struct {
    int client_socket;
    struct sockaddr_in client_addr;
} worker_data_t;

// API server context
typedef struct {
    int server_socket;
    int port;
    bool running;
    pthread_t worker_threads[MAX_WORKER_THREADS];
    pthread_mutex_t mutex;
    size_t active_connections;
    time_t start_time;
} api_server_context_t;

// Global context
static api_server_context_t *api_context = NULL;

// Forward declarations
static void *worker_thread_func(void *arg);
static au_error_t parse_http_request(int client_socket, http_request_t *request);
static au_error_t process_request(const http_request_t *request, http_response_t *response);
static au_error_t handle_health_check(http_response_t *response);
static au_error_t handle_application_submission(const http_request_t *request, http_response_t *response);
static au_error_t handle_decision_retrieval(const http_request_t *request, http_response_t *response);
static au_error_t handle_metrics_request(http_response_t *response);
static au_error_t send_http_response(int client_socket, const http_response_t *response);
static au_error_t create_json_response(http_response_t *response, const char *json_data);
static au_error_t create_error_response(http_response_t *response, int status_code, const char *message);

/**
 * @brief Initialize the API server
 */
au_error_t au_initialize_api_server(int port) {
    if (api_context != NULL) {
        return AU_ERROR_ALREADY_INITIALIZED;
    }

    api_context = calloc(1, sizeof(api_server_context_t));
    if (api_context == NULL) {
        return AU_ERROR_MEMORY_ALLOCATION;
    }

    api_context->port = port;
    api_context->running = false;
    api_context->active_connections = 0;
    api_context->start_time = time(NULL);

    // Initialize mutex
    if (pthread_mutex_init(&api_context->mutex, NULL) != 0) {
        free(api_context);
        api_context = NULL;
        return AU_ERROR_THREADING;
    }

    // Create server socket
    api_context->server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (api_context->server_socket < 0) {
        pthread_mutex_destroy(&api_context->mutex);
        free(api_context);
        api_context = NULL;
        return AU_ERROR_NETWORK;
    }

    // Set socket options
    int opt = 1;
    if (setsockopt(api_context->server_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        close(api_context->server_socket);
        pthread_mutex_destroy(&api_context->mutex);
        free(api_context);
        api_context = NULL;
        return AU_ERROR_NETWORK;
    }

    // Bind socket
    struct sockaddr_in server_addr;
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(api_context->server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        close(api_context->server_socket);
        pthread_mutex_destroy(&api_context->mutex);
        free(api_context);
        api_context = NULL;
        return AU_ERROR_NETWORK;
    }

    // Listen for connections
    if (listen(api_context->server_socket, MAX_CONNECTIONS) < 0) {
        close(api_context->server_socket);
        pthread_mutex_destroy(&api_context->mutex);
        free(api_context);
        api_context = NULL;
        return AU_ERROR_NETWORK;
    }

    au_log_info("API server initialized on port %d", port);
    return AU_SUCCESS;
}

/**
 * @brief Start the API server
 */
au_error_t au_start_api_server(void) {
    if (api_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (api_context->running) {
        return AU_ERROR_ALREADY_RUNNING;
    }

    api_context->running = true;

    // Create worker threads
    for (int i = 0; i < MAX_WORKER_THREADS; i++) {
        if (pthread_create(&api_context->worker_threads[i], NULL, worker_thread_func, NULL) != 0) {
            api_context->running = false;
            return AU_ERROR_THREADING;
        }
    }

    au_log_info("API server started with %d worker threads", MAX_WORKER_THREADS);
    return AU_SUCCESS;
}

/**
 * @brief Stop the API server
 */
au_error_t au_stop_api_server(void) {
    if (api_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    if (!api_context->running) {
        return AU_SUCCESS;
    }

    api_context->running = false;

    // Close server socket to wake up accept()
    close(api_context->server_socket);

    // Wait for worker threads to finish
    for (int i = 0; i < MAX_WORKER_THREADS; i++) {
        pthread_join(api_context->worker_threads[i], NULL);
    }

    au_log_info("API server stopped");
    return AU_SUCCESS;
}

/**
 * @brief Shutdown the API server
 */
au_error_t au_shutdown_api_server(void) {
    if (api_context == NULL) {
        return AU_ERROR_NOT_INITIALIZED;
    }

    au_stop_api_server();

    pthread_mutex_destroy(&api_context->mutex);
    free(api_context);
    api_context = NULL;

    au_log_info("API server shutdown");
    return AU_SUCCESS;
}

/**
 * @brief Worker thread function
 */
static void *worker_thread_func(void *arg) {
    (void)arg; // Unused parameter

    while (api_context->running) {
        // Accept client connection
        struct sockaddr_in client_addr;
        socklen_t client_len = sizeof(client_addr);
        int client_socket = accept(api_context->server_socket,
                                 (struct sockaddr *)&client_addr, &client_len);

        if (client_socket < 0) {
            if (api_context->running) {
                au_log_error("Failed to accept client connection: %s", strerror(errno));
            }
            continue;
        }

        // Check connection limit
        pthread_mutex_lock(&api_context->mutex);
        if (api_context->active_connections >= MAX_CONNECTIONS) {
            pthread_mutex_unlock(&api_context->mutex);
            close(client_socket);
            continue;
        }
        api_context->active_connections++;
        pthread_mutex_unlock(&api_context->mutex);

        au_log_debug("Accepted connection from %s:%d",
                    inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        // Process request
        http_request_t request;
        au_error_t result = parse_http_request(client_socket, &request);

        if (result == AU_SUCCESS) {
            http_response_t response;
            result = process_request(&request, &response);

            if (result == AU_SUCCESS) {
                send_http_response(client_socket, &response);
            } else {
                http_response_t error_response;
                create_error_response(&error_response, 500, "Internal server error");
                send_http_response(client_socket, &error_response);
            }
        } else {
            http_response_t error_response;
            create_error_response(&error_response, 400, "Bad request");
            send_http_response(client_socket, &error_response);
        }

        // Close connection
        close(client_socket);

        pthread_mutex_lock(&api_context->mutex);
        api_context->active_connections--;
        pthread_mutex_unlock(&api_context->mutex);
    }

    return NULL;
}

/**
 * @brief Parse HTTP request
 */
static au_error_t parse_http_request(int client_socket, http_request_t *request) {
    char buffer[REQUEST_BUFFER_SIZE];
    ssize_t bytes_read;

    // Set socket timeout
    struct timeval timeout = {REQUEST_TIMEOUT_SEC, 0};
    if (setsockopt(client_socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout)) < 0) {
        return AU_ERROR_NETWORK;
    }

    // Read request line
    bytes_read = recv(client_socket, buffer, sizeof(buffer) - 1, 0);
    if (bytes_read <= 0) {
        return AU_ERROR_NETWORK;
    }

    buffer[bytes_read] = '\0';

    // Parse request line
    char *line_end = strstr(buffer, "\r\n");
    if (line_end == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    *line_end = '\0';
    if (sscanf(buffer, "%15s %255s %15s", request->method, request->path, request->version) != 3) {
        return AU_ERROR_INVALID_INPUT;
    }

    // Read headers and body
    char *header_start = line_end + 2;
    char *body_start = strstr(header_start, "\r\n\r\n");
    if (body_start == NULL) {
        return AU_ERROR_INVALID_INPUT;
    }

    *body_start = '\0';
    strcpy(request->headers, header_start);

    body_start += 4;
    strcpy(request->body, body_start);
    request->body_length = strlen(request->body);

    return AU_SUCCESS;
}

/**
 * @brief Process HTTP request
 */
static au_error_t process_request(const http_request_t *request, http_response_t *response) {
    au_log_debug("Processing %s request to %s", request->method, request->path);

    // Route request based on method and path
    if (strcmp(request->method, "GET") == 0) {
        if (strcmp(request->path, ENDPOINT_HEALTH) == 0) {
            return handle_health_check(response);
        } else if (strcmp(request->path, ENDPOINT_METRICS) == 0) {
            return handle_metrics_request(response);
        } else if (strncmp(request->path, ENDPOINT_DECISIONS, strlen(ENDPOINT_DECISIONS)) == 0) {
            return handle_decision_retrieval(request, response);
        }
    } else if (strcmp(request->method, "POST") == 0) {
        if (strcmp(request->path, ENDPOINT_APPLICATIONS) == 0) {
            return handle_application_submission(request, response);
        }
    }

    // Method not allowed
    return create_error_response(response, 405, "Method not allowed");
}

/**
 * @brief Handle health check request
 */
static au_error_t handle_health_check(http_response_t *response) {
    char json_response[512];
    time_t uptime = time(NULL) - api_context->start_time;

    snprintf(json_response, sizeof(json_response),
             "{\"status\":\"healthy\",\"uptime\":%ld,\"timestamp\":%ld}",
             uptime, time(NULL));

    return create_json_response(response, json_response);
}

/**
 * @brief Handle application submission
 */
static au_error_t handle_application_submission(const http_request_t *request, http_response_t *response) {
    // Parse JSON request body (simplified - in real implementation use JSON parser)
    underwriting_application_t application = {0};

    // Extract fields from JSON (placeholder implementation)
    // In real implementation, would parse JSON properly
    char *json = request->body;

    // Simple JSON parsing (very basic)
    char *id_str = strstr(json, "\"id\":");
    if (id_str) {
        sscanf(id_str, "\"id\":%llu", (unsigned long long *)&application.id);
    }

    char *name_str = strstr(json, "\"applicant_name\":");
    if (name_str) {
        char *start = strchr(name_str, '"') + 1;
        char *end = strchr(start, '"');
        if (end) {
            size_t len = end - start;
            if (len < sizeof(application.applicant_name)) {
                strncpy(application.applicant_name, start, len);
            }
        }
    }

    // Process application
    underwriting_decision_t decision;
    au_error_t result = au_process_application(&application, &decision);

    if (result != AU_SUCCESS) {
        return create_error_response(response, 500, "Failed to process application");
    }

    // Create response
    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"application_id\":%llu,\"decision\":\"%s\",\"risk_score\":%.3f,\"premium\":%.2f}",
             (unsigned long long)application.id,
             au_decision_string(decision.decision),
             decision.risk_score,
             decision.calculated_premium);

    response->status_code = 201;
    strcpy(response->status_text, HTTP_201_CREATED);
    strcpy(response->content_type, "application/json");
    strcpy(response->body, json_response);
    response->body_length = strlen(response->body);

    return AU_SUCCESS;
}

/**
 * @brief Handle decision retrieval
 */
static au_error_t handle_decision_retrieval(const http_request_t *request, http_response_t *response) {
    // Extract application ID from path
    uint64_t application_id = 0;
    if (sscanf(request->path, ENDPOINT_DECISIONS "/%llu", (unsigned long long *)&application_id) != 1) {
        return create_error_response(response, 400, "Invalid application ID");
    }

    // Retrieve decision (placeholder - in real implementation would query database/cache)
    underwriting_decision_t decision = {0};
    decision.application_id = application_id;
    decision.decision = DECISION_APPROVED;
    decision.risk_score = 0.3f;
    decision.calculated_premium = 1500.0f;
    strcpy(decision.explanation, "Low risk application");

    char json_response[1024];
    snprintf(json_response, sizeof(json_response),
             "{\"application_id\":%llu,\"decision\":\"%s\",\"risk_score\":%.3f,\"premium\":%.2f,\"explanation\":\"%s\"}",
             (unsigned long long)decision.application_id,
             au_decision_string(decision.decision),
             decision.risk_score,
             decision.calculated_premium,
             decision.explanation);

    return create_json_response(response, json_response);
}

/**
 * @brief Handle metrics request
 */
static au_error_t handle_metrics_request(http_response_t *response) {
    char json_response[1024];

    // Get system metrics (placeholder)
    size_t active_connections = api_context->active_connections;
    time_t uptime = time(NULL) - api_context->start_time;

    snprintf(json_response, sizeof(json_response),
             "{\"uptime\":%ld,\"active_connections\":%zu,\"total_requests\":0}",
             uptime, active_connections);

    return create_json_response(response, json_response);
}

/**
 * @brief Send HTTP response
 */
static au_error_t send_http_response(int client_socket, const http_response_t *response) {
    char header[1024];

    // Create HTTP header
    snprintf(header, sizeof(header),
             "HTTP/1.1 %d %s\r\n"
             "Content-Type: %s\r\n"
             "Content-Length: %zu\r\n"
             "Connection: close\r\n"
             "\r\n",
             response->status_code, response->status_text,
             response->content_type, response->body_length);

    // Send header
    if (send(client_socket, header, strlen(header), 0) < 0) {
        return AU_ERROR_NETWORK;
    }

    // Send body
    if (response->body_length > 0) {
        if (send(client_socket, response->body, response->body_length, 0) < 0) {
            return AU_ERROR_NETWORK;
        }
    }

    return AU_SUCCESS;
}

/**
 * @brief Create JSON response
 */
static au_error_t create_json_response(http_response_t *response, const char *json_data) {
    response->status_code = 200;
    strcpy(response->status_text, HTTP_200_OK);
    strcpy(response->content_type, "application/json");
    strcpy(response->body, json_data);
    response->body_length = strlen(response->body);

    return AU_SUCCESS;
}

/**
 * @brief Create error response
 */
static au_error_t create_error_response(http_response_t *response, int status_code, const char *message) {
    char json_response[256];
    snprintf(json_response, sizeof(json_response), "{\"error\":\"%s\"}", message);

    response->status_code = status_code;
    switch (status_code) {
        case 400:
            strcpy(response->status_text, HTTP_400_BAD_REQUEST);
            break;
        case 404:
            strcpy(response->status_text, HTTP_404_NOT_FOUND);
            break;
        case 405:
            strcpy(response->status_text, HTTP_405_METHOD_NOT_ALLOWED);
            break;
        case 500:
            strcpy(response->status_text, HTTP_500_INTERNAL_ERROR);
            break;
        case 503:
            strcpy(response->status_text, HTTP_503_SERVICE_UNAVAILABLE);
            break;
        default:
            strcpy(response->status_text, HTTP_500_INTERNAL_ERROR);
            break;
    }

    strcpy(response->content_type, "application/json");
    strcpy(response->body, json_response);
    response->body_length = strlen(response->body);

    return AU_SUCCESS;
}