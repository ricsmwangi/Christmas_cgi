# Project 3: IPC & Signals - Producer-Consumer Pipeline

## 📖 Overview

Build a producer-consumer system using pipes and signals. This teaches:
- Inter-process communication via pipes
- Signal handling and asynchronous events
- Process synchronization without threads
- Data flow coordination

**Learning outcomes:**
- Master `pipe()` syscall
- Understand blocking I/O behavior
- Handle signals from child processes
- Coordinate multiple processes

---

## 🎯 Project Goals

**Basic version:**
- Producer process: reads from file, writes to pipe
- Consumer process: reads from pipe, writes to output
- Both run concurrently

**Extended version:**
- Multiple producers
- Multiple consumers
- Statistics reporting via signals
- Graceful shutdown

---

## 💻 Pipe Architecture

```
Producer Process          Consumer Process
┌──────────────┐         ┌──────────────┐
│ Read file    │         │ Read pipe    │
│ Write pipe   │────────>│ Write output │
└──────────────┘         └──────────────┘
       (parent)                (child)

Pipe:
  Write end (0) ──────> Read end (1)
```

---

## 📚 Key Concepts

### Pipes

```c
#include <unistd.h>

int pipefd[2];
pipe(pipefd);

pipefd[0] = read end
pipefd[1] = write end

// Write to pipe
write(pipefd[1], data, size);

// Read from pipe
read(pipefd[0], buffer, size);
```

### Signal Handling

```c
#include <signal.h>

void handler(int sig) {
    // Handle signal
}

signal(SIGUSR1, handler);  // Register handler

// Send signal from another process
kill(child_pid, SIGUSR1);  // Trigger handler
```

---

## 🔧 Implementation Structure

```c
// Producer: reads from input file, writes to pipe
// - Opens input file
// - Reads chunks
// - Writes to pipe
// - Closes pipe when done

// Consumer: reads from pipe, writes to output
// - Opens output file
// - Reads from pipe
// - Writes to output file
// - Detects EOF on pipe

// Signals: SIGUSR1 for statistics
// - Producer reports: bytes read
// - Consumer reports: bytes written
```

---

## 🧪 Testing

```bash
make
echo "Hello from producer!" > input.txt
./producer input.txt 2>&1 | ./consumer output.txt
cat output.txt
```

---

## 📖 Study Materials

See `FUNDAMENTALS.md` - Pipes & Signals section for detailed explanation of:
- How pipes work (write end → read end)
- Blocking behavior (read on empty pipe blocks)
- Signal delivery and handlers
- Multiple process synchronization

---

**Next project: Networking**
