#ifndef EVENT_LOOP_H
#define EVENT_LOOP_H
#include <stdbool.h>
int evloop_run(void);
void evloop_request_stop(void);
bool evloop_add_fd(int fd);
#endif
