****# Systems Programming Fundamentals

## 🔍 Part 1: File Descriptors & File I/O

****### What is a File Descriptor?

A **file descriptor (fd)** is a small non-negative integer that the kernel uses to track open files/resources for a process.

**Standard descriptors (always available):**
- `0` = `STDIN_FILENO` (standard input)
- `1` = `STDOUT_FILENO` (standard output)
- `2` = `STDERR_FILENO` (standard error)

**Key insight:** Everything in Unix is a file—regular files, directories, sockets, pipes, devices. FDs unify access.

---

### Syscall: `open()`

Opens a file and returns a file descriptor.

```c
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

int fd = open(filename, flags, mode);
```

**Parameters:**
- `filename`: Path to file
- `flags`: How to open (bitwise OR combinations)
  - `O_RDONLY` - read only
  - `O_WRONLY` - write only
  - `O_RDWR` - read and write
  - `O_CREAT` - create if doesn't exist
  - `O_APPEND` - append to end
  - `O_TRUNC` - truncate to zero length
- `mode`: Permissions if creating (e.g., `0644` = rw-r--r--)

**Returns:** FD on success, `-1` on error

**Example:**
```c
// Open existing file for reading
int fd = open("data.txt", O_RDONLY);
if (fd == -1) {
    perror("open");  // Prints error message
    exit(EXIT_FAILURE);
}

// Create new file for writing (rw-r--r--)
int fd = open("output.txt", O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

**Why flags matter:**
- `O_CREAT` alone won't truncate; use `O_TRUNC` if you want fresh file
- `O_APPEND` automatically seeks to end before each write
- Race condition: File could be deleted between check and open; use `O_EXCL` for safety

---

### Syscall: `read()`

Reads data from a file descriptor.

```c
#include <unistd.h>

ssize_t bytes_read = read(int fd, void *buf, size_t count);
```

**Parameters:**
- `fd`: File descriptor
- `buf`: Buffer to store data (must be allocated!)
- `count`: Max bytes to read

**Returns:** 
- Number of bytes read (>0)
- `0` = end of file
- `-1` = error

**Key points:**
- `read()` doesn't null-terminate; you must handle that
- Partial reads are normal (especially on pipes/sockets)
- Always check return value
- Short reads don't mean EOF—loop until 0 bytes

**Example:**
```c
char buffer[1024];
ssize_t n = read(fd, buffer, sizeof(buffer) - 1);
if (n == -1) {
    perror("read");
} else if (n == 0) {
    printf("End of file\n");
} else {
    buffer[n] = '\0';  // Null-terminate
    printf("Read %ld bytes: %s\n", n, buffer);
}
```

---

### Syscall: `write()`

Writes data to a file descriptor.

```c
#include <unistd.h>

ssize_t bytes_written = write(int fd, const void *buf, size_t count);
```

**Returns:**
- Number of bytes written
- May be less than requested (especially on pipes/sockets)
- `-1` on error

**Example:**
```c
const char *msg = "Hello, World!\n";
ssize_t n = write(1, msg, strlen(msg));  // Write to stdout
if (n == -1) {
    perror("write");
}
```

---

### Syscall: `close()`

Closes a file descriptor.

```c
#include <unistd.h>

int result = close(int fd);
```

**Returns:** `0` on success, `-1` on error

**Important:**
- Always close files when done
- Frees the FD (can be reused)
- Failing to close = resource leak

---

### Syscall: `stat()` / `fstat()`

Gets file metadata (size, permissions, timestamps).

```c
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

