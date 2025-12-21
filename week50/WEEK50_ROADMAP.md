# Week50 Study Roadmap & Schedule

## 📅 Week50 Timeline (Dec 4-12, 2025)

### Dec 4 (Today) - Planning & Foundation
- ✅ Reviewed graduation website (live Dec 5)
- ✅ Created week50 structure
- ✅ Read FUNDAMENTALS.md (30-60 min)
- ⏳ **Action:** Rest, enjoy mom's graduation!

### Dec 5 - Graduation Day & Rest
- 🎓 Mom's graduation!
- Enjoy the day
- Relax - no coding

### Dec 6-7 (Saturday-Sunday) - Project 1: File I/O
**Focus:** File descriptors, open/read/write/close

**Schedule:**
- Saturday morning: Study FUNDAMENTALS.md - File Descriptors section
- Saturday afternoon: Build mycp basic version (copy file)
- Saturday evening: Add progress bar
- Sunday morning: Add error handling
- Sunday afternoon: Test with large files, git commit

**Commits:**
```
Feat: add mycp with basic copy functionality
Feat: add progress bar to mycp (bytes/sec)
Fix: handle EOF and partial writes
Docs: add README for mycp project
```

**What to understand:**
- File descriptors (0, 1, 2, and beyond)
- `open()` flags: O_RDONLY, O_WRONLY, O_CREAT, O_TRUNC
- `read()` and `write()` return partial data
- Always check return values
- File permissions and `stat()`

---

### Dec 8-9 (Monday-Tuesday) - Project 2: Process Management
**Focus:** fork(), exec(), wait() - the Unix process model

**Schedule:**
- Monday morning: Study FUNDAMENTALS.md - Process Management section
- Monday afternoon: Build shell that forks child per command
- Monday evening: Add built-in commands (cd, pwd, exit)
- Tuesday morning: Add background job support (`&`)
- Tuesday afternoon: Test with various commands, polish

**Commits:**
```
Feat: add simple_shell with fork/execvp
Feat: add built-in commands (cd, pwd)
Feat: add background job execution
Feat: add signal handling for Ctrl+C
Docs: add README for simple_shell project
```

**What to understand:**
- fork() returns twice (parent gets PID, child gets 0)
- Why you need fork + exec (not just exec)
- Zombie processes and why wait() is critical
- Process lifecycle and signals
- Background jobs vs foreground

---

### Dec 10 (Wednesday) - Projects 3 & 4: IPC/Networking
**Focus:** Pipes, signals, sockets

**Morning: Project 3 (IPC & Signals)**
- Study pipes and signals in FUNDAMENTALS.md
- Build producer-consumer with pipes
- Handle SIGUSR1 for statistics

**Afternoon: Project 4 (Networking)**
- Study socket API
- Build simple TCP echo server
- Test with netcat/telnet

**Commits:**
```
Feat: add producer-consumer pipeline with pipes
Feat: add signal-based statistics reporting
Feat: add TCP echo server handling multiple clients
Docs: add README for projects
```

**What to understand:**
- Pipes: write end → read end (unidirectional)
- Blocking I/O: read on empty pipe blocks
- Signals: asynchronous events
- Socket server pattern: socket → bind → listen → accept
- Concurrent handling: fork per client

---

### Dec 11 (Thursday) - Project 5: Integration
**Focus:** Production daemon combining all concepts

**Schedule:**
- Morning: Study daemonization process
- Midday: Build file server daemon
- Afternoon: Add logging, signal handling, cleanup
- Evening: Test and polish

**Commits:**
```
Feat: add file_server daemon (all concepts integrated)
Feat: add syslog integration and logging
Feat: add graceful shutdown on SIGTERM
Docs: add comprehensive README
```

**What to understand:**
- Daemonization process
- Signal safety and graceful shutdown
- Combining file I/O, processes, IPC, networking
- Production-quality code patterns

---

### Dec 12 (Friday) - Documentation & Push
**Focus:** Polish, document, and push everything

**Schedule:**
- Morning: Review all code comments
- Midday: Create notes for each project (what you learned)
- Afternoon: Final git commits and push
- Evening: Update main README, celebrate!

**Commits:**
```
Docs: finalize FUNDAMENTALS explanations
Docs: add WEEK50 roadmap and timeline
Week50: complete systems programming mastery (5 projects)
```

---

## 📚 Reading Materials

### Before Each Project

**Project 1 (File I/O):**
- Read: FUNDAMENTALS.md - File Descriptors & File I/O section
- Time: 45 minutes
- Key: Understand `open()` flags and error handling

**Project 2 (Process Management):**
- Read: FUNDAMENTALS.md - Process Management section
- Time: 45 minutes
- Key: Understand fork/exec model and zombie processes

**Project 3 (IPC & Signals):**
- Read: FUNDAMENTALS.md - Pipes & Signals section
- Time: 45 minutes
- Key: Understand blocking I/O and signal handling

**Project 4 (Networking):**
- Read: FUNDAMENTALS.md - Sockets & Networking section
- Time: 45 minutes
- Key: Understand server architecture

**Project 5 (Daemon):**
- Read: GIT_PUSH_GUIDE.md and all previous sections
- Time: 45 minutes
- Key: Combine all concepts

---

## 🎓 Concepts You'll Master

