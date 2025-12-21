#include "supervisor.h"
#include "log.h"
#include <sys/wait.h>
#include <unistd.h>
#include <errno.h>
int supervisor_start(char *const cmd[]){
    pid_t pid = fork();
    if(pid < 0){ log_msg(LOG_ERROR, "fork: %d", errno); return -1; }
    if(pid == 0){ execvp(cmd[0], cmd); _exit(127); }
    log_msg(LOG_INFO, "spawned pid %d", pid);
    int status;
    while(1){ pid_t r = waitpid(pid, &status, 0);
        if(r<0){ if(errno==EINTR) continue; log_msg(LOG_ERROR, "waitpid: %d", errno); return -1; }
        if(WIFEXITED(status)){ log_msg(LOG_WARN, "child exited %d", WEXITSTATUS(status)); return WEXITSTATUS(status); }
        if(WIFSIGNALED(status)){ log_msg(LOG_WARN, "child signaled %d", WTERMSIG(status)); return -1; }
    }
}