int result = stat(const char *pathname, struct stat *statbuf);
int result = fstat(int fd, struct stat *statbuf);
```

**struct stat fields:**
```c
struct stat {
    off_t st_size;      // File size in bytes
    mode_t st_mode;     // Permissions and file type
    uid_t st_uid;       // Owner user ID
    time_t st_mtime;    // Last modification time
    // ... more fields
};
```

**Example:**
```c
struct stat sb;
if (stat("data.txt", &sb) == -1) {
    perror("stat");
} else {
    printf("File size: %ld bytes\n", sb.st_size);
    printf("Mode: %o (octal)\n", sb.st_mode);
}
```

---

## 🔄 Part 2: Process Management

### Understanding Process Lifecycle

**Process states:**
- **Running:** CPU executing
- **Ready:** Waiting for CPU
- **Blocked:** Waiting for I/O
- **Zombie:** Exited but parent hasn't reaped
- **Stopped:** Suspended by signal

---

### Syscall: `fork()`

Creates a new process (child) by copying the parent.

```c
#include <unistd.h>

pid_t pid = fork();
```

**Returns:**
- Child process: `0`
- Parent process: Child's PID (>0)
- Error: `-1`

**Critical behavior:**
- `fork()` returns TWICE (once in parent, once in child)
- Child gets copy of parent's memory (but separate address space)
- Both processes continue from same point in code
- Child inherits open file descriptors

**Example:**
```c
pid_t pid = fork();

if (pid == -1) {
    perror("fork");
    exit(EXIT_FAILURE);
} else if (pid == 0) {
    // Child process
    printf("I am child, PID: %d\n", getpid());
} else {
    // Parent process
    printf("I am parent, child PID: %d\n", pid);
}
```

**Key gotcha:**
```c
printf("Before fork\n");  // Printed once
fork();
printf("After fork\n");   // Printed TWICE (parent and child)
```

---

### Syscall: `execve()`

Replaces current process with new program.

```c
#include <unistd.h>

int execve(const char *filename, char *const argv[], char *const envp[]);
```

**Parameters:**
- `filename`: Path to executable
- `argv`: Argument array (must end with NULL)
- `envp`: Environment variables (usually use `environ`)

**Returns:** Never returns on success (process replaced)
Returns `-1` on error

**Family of exec functions (wrappers around execve):**
- `execl()` - list of args
- `execv()` - vector of args
- `execvp()` - search PATH for program
- `execlp()` - list + search PATH

**Example:**
```c
// Using execv
char *args[] = {"ls", "-la", "/home", NULL};
execv("/bin/ls", args);
perror("execv");  // Only reached if exec fails
```

---

### Syscall: `wait()` / `waitpid()`

Parent waits for child to exit and retrieves status.

```c
#include <sys/wait.h>

pid_t wait(int *status);
pid_t waitpid(pid_t pid, int *status, int options);
```

**Parameters:**
- `pid`: Which child to wait for
  - `-1` = any child
  - `>0` = specific PID
- `status`: Exit status (optional, can be NULL)
- `options`: Flags like `WNOHANG` (non-blocking)

**Returns:** PID of child, `-1` on error, `0` if `WNOHANG` + no children ready

**Example:**
```c
pid_t pid = fork();

if (pid == 0) {
    // Child: run program
    execlp("sleep", "sleep", "5", NULL);
} else {
    // Parent: wait for child
    int status;
    waitpid(pid, &status, 0);
    
    if (WIFEXITED(status)) {
        int exit_code = WEXITSTATUS(status);
        printf("Child exited with code: %d\n", exit_code);
    }
}
```

**Macros to extract status:**
- `WIFEXITED(status)` - Did child exit normally?
- `WEXITSTATUS(status)` - What was exit code?
- `WIFSIGNALED(status)` - Was child killed by signal?

---

## 📡 Part 3: Pipes & Signals

### Syscall: `pipe()`

Creates unidirectional communication channel.

```c
#include <unistd.h>

