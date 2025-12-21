#include "ipc.h"
#include "log.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
int ipc_server_start(const char *path){
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if(s<0){ log_msg(LOG_ERROR, "socket: %d", errno); return -1; }
    struct sockaddr_un addr; memset(&addr,0,sizeof addr); addr.sun_family=AF_UNIX; strncpy(addr.sun_path, path, sizeof addr.sun_path-1);
    unlink(path);
    if(bind(s,(struct sockaddr*)&addr,sizeof addr)<0){ log_msg(LOG_ERROR,"bind: %d", errno); close(s); return -1; }
    if(listen(s, 4)<0){ log_msg(LOG_ERROR,"listen: %d", errno); close(s); return -1; }
    return s;
}
int ipc_client_connect(const char *path){
    int s = socket(AF_UNIX, SOCK_STREAM, 0);
    if(s<0){ log_msg(LOG_ERROR, "socket: %d", errno); return -1; }
    struct sockaddr_un addr; memset(&addr,0,sizeof addr); addr.sun_family=AF_UNIX; strncpy(addr.sun_path, path, sizeof addr.sun_path-1);
    if(connect(s,(struct sockaddr*)&addr,sizeof addr)<0){ log_msg(LOG_ERROR,"connect: %d", errno); close(s); return -1; }
    return s;
}
