# Project 5: Production Daemon - File Server

## 📖 Overview

Integrate all concepts into a production-ready daemon. This teaches:
- Daemonization process
- Signal handling for graceful shutdown
- Combining file I/O, processes, IPC, and networking
- Production code patterns

**Learning outcomes:**
- How to write proper Unix daemons
- Handling multiple concurrent clients
- Logging and monitoring
- Process lifecycle management

---

## 🎯 Project Goals

**File Server Daemon:**
- Listens on TCP port 9090
- Accepts client connections
- Protocol: clients request files by path
- Server returns file contents
- Logs all activity
- Graceful shutdown on SIGTERM

**Features:**
- Runs in background (detached from terminal)
- PID file for process management
- Syslog integration
- Statistics tracking
- Rate limiting (prevent abuse)

---

## 💻 Daemon Architecture

### Daemonization Steps

```c
// Step 1: Fork and let parent exit
// Step 2: Create new session (setsid)
// Step 3: Change working directory to /
// Step 4: Close stdin/stdout/stderr
// Step 5: Reopen to /dev/null or log file
// Step 6: Set umask
// Step 7: Write PID to file
```

### Server Structure

```
main()
├─ Parse arguments (port, logfile)
├─ Daemonize
├─ Create listening socket
├─ Setup signal handlers
├─ Main loop:
│  ├─ accept() connection
│  ├─ fork() child
│  └─ Child handles client:
│     ├─ Read requested filename
│     ├─ Open file (with security checks)
│     ├─ Send file contents
│     ├─ Close connection
│     ├─ Log transaction
│     └─ exit()
└─ Cleanup on SIGTERM
```

---

## 📋 Protocol Design

```
CLIENT → SERVER:
  GET /path/to/file

SERVER → CLIENT:
  [error or file contents]
```

---

## 🧪 Testing

```bash
# Build
make

# Start daemon
./file_server --port 9090 --logfile server.log

# Connect as client
nc 127.0.0.1 9090
GET /etc/hostname
[returns hostname file contents]

# Check logs
tail -f server.log

# Send SIGTERM to gracefully shutdown
pkill -TERM file_server
```

---

## 📖 Key Concepts

### Signal Handling in Daemons

```c
volatile int shutdown_flag = 0;

void handle_sigterm(int sig) {
    shutdown_flag = 1;  // Set flag
    // Don't do complex stuff in signal handler!
}

// In main loop
if (shutdown_flag) {
    cleanup();
    exit(0);
}
```

### Logging from Daemon

```c
#include <syslog.h>

openlog("file_server", LOG_PID, LOG_DAEMON);

syslog(LOG_INFO, "Client connected from %s", client_ip);
syslog(LOG_ERR, "Failed to open file: %s", filename);

closelog();
```

### Security Considerations

- Don't serve files outside allowed directory
- Check file permissions
- Validate client requests
- Limit request size
- Rate limit per IP
- Audit all access

---

## 📚 Concepts Integrated

This project combines ALL Week50 learning:

1. **File I/O** - Open and serve files
2. **Process Management** - Fork child per client
3. **IPC & Signals** - Handle SIGTERM, coordinate shutdown
4. **Networking** - TCP server for clients
5. **Production Quality** - Daemonization, logging, error handling

---

## 🚀 Extensions

1. **Multiple file types:** MIME type detection
2. **Caching:** Cache frequently accessed files
3. **Compression:** gzip compression option
4. **Authentication:** Client verification
5. **Monitoring:** Real-time statistics via UDP
6. **Systemd:** Create systemd service file

---

**End of Week50! Time for open-source contributions!**