int pipe(int pipefd[2]);
```

**Parameters:**
- `pipefd[0]` - read end (filled by pipe())
- `pipefd[1]` - write end (filled by pipe())

**Returns:** `0` on success, `-1` on error

**Important:**
- Typically use with `fork()` to share between processes
- Write to `pipefd[1]`, read from `pipefd[0]`
- Close unused ends in child/parent
- Reading from closed write end returns 0 (EOF)
- Writing to closed read end triggers `SIGPIPE`

**Example:**
```c
int pipefd[2];
if (pipe(pipefd) == -1) {
    perror("pipe");
    exit(EXIT_FAILURE);
}

pid_t pid = fork();
if (pid == 0) {
    // Child: read from pipe
    close(pipefd[1]);  // Close write end (not using)
    char buf[100];
    read(pipefd[0], buf, sizeof(buf));
    printf("Child received: %s\n", buf);
    close(pipefd[0]);
} else {
    // Parent: write to pipe
    close(pipefd[0]);  // Close read end (not using)
    const char *msg = "Hello from parent!";
    write(pipefd[1], msg, strlen(msg));
    close(pipefd[1]);
}
```

---

### Signals: Software Interrupts

**What is a signal?**
- Asynchronous event notification
- Examples: `SIGTERM` (terminate), `SIGKILL` (kill), `SIGUSR1` (user-defined)
- Like interrupts for processes

**Common signals:**
- `SIGTERM` (15) - Graceful termination request
- `SIGKILL` (9) - Forced kill (can't be caught)
- `SIGINT` (2) - Interrupt (Ctrl+C)
- `SIGUSR1` (10) - User-defined 1
- `SIGUSR2` (12) - User-defined 2
- `SIGCHLD` (17) - Child process changed state

---

### Syscall: `signal()` (Old API, use `sigaction()`)

Registers handler for signal.

```c
#include <signal.h>

typedef void (*sighandler_t)(int);
sighandler_t signal(int signum, sighandler_t handler);
```

**Example:**
```c
void handle_sigterm(int sig) {
    printf("Received SIGTERM, cleaning up...\n");
    // Cleanup code
    exit(0);
}

signal(SIGTERM, handle_sigterm);
```

---

### Syscall: `sigaction()` (Modern API)

Better signal handling with more control.

```c
#include <signal.h>

int sigaction(int signum, const struct sigaction *act,
              struct sigaction *oldact);

struct sigaction {
    void (*sa_handler)(int);
    void (*sa_sigaction)(int, siginfo_t *, void *);
    sigset_t sa_mask;
    int sa_flags;
};
```

**Example:**
```c
struct sigaction sa;
memset(&sa, 0, sizeof(sa));
sa.sa_handler = handle_sigterm;
sigemptyset(&sa.sa_mask);
sa.sa_flags = 0;

sigaction(SIGTERM, &sa, NULL);
```

---

## 🌐 Part 4: Sockets & Networking

### Socket Fundamentals

**What is a socket?**
- Endpoint for network communication
- Like a "virtual cable" between processes/machines
- Stream (TCP) or datagram (UDP)

**Socket communication flow:**

**Server:**
```
socket() → bind() → listen() → accept() → read/write → close()
```

**Client:**
```
socket() → connect() → read/write → close()
```

---

### Syscall: `socket()`

Creates a socket endpoint.

```c
#include <sys/socket.h>

int sockfd = socket(int domain, int type, int protocol);
```

**Parameters:**
- `domain`: `AF_INET` (IPv4), `AF_INET6` (IPv6), `AF_UNIX` (local)
- `type`: `SOCK_STREAM` (TCP), `SOCK_DGRAM` (UDP)
- `protocol`: Usually `0` (default for type)

**Returns:** Socket FD, `-1` on error

**Example:**
```c
int sockfd = socket(AF_INET, SOCK_STREAM, 0);
if (sockfd == -1) {
    perror("socket");
}
```

---

### Syscall: `bind()`

Binds socket to address (server-side).

```c
#include <sys/socket.h>
#include <netinet/in.h>

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Example:**
```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));

server_addr.sin_family = AF_INET;
server_addr.sin_addr.s_addr = htonl(INADDR_ANY);  // Listen on all IPs
server_addr.sin_port = htons(8080);  // Port 8080

if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("bind");
}
```

