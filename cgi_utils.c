#include "cgi_utils.h"
#include <string.h>  // Add this for strdup

// CGI environment variables
char *cgi_method = NULL;
char *cgi_query_string = NULL;
char *cgi_content_type = NULL;
char *cgi_content_length = NULL;
char *cgi_remote_addr = NULL;

// Internal storage for parameters
#define MAX_PARAMS 50
#define MAX_PARAM_LEN 256
static char param_names[MAX_PARAMS][MAX_PARAM_LEN];
static char param_values[MAX_PARAMS][MAX_PARAM_LEN];
static int param_count = 0;

// Initialize CGI environment
void cgi_init(void) {
    cgi_method = getenv("REQUEST_METHOD");
    cgi_query_string = getenv("QUERY_STRING");
    cgi_content_type = getenv("CONTENT_TYPE");
    cgi_content_length = getenv("CONTENT_LENGTH");
    cgi_remote_addr = getenv("REMOTE_ADDR");

    // Parse GET parameters
    if (cgi_query_string) {
        char *query = strdup(cgi_query_string);
        char *token = strtok(query, "&");
        while (token && param_count < MAX_PARAMS) {
            char *eq = strchr(token, '=');
            if (eq) {
                *eq = '\0';
                url_decode(token);
                url_decode(eq + 1);
                strncpy(param_names[param_count], token, MAX_PARAM_LEN - 1);
                strncpy(param_values[param_count], eq + 1, MAX_PARAM_LEN - 1);
                param_count++;
            }
            token = strtok(NULL, "&");
        }
        free(query);
    }

    // Parse POST data if available
    if (cgi_method && strcmp(cgi_method, "POST") == 0 && cgi_content_length) {
        int len = atoi(cgi_content_length);
        if (len > 0 && len < 1024 * 1024) { // 1MB limit
            char *post_data = malloc(len + 1);
            if (post_data) {
                fread(post_data, 1, len, stdin);
                post_data[len] = '\0';

                // Parse POST parameters (assuming application/x-www-form-urlencoded)
                char *token = strtok(post_data, "&");
                while (token && param_count < MAX_PARAMS) {
                    char *eq = strchr(token, '=');
                    if (eq) {
                        *eq = '\0';
                        url_decode(token);
                        url_decode(eq + 1);
                        strncpy(param_names[param_count], token, MAX_PARAM_LEN - 1);
                        strncpy(param_values[param_count], eq + 1, MAX_PARAM_LEN - 1);
                        param_count++;
                    }
                    token = strtok(NULL, "&");
                }
                free(post_data);
            }
        }
    }
}

// Get parameter value by name
char *cgi_get_param(const char *name) {
    for (int i = 0; i < param_count; i++) {
        if (strcmp(param_names[i], name) == 0) {
            return param_values[i];
        }
    }
    return NULL;
}

// URL decode a string (simplified version)
void url_decode(char *str) {
    char *src = str;
    char *dst = str;

    while (*src) {
        if (*src == '+') {
            *dst++ = ' ';
            src++;
        } else if (*src == '%' && isxdigit(*(src+1)) && isxdigit(*(src+2))) {
            char hex[3] = {*(src+1), *(src+2), '\0'};
            *dst++ = (char)strtol(hex, NULL, 16);
            src += 3;
        } else {
            *dst++ = *src++;
        }
    }
    *dst = '\0';
}

// HTML escape special characters
void html_escape(const char *input, char *output, size_t max_len) {
    size_t i = 0, j = 0;

    while (input[i] && j < max_len - 1) {
        switch (input[i]) {
            case '<':
                if (j + 4 < max_len) {
                    strcpy(&output[j], "&lt;");
                    j += 4;
                }
                break;
            case '>':
                if (j + 4 < max_len) {
                    strcpy(&output[j], "&gt;");
                    j += 4;
                }
                break;
            case '&':
                if (j + 5 < max_len) {
                    strcpy(&output[j], "&amp;");
                    j += 5;
                }
                break;
            case '"':
                if (j + 6 < max_len) {
                    strcpy(&output[j], "&quot;");
                    j += 6;
                }
                break;
            case '\'':
                if (j + 6 < max_len) {
                    strcpy(&output[j], "&#39;");
                    j += 6;
                }
                break;
            default:
                output[j++] = input[i];
                break;
        }
        i++;
    }
    output[j] = '\0';
}

// Get client IP address
const char *get_client_ip(void) {
    return cgi_remote_addr ? cgi_remote_addr : "unknown";
}

// Log access for debugging
void log_access(const char *page, const char *action) {
    FILE *log = fopen("access.log", "a");
    if (log) {
        fprintf(log, "[%s] %s - %s - %s\n",
                get_client_ip(), page, action,
                cgi_remote_addr ? cgi_remote_addr : "unknown");
        fclose(log);
    }
}