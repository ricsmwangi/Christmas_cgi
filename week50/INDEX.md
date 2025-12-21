# Week50: Systems Programming Curriculum - START HERE 🚀

## 📚 Quick Navigation

### 🎯 Getting Started (Read First)

1. **[SYSTEMS_PROGRAMMING_WEEK50.md](./SYSTEMS_PROGRAMMING_WEEK50.md)** - Overview and learning path
2. **[WEEK50_ROADMAP.md](./WEEK50_ROADMAP.md)** - Timeline and schedule
3. **[FUNDAMENTALS.md](./FUNDAMENTALS.md)** - Complete study guide with all syscalls explained
4. **[GIT_PUSH_GUIDE.md](./GIT_PUSH_GUIDE.md)** - Professional Git workflow

---

## 🏗️ Projects (Complete with Code & Documentation)

### Project 1: File I/O Mastery (Days 1-2)
**📁 [01_file_io/](./01_file_io/)**

Build a professional file copy utility with progress tracking.

- **Syscalls:** `open()`, `read()`, `write()`, `close()`, `stat()`, `fchmod()`
- **Skills:** File descriptors, buffering, error handling, performance optimization
- **Deliverables:** mycp.c (500+ lines with comments), README.md, Makefile
- **Status:** ✅ Complete with full source code

**Quick Start:**
```bash
cd 01_file_io
make
./mycp source.txt destination.txt
```

---

### Project 2: Process Management & Shells (Days 3-4)
**📁 [02_process_management/](./02_process_management/)**

Build a command-line shell using fork and exec.

- **Syscalls:** `fork()`, `execvp()`, `wait()`, `signal()`
- **Skills:** Process creation, concurrent execution, background jobs
- **Deliverables:** simple_shell.c (600+ lines with comments), README.md, Makefile
- **Status:** ✅ Complete with full source code

**Quick Start:**
```bash
cd 02_process_management
make
./simple_shell
> ls -l
> pwd
> cd /tmp
> exit
```

---

### Project 3: IPC & Signals (Day 5 - Morning)
**📁 [03_ipc_signals/](./03_ipc_signals/)**

Producer-consumer pipeline with inter-process communication.

- **Syscalls:** `pipe()`, `signal()`, `kill()`
- **Skills:** Pipes, asynchronous events, process synchronization
- **Status:** 📋 README with design, code stub ready for implementation
- **Challenge:** Implement producer.c and consumer.c

**Starter Template:**
```c
int pipefd[2];
pipe(pipefd);
// Your implementation here
```

---

### Project 4: Networking Fundamentals (Day 5 - Afternoon)
**📁 [04_networking/](./04_networking/)**

TCP echo server handling multiple concurrent clients.

- **Syscalls:** `socket()`, `bind()`, `listen()`, `accept()`, `connect()`
- **Skills:** Network programming, server architecture, concurrent clients
- **Status:** 📋 README with architecture, code stub ready
- **Challenge:** Implement echo_server.c

**Server Pattern:**
```c
socket() → bind() → listen() → accept() → read/write → close()
```

---

### Project 5: Production Daemon (Days 9-10)
**📁 [05_daemon/](./05_daemon/)**

Integrate all concepts into production-ready file server daemon.

- **Syscalls:** All previous + daemonization APIs
- **Skills:** Production quality code, logging, signal handling, graceful shutdown
- **Status:** 📋 README with daemon architecture, code stub ready
- **Challenge:** Implement file_server.c with logging and signal handling

---

## 📖 Study Materials

### Complete Syscall Reference
**[FUNDAMENTALS.md](./FUNDAMENTALS.md)** contains:

✅ File I/O section - 45 min read
```
- File descriptors explained
- open() with all flags
- read() and write() behavior
- stat() for metadata
- Error handling patterns
```

✅ Process Management section - 45 min read
```
- fork() model and return behavior
- execve() family and when to use each
- wait() and waitpid() patterns
- Zombie processes
- Exit codes and status macros
```

✅ Pipes & Signals section - 45 min read
```
- Pipe creation and usage
- Blocking I/O behavior
- Signal handling with signal()
- sigaction() for advanced control
- Signal safety
```

