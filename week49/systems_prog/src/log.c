#include "log.h"
#include <stdarg.h>
#include <string.h>
static log_level_t CURRENT = LOG_INFO;
static const char *lvlstr[] = {"DEBUG","INFO","WARN","ERROR"};
void log_set_level(log_level_t lvl){ CURRENT = lvl; }
void log_msg(log_level_t lvl, const char *fmt, ...){
    if(lvl < CURRENT) return;
    time_t t = time(NULL);
    struct tm *ptm = localtime(&t);
    char ts[32]; strftime(ts, sizeof ts, "%Y-%m-%d %H:%M:%S", ptm ? ptm : &(struct tm){0});
    fprintf(stderr, "%s [%s] ", ts, lvlstr[lvl]);
    va_list ap; va_start(ap, fmt); vfprintf(stderr, fmt, ap); va_end(ap);
    fputc('\n', stderr);
}
