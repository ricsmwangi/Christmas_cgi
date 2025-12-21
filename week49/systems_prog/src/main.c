#include "log.h"
#include "event_loop.h"
#include "ipc.h"
#include "supervisor.h"
#include <unistd.h>
#include <string.h>
int main(int argc, char **argv){
    log_set_level(LOG_DEBUG);
    log_msg(LOG_INFO, "sysprog starting");
    int p[2]; if(pipe(p)==0){ write(p[1], "hello from pipe\n", 16); close(p[1]); evloop_add_fd(p[0]); }
    int srv = ipc_server_start("/tmp/sysprog.sock");
    if(srv>=0) evloop_add_fd(srv);
    if(argc>1){ supervisor_start(&argv[1]); }
    evloop_run();
    return 0;
}