✅ Sockets & Networking section - 45 min read
```
- Socket fundamentals
- Server flow (bind→listen→accept)
- Client flow (socket→connect)
- Address structures
- Common networking patterns
```

✅ Error Handling section - 15 min read
```
- Why check return values
- errno and perror()
- Common error codes
- Best practices
```

### Professional Git & Portfolio
**[GIT_PUSH_GUIDE.md](./GIT_PUSH_GUIDE.md)** teaches:

- Clean, meaningful commits
- Professional commit message format
- When and how to push
- GitHub portfolio optimization
- Open-source contribution workflow

---

## 🎯 Week50 Timeline

```
Dec 4 (Today)   - Completed! Setup + FUNDAMENTALS reading
Dec 5           - Mom's graduation 🎓
Dec 6-7 (Sat-Sun)   - Project 1 (File I/O - mycp)
Dec 8-9 (Mon-Tue)   - Project 2 (Process Mgmt - shell)
Dec 10 (Wed)        - Projects 3-4 (IPC/Networking)
Dec 11 (Thu)        - Project 5 (Daemon integration)
Dec 12 (Fri)        - Polish, document, final push
Dec 13+             - Open-source contributions!
```

---

## 💡 How to Use This Curriculum

### Option A: Step-by-Step (Recommended)

1. Read [WEEK50_ROADMAP.md](./WEEK50_ROADMAP.md) for full schedule
2. For each project:
   - Read relevant section in [FUNDAMENTALS.md](./FUNDAMENTALS.md)
   - Read project README (01_file_io/README.md, etc.)
   - Study provided source code
   - Build and test
   - Make improvements
   - Commit and push
3. Follow [GIT_PUSH_GUIDE.md](./GIT_PUSH_GUIDE.md) for commits

### Option B: Deep Dive First

1. Read [FUNDAMENTALS.md](./FUNDAMENTALS.md) completely (3-4 hours)
2. Study all project READMEs
3. Then implement projects with solid understanding

### Option C: Reference Mode

- Use [FUNDAMENTALS.md](./FUNDAMENTALS.md) as reference while coding
- Jump to specific project README when needed
- Refer to [GIT_PUSH_GUIDE.md](./GIT_PUSH_GUIDE.md) for commits

---

## 📊 What's Included

### Complete Project Implementations

- ✅ **01_file_io/mycp.c** - 400+ lines, heavily commented
  - Explains every syscall
  - Shows error handling
  - Demonstrates buffering strategy

- ✅ **02_process_management/simple_shell.c** - 600+ lines, heavily commented
  - Shows fork/exec pattern
  - Demonstrates signal handling
  - Includes zombie process explanation

### Professional Build System

- All projects have `Makefile` with:
  - `make` - build program
  - `make test` - run tests
  - `make clean` - remove binaries
  - `make debug` - debug build

### Comprehensive Documentation

- ✅ SYSTEMS_PROGRAMMING_WEEK50.md - Overview
- ✅ FUNDAMENTALS.md - 3000+ lines of detailed explanations
- ✅ WEEK50_ROADMAP.md - Schedule and timeline
- ✅ GIT_PUSH_GUIDE.md - Professional workflow
- ✅ Each project has detailed README

### Well-Commented Source Code

Every function includes:
- Purpose and parameters
- Syscall explanations
- Return value meanings
- Error handling examples
- Common pitfalls and solutions

---

## 🎓 Learning Outcomes

After completing Week50, you'll understand:

### File I/O
- ✅ File descriptors (FD architecture)
- ✅ POSIX file operations
- ✅ Buffering and performance
- ✅ Error handling patterns

### Process Management
- ✅ Process creation (fork)
- ✅ Process replacement (exec)
- ✅ Process coordination (wait)
- ✅ Process lifecycle and signals

### IPC & Communication
- ✅ Pipes for data flow
- ✅ Signals for events
- ✅ Asynchronous programming
- ✅ Process synchronization

### Networking
- ✅ Socket API
- ✅ Server architecture
- ✅ Client-server model
- ✅ Concurrent connections

### Production Code
- ✅ Error checking and recovery
- ✅ Resource cleanup
- ✅ Logging and monitoring
- ✅ Graceful shutdown

---

