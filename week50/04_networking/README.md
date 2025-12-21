# Project 4: Networking - TCP Echo Server

## 📖 Overview

Build a concurrent TCP echo server that handles multiple clients. This teaches:
- Socket API fundamentals
- TCP/IP networking
- Concurrent connection handling
- Network protocol implementation

**Learning outcomes:**
- Master socket syscalls
- Understand server architecture patterns
- Handle multiple clients with fork
- Network programming best practices

---

## 🎯 Project Goals

**Basic version:**
- Server listens on port 8080
- Accepts client connections
- Echoes back received data
- Closes connection on EOF

**Extended version:**
- Multiple concurrent clients (fork per client)
- Protocol: commands instead of just echo
- Commands: ECHO, FILE, TIME, HELP
- Graceful shutdown on SIGTERM

---

## 💻 Socket Programming

### Server Flow

```
1. socket()      - Create socket endpoint
2. bind()        - Bind to address and port
3. listen()      - Mark as accepting connections
4. accept()      - Wait for client connection (returns new FD)
5. read/write()  - Communicate with client
6. close()       - Close connection
```

### Client Flow

```
1. socket()      - Create socket endpoint
2. connect()     - Connect to server
3. write()       - Send data
4. read()        - Receive data
5. close()       - Close connection
```

---

## 🔧 Implementation Structure

### Server

```c
int server_fd = socket(AF_INET, SOCK_STREAM, 0);
bind(server_fd, (struct sockaddr *)&addr, sizeof(addr));
listen(server_fd, 5);  // Backlog of 5

while (1) {
    int client_fd = accept(server_fd, ...);  // Blocks until client connects
    
    pid_t pid = fork();
    if (pid == 0) {
        // Child: handle client
        close(server_fd);  // Child doesn't need listening socket
        while (read(client_fd, buf, SIZE) > 0) {
            write(client_fd, buf, n);  // Echo
        }
        close(client_fd);
        exit(0);
    } else {
        // Parent: continue accepting
        close(client_fd);  // Parent doesn't need client socket
        waitpid(pid, NULL, 0);  // Reap child
    }
}
```

### Client

```c
int sock = socket(AF_INET, SOCK_STREAM, 0);
connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

// Send data
write(sock, "Hello", 5);

// Receive echo
char buf[256];
read(sock, buf, sizeof(buf));

close(sock);
```

---

## 🧪 Testing

```bash
# Terminal 1: Start server
make
./echo_server 8080

# Terminal 2: Connect with netcat
nc 127.0.0.1 8080
Hello
Hello       ← Echo back
World
World       ← Echo back
(Ctrl+C to exit)

# Or with telnet
telnet 127.0.0.1 8080
```

---

## 📖 Study Materials

See `FUNDAMENTALS.md` - Networking section for:
- Socket API overview
- Address structures (struct sockaddr_in)
- TCP/IP basics
- Concurrent server patterns

---

## 🚀 Extensions

1. **Protocol:** Implement command protocol (not just echo)
2. **Statistics:** Track bytes transferred
3. **Timeout:** Close idle connections
4. **SSL/TLS:** Secure communication
5. **Thread pool:** Use threads instead of fork

---

**Next project: Daemon Integration**
