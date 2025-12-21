#ifndef IPC_H
#define IPC_H
int ipc_server_start(const char *path);
int ipc_client_connect(const char *path);
#endif