## 🚀 Getting Started NOW

### Immediate Action (Next 30 Minutes)

1. Read this file (you're doing it!)
2. Read [SYSTEMS_PROGRAMMING_WEEK50.md](./SYSTEMS_PROGRAMMING_WEEK50.md) (5 min)
3. Skim [FUNDAMENTALS.md](./FUNDAMENTALS.md) introduction (10 min)
4. Read [WEEK50_ROADMAP.md](./WEEK50_ROADMAP.md) (10 min)
5. Review [01_file_io/README.md](./01_file_io/README.md) (5 min)

### Tomorrow (Project 1 Start)

```bash
cd /home/shinigami/CODE/week50/01_file_io
cat README.md          # Understand project
cat mycp.c             # Review code
make                   # Build
./mycp /etc/hostname .  # Test
make test              # Run tests
```

---

## 📝 Important Notes

### Code Quality Standards

Every project follows:
- `-Wall -Wextra -pedantic` (zero warnings)
- Clear variable names
- Comments on complex logic
- Proper error handling
- Resource cleanup
- Professional structure

### Testing Approach

Each project includes:
- Happy path (normal operation)
- Edge cases (boundaries)
- Error cases (what goes wrong)
- Performance (scales well)

### Git Workflow

Every commit:
- Is focused (one feature)
- Has clear message (explains what/why)
- Is tested and working
- Includes related files

---

## ❓ FAQ

**Q: Can I skip projects and just read FUNDAMENTALS.md?**
A: You could, but building projects is where the real learning happens. Code teaches more than reading.

**Q: What if Project 5 is too hard?**
A: Start with Projects 1-4. They're stepping stones. By Project 5, you'll understand everything.

**Q: Should I use additional resources?**
A: Yes! Use `man 2 syscall` for details, but this curriculum is complete and self-contained.

**Q: Can I implement projects differently than shown?**
A: Absolutely! The provided code is ONE way. Explore alternatives. Learning matters more than matching exactly.

**Q: How do I handle compile errors?**
A: Read the error message carefully. Usually it's a typo or missing header. Fix it and try again.

**Q: What if I get stuck on a project?**
A: 1) Re-read the relevant FUNDAMENTALS section 2) Look at code comments 3) Try simpler version first 4) Ask in forums

---

## 🎯 Success Criteria

**By end of Week50, you should be able to:**

- [ ] Explain fork/exec model to someone
- [ ] Write file copy utility without references
- [ ] Build simple shell with built-in commands
- [ ] Implement producer-consumer with pipes
- [ ] Create TCP server with multiple clients
- [ ] Write daemon with graceful shutdown
- [ ] Understand all syscalls used in projects
- [ ] Contribute to systems programming open source

---

## 🏆 Showcase Your Work

After Week50:

1. Push all projects to GitHub
2. Create personal portfolio README showing Week50
3. Link to week50 from main learning-progress README
4. Share on social media (LinkedIn, Twitter)
5. Contribute to open source with these skills
6. Discuss in technical interviews

---

## 📞 Support Resources

### If You Get Stuck:

1. **FUNDAMENTALS.md** - Reference for all concepts
2. **Project README** - Specific guidance
3. **Project Source Code** - Working examples
4. **man pages** - `man 2 fork`, `man 2 socket`, etc.
5. **Compiling Help** - `-Wall -Wextra` shows issues

### Testing Your Work:

```bash
# Compile check
gcc -Wall -Wextra -pedantic *.c

# Run tests
make test

# Debug
gdb ./program

# Check resources
lsof -p $$  # Check open files
ps aux      # Check processes
```

---

## 🎉 Let's Begin!

You now have:
- ✅ Complete curriculum
- ✅ All source code provided
- ✅ Detailed explanations
- ✅ Professional workflow guide
- ✅ Timeline and schedule
- ✅ Testing strategy

**Everything you need to master systems programming!**

### Next Step: Read FUNDAMENTALS.md Section by Section

Start with File I/O (45 minutes), then build Project 1. Enjoy the journey! 🚀

---

**Happy coding! You've got this! 💪**

---

*Week50: Systems Programming Mastery*  
*Created: December 4, 2025*  
*Status: Ready for your learning journey*
