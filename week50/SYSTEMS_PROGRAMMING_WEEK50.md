# Week 50: Systems Programming Mastery

## 📚 Overview

Deep dive into Unix/Linux systems programming. Master low-level OS interactions, process management, inter-process communication, and networking—the foundation of production systems.

**Week50 Learning Goals:**
- ✅ Understand file descriptors and POSIX file I/O
- ✅ Master process creation and management (`fork`, `exec`)
- ✅ Implement inter-process communication (pipes, signals)
- ✅ Build networked applications (sockets)
- ✅ Combine concepts into production daemons
- ✅ Prepare for open-source contributions

---

## 📋 Project Structure

```
week50/
├── 01_file_io/           → File descriptor mastery (mycp utility)
├── 02_process_management/→ Process creation and shells
├── 03_ipc_signals/       → Pipes, signals, synchronization
├── 04_networking/        → TCP/UDP, socket programming
├── 05_daemon/            → Production daemon with all concepts
├── FUNDAMENTALS.md       → Core concepts and syscalls
└── GIT_PUSH_GUIDE.md     → Best practices for portfolio projects
```

---

## 🎯 Learning Path

### **Day 1-2: File I/O Foundations**
- Read: `FUNDAMENTALS.md` - File Descriptors section
- Project: `01_file_io/mycp` - file copy with progress
- Syscalls: `open()`, `read()`, `write()`, `close()`, `stat()`

### **Day 3-4: Process Control**
- Read: `FUNDAMENTALS.md` - Process Management section
- Project: `02_process_management/simple_shell` - fork-based shell
- Syscalls: `fork()`, `execve()`, `wait()`, `exit()`

### **Day 5-6: IPC & Signals**
- Read: `FUNDAMENTALS.md` - IPC & Signals section
- Project: `03_ipc_signals/producer_consumer` - pipe-based sync
- Syscalls: `pipe()`, `signal()`, `sigaction()`, `kill()`

### **Day 7-8: Networking**
- Read: `FUNDAMENTALS.md` - Networking section
- Project: `04_networking/echo_server` - TCP server
- Syscalls: `socket()`, `bind()`, `listen()`, `accept()`, `connect()`

### **Day 9-10: Integration**
- Project: `05_daemon/file_server` - daemon + all concepts
- Polish and documentation
- Push to GitHub

---

## 🚀 Best Practices Summary

**For every project:**
1. Compile with warnings: `gcc -Wall -Wextra -pedantic`
2. Check return values (all syscalls can fail)
3. Handle `errno` properly
4. Test edge cases (large files, signals, connection drops)
5. Document every syscall with comments
6. Include Makefile with `clean` and `debug` targets

**Git workflow:**
```bash
git add week50/project_name/
git commit -m "Feat: add project_name - syscalls: fork, pipe, signal"
git push origin main
```

---

## 📖 Key Resources

- **man pages:** `man 2 syscall_name` (e.g., `man 2 fork`)
- **POSIX specification:** What standard guarantees
- **Error handling:** Always check return values, use `perror()`
- **Testing:** Use `strace` to trace syscalls

---

## 🎓 What You'll Learn

By end of Week50:
- How operating systems manage resources
- Why processes are isolated and how to communicate
- How servers handle multiple clients
- How to write robust, production-ready C code
- How to contribute to open-source systems projects

**Next: Open-source contributions targeting systems programming repos**

---

## 📝 Notes

- All projects are self-contained and runnable
- Makefiles provided for easy compilation
- Study materials explain every concept and syscall
- Progress through projects sequentially (each builds on previous)
- Save your work and push regularly

**Good luck! 🚀**
