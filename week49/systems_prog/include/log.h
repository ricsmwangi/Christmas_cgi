#ifndef LOG_H
#define LOG_H
#include <stdio.h>
#include <time.h>

typedef enum { LOG_DEBUG, LOG_INFO, LOG_WARN, LOG_ERROR } log_level_t;
void log_set_level(log_level_t lvl);
void log_msg(log_level_t lvl, const char *fmt, ...);
#endif
