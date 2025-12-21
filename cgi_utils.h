#ifndef CGI_UTILS_H
#define CGI_UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// CGI environment variables
extern char *cgi_method;
extern char *cgi_query_string;
extern char *cgi_content_type;
extern char *cgi_content_length;
extern char *cgi_remote_addr;

// Initialize CGI environment
void cgi_init(void);

// Get parameter from GET or POST data
char *cgi_get_param(const char *name);

// URL decode a string
void url_decode(char *str);

// HTML escape special characters
void html_escape(const char *input, char *output, size_t max_len);

// Get client IP address
const char *get_client_ip(void);

// Log access for debugging
void log_access(const char *page, const char *action);

#endif // CGI_UTILS_H