### File I/O
- [ ] File descriptors
- [ ] `open()`, `read()`, `write()`, `close()`
- [ ] Buffering and performance
- [ ] `stat()` for metadata
- [ ] Error handling

### Process Management
- [ ] `fork()` - process creation
- [ ] `exec*()` - process replacement
- [ ] `wait()` / `waitpid()` - process reaping
- [ ] Zombie processes
- [ ] Exit codes and signals

### IPC & Signals
- [ ] `pipe()` - inter-process communication
- [ ] `signal()` - signal handling
- [ ] Blocking vs non-blocking I/O
- [ ] Process coordination

### Networking
- [ ] `socket()` - endpoint creation
- [ ] `bind()` - address binding
- [ ] `listen()` - accept connections
- [ ] `accept()` - get client connection
- [ ] `connect()` - client connection

### Integration
- [ ] Daemonization
- [ ] Graceful shutdown
- [ ] Logging and monitoring
- [ ] Production-quality code

---

## 💻 Building Projects

### For Each Project:

```bash
# Enter project directory
cd 01_file_io

# Build
make

# Test (basic)
./mycp test_input.txt test_output.txt

# Test (advanced)
make test

# Debug if needed
make debug
gdb ./mycp

# Clean
make clean
```

### Common Issues & Solutions

**Problem: Compilation errors**
```bash
# Check -Wall -Wextra output
gcc -Wall -Wextra *.c
# Fix warnings (usually simple)
```

**Problem: Segmentation fault**
```bash
# Use gdb
gdb ./program
> run [args]
> backtrace
> print variable_name
```

**Problem: "resource busy"**
```bash
# Port in use (networking)
sudo lsof -i :8080
kill -9 <PID>
```

---

## 🧪 Testing Strategy

### For Each Project:

1. **Happy path:** Normal operation works
   ```bash
   ./mycp small_file.txt output.txt
   ```

2. **Edge cases:** Boundary conditions
   ```bash
   ./mycp empty_file.txt output.txt
   ./mycp 1GB_file.bin output.bin
   ```

3. **Error cases:** What goes wrong
   ```bash
   ./mycp nonexistent.txt output.txt  # ENOENT
   ./mycp /root/secret.txt output.txt # EACCES
   ```

4. **Performance:** Does it scale?
   ```bash
   time ./mycp 100MB_file.bin output.bin
   # Should report speed in MB/s
   ```

---

## 📊 Code Quality Checklist

Before committing each project:

- [ ] Compiles: `gcc -Wall -Wextra -pedantic`
- [ ] No warnings
- [ ] All syscalls checked for errors
- [ ] Memory properly allocated/freed
- [ ] File descriptors properly closed
- [ ] Child processes properly reaped
- [ ] Comments explain complex logic
- [ ] README explains usage
- [ ] Makefile works correctly
- [ ] Tested with multiple cases

---

## 🚀 After Week50: Open-Source

Once all projects pushed:

**Week of Dec 13:**
- Browse "good-first-issue" repos
- Target: `cJSON`, `nanomsg`, `curl`
- Make 1 meaningful contribution

**Skills you now have:**
- ✅ Solid C programming (Week 47-50)
- ✅ Systems programming (Week 50)
- ✅ Git workflow (clean history)
- ✅ Production code patterns
- ✅ Open-source ready

---

## 📈 Progress Tracking

### Track Your Learning:

After each project, answer:
1. What syscalls did I use?
2. What was the hardest part?
3. What would I do differently?
4. How would I extend this?

Save in WEEK50_NOTES.md:

```markdown
## Project 1: mycp
**Syscalls:** open, read, write, close, stat, fchmod

**Hardest part:** Understanding O_CREAT | O_WRONLY | O_TRUNC flags

**Would do differently:** Add write error loop for partial writes

**Extensions:** Add -r for recursive directory copy

## Project 2: simple_shell
**Syscalls:** fork, execvp, wait, signal

...
```

---

## 🎯 Success Metrics

**By end of Week50, you should be able to:**

- ✅ Explain fork() and exec() model
- ✅ Build command-line tools with proper error handling
- ✅ Create server applications handling concurrent clients
- ✅ Understand process lifecycle and signals
- ✅ Write production-quality Unix code
- ✅ Contribute to systems programming open source

---

## 📞 Common Questions

**Q: Should I memorize all syscalls?**
A: No! Understand the patterns. Use `man 2 syscall_name` as reference.

**Q: What if my program crashes?**
A: Use `gdb` to debug:
```bash
gdb ./myprogram
(gdb) run [args]
(gdb) backtrace  # See call stack
```

**Q: Can I look at example code?**
A: Yes! Each project README has complete code with comments.

**Q: How long should each project take?**
A: 6-8 hours (build + test + debug). Spread over 2 days.

**Q: What if I get stuck?**
A: 
1. Read FUNDAMENTALS.md again
2. Check man pages: `man 2 syscall_name`
3. Look at provided code examples
4. Build incrementally (don't try everything at once)

---

## 🎓 Certificate of Completion

After pushing all projects:

```
✅ Week 50: Systems Programming Masterclass
   - 5 Complete Projects
   - 50+ Unix Syscalls
   - Professional Git History
   - Production-Quality Code
   - Ready for Open-Source Contributions

Date: December 12, 2025
```

---

**You've got this! Let's build something amazing! 🚀**
