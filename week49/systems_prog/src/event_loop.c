#include "event_loop.h"
#include "log.h"
#include <sys/select.h>
#include <unistd.h>
#include <errno.h>
#define MAXFDS 64
static int fds[MAXFDS]; static int nfds = 0; static int stop = 0;
bool evloop_add_fd(int fd){ if(nfds>=MAXFDS) return false; fds[nfds++]=fd; return true; }
void evloop_request_stop(void){ stop = 1; }
int evloop_run(void){
    while(!stop){
        fd_set set; FD_ZERO(&set); int maxfd = -1;
        for(int i=0;i<nfds;i++){ FD_SET(fds[i], &set); if(fds[i]>maxfd) maxfd=fds[i]; }
        struct timeval tv = { .tv_sec = 1, .tv_usec = 0 };
        int r = select(maxfd+1, &set, NULL, NULL, &tv);
        if(r < 0){ if(errno==EINTR) continue; log_msg(LOG_ERROR, "select failed: %d", errno); return -1; }
        if(r == 0){ continue; }
        for(int i=0;i<nfds;i++) if(FD_ISSET(fds[i], &set)) { char buf[256]; ssize_t n = read(fds[i], buf, sizeof buf);
            if(n>0){ write(STDOUT_FILENO, buf, n); }
        }
    }
    return 0;
}