---

### Syscall: `listen()`

Marks socket as accepting connections.

```c
#include <sys/socket.h>

int listen(int sockfd, int backlog);
```

**Parameters:**
- `backlog`: Max queued connections (typically 5-10)

---

### Syscall: `accept()`

Accepts incoming connection.

```c
#include <sys/socket.h>

int client_fd = accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
```

**Returns:** New FD for client connection

**Example:**
```c
struct sockaddr_in client_addr;
socklen_t addr_len = sizeof(client_addr);

int client_fd = accept(sockfd, (struct sockaddr *)&client_addr, &addr_len);
if (client_fd == -1) {
    perror("accept");
} else {
    printf("Client connected\n");
    // Use client_fd for read/write
}
```

---

### Syscall: `connect()`

Connects to server (client-side).

```c
#include <sys/socket.h>

int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
```

**Example:**
```c
struct sockaddr_in server_addr;
memset(&server_addr, 0, sizeof(server_addr));
server_addr.sin_family = AF_INET;
server_addr.sin_port = htons(8080);
inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("connect");
}
```

---

## 🛡️ Error Handling Best Practices

### Always check return values

```c
// BAD
int fd = open("file.txt", O_RDONLY);
read(fd, buf, 100);

// GOOD
int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    perror("open");  // Prints error message + context
    exit(EXIT_FAILURE);
}

ssize_t n = read(fd, buf, 100);
if (n == -1) {
    perror("read");
    close(fd);
    exit(EXIT_FAILURE);
}
```

### Using `errno` and `perror()`

```c
#include <errno.h>
#include <string.h>

int fd = open("nonexistent.txt", O_RDONLY);
if (fd == -1) {
    // Method 1: perror (simple)
    perror("open");  // Prints: "open: No such file or directory"
    
    // Method 2: strerror (customizable)
    fprintf(stderr, "Error opening file: %s\n", strerror(errno));
    
    // Method 3: errno directly
    if (errno == ENOENT) {
        printf("File not found\n");
    }
}
```

---

## 📋 Common Error Codes

- `ENOENT` - No such file or directory
- `EACCES` - Permission denied
- `EAGAIN` - Resource temporarily unavailable (non-blocking)
- `EINTR` - Interrupted system call
- `EBADF` - Bad file descriptor
- `EPIPE` - Broken pipe

---

## 🎯 Summary Table

| Syscall | Purpose | Returns | Error |
|---------|---------|---------|-------|
| `open()` | Open/create file | FD | -1 |
| `read()` | Read from FD | bytes read | -1 |
| `write()` | Write to FD | bytes written | -1 |
| `close()` | Close FD | 0 | -1 |
| `stat()` | Get file metadata | 0 | -1 |
| `fork()` | Create process | child PID or 0 | -1 |
| `execve()` | Replace process | never | -1 |
| `wait()` | Wait for child | child PID | -1 |
| `pipe()` | Create pipe | 0 | -1 |
| `socket()` | Create socket | FD | -1 |
| `bind()` | Bind socket | 0 | -1 |
| `listen()` | Listen for connections | 0 | -1 |
| `accept()` | Accept connection | FD | -1 |
| `connect()` | Connect to server | 0 | -1 |

---

## 🧠 Key Principles

1. **Everything is a file** - FDs unify access to files, pipes, sockets, devices
2. **Always check return values** - Syscalls can fail (permissions, resources, network)
3. **Resource cleanup** - Close FDs, terminate processes, free memory
4. **Asynchronous events** - Signals interrupt normal execution
5. **Processes are isolated** - Use pipes/sockets for communication
6. **Network is unreliable** - Partial reads/writes, timeouts, connection drops

---

**Next: Study each project to see these concepts in action!**
