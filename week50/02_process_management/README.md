# Project 2: Process Management - Simple Shell

## 📖 Overview

Build a simple command-line shell using `fork()` and `execve()`. This teaches:
- Process creation with `fork()`
- Process replacement with `execve()` family
- Parent-child synchronization with `wait()`
- Signal handling basics
- Building interactive applications

**Learning outcomes:**
- Understand process lifecycle and forking model
- Know when to use `fork()` vs `exec()` vs both
- Handle zombie processes properly
- Build background job support

---

## 🎯 Project Goals

**Basic shell:**
- Read user commands from stdin
- Execute commands using `fork()` + `execvp()`
- Wait for command completion
- Handle exit/quit commands

**Extended shell:**
- Support background execution (`&` suffix)
- Show job list
- Handle Ctrl+C gracefully
- Change working directory (`cd`)

---

## 💻 Complete Implementation

### simple_shell.c - Full Commented Code

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <pwd.h>

#define COMMAND_LEN 1024
#define ARGS_LEN 128

/* Global to track if we should exit */
int shell_running = 1;

/* Handle Ctrl+C signal - don't exit shell, just print newline */
void handle_sigint(int sig) {
    printf("\n");
    fflush(stdout);
    /* Return to shell prompt */
}

/**
 * Parse command line into arguments array.
 * Returns number of arguments, or -1 if background job (ends with &).
 * 
 * Example: "ls -la /home" → args = ["ls", "-la", "/home", NULL]
 */
int parse_command(char *line, char *args[], int *background) {
    int argc = 0;
    *background = 0;
    
    /* Tokenize by spaces */
    char *token = strtok(line, " \t\n");
    
    while (token != NULL && argc < ARGS_LEN - 1) {
        /* Check for background indicator */
        if (strcmp(token, "&") == 0) {
            *background = 1;
            break;
        }
        
        args[argc++] = token;
        token = strtok(NULL, " \t\n");
    }
    
    args[argc] = NULL;  /* NULL-terminate argument array for execvp */
    return argc;
}

/**
 * Get current working directory and format prompt.
 * Returns static buffer with prompt string.
 */
const char *get_prompt(void) {
    static char prompt[256];
    char *home = getenv("HOME");
    char cwd[256];
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        snprintf(prompt, sizeof(prompt), "> ");
    } else {
        /* Show ~/ instead of full home path for cleaner display */
        if (home && strncmp(cwd, home, strlen(home)) == 0) {
            snprintf(prompt, sizeof(prompt), "~%s> ", cwd + strlen(home));
        } else {
            snprintf(prompt, sizeof(prompt), "%s> ", cwd);
        }
    }
    
    return prompt;
}

/**
 * Execute a single command.
 * Returns 0 on success, -1 on error.
 */
int execute_command(char *args[], int background) {
    if (args[0] == NULL) {
        return 0;  /* Empty command */
    }
    
    /* Handle built-in commands */
    if (strcmp(args[0], "cd") == 0) {
        const char *target = args[1] ? args[1] : getenv("HOME");
        if (chdir(target) == -1) {
            perror("cd");
        }
        return 0;
    }
    
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
        shell_running = 0;
        return 0;
    }
    
    if (strcmp(args[0], "pwd") == 0) {
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
        }
        return 0;
    }
    
    /* === FORK CHILD PROCESS TO RUN EXTERNAL COMMAND === */
    /*
     * WHY fork()?
     * 
     * We need to:
     * 1. Keep shell running to accept next command
     * 2. Run user's command (replace current process image)
     * 
     * Solution: fork() to create child, child runs command, parent waits
     * 
     * Process tree:
     * 
     * shell (parent)
       /  |  \
      /   |   \
    child child child    (each child runs a command)
    (ls) (cat) (find)
     */
    
    pid_t pid = fork();
    
    if (pid == -1) {
        /* fork() failed */
        perror("fork");
        return -1;
    }
    
    if (pid == 0) {
        /* === CHILD PROCESS === */
        /*
         * We're now in the child process.
         * Child is an exact copy of parent at fork() point.
         * 
         * Next: REPLACE child process image with user's command
         * using execvp() - changes process image but keeps PID, FDs, etc.
         */
        
        /*
         * SYSCALL: execvp(program, args)
         * 
         * Like execve() but searches PATH environment variable
         * for the program.
         * 
         * Returns: never on success (process replaced)
         *          -1 on error (wrong in arguments)
         */
        execvp(args[0], args);
        
        /*
         * If we reach here, execvp failed.
         * Child process prints error and exits.
         * Parent doesn't know about this error (that's why we check below).
         */
        perror("execvp");
        exit(EXIT_FAILURE);
        
    } else {
        /* === PARENT PROCESS === */
        /*
         * We're back in parent (original shell process).
         * Child is running in parallel.
         */
        
        if (background) {
            /* Background job: don't wait, print PID, return to prompt */
            printf("[%d] %s\n", pid, args[0]);
            return 0;
        }
        
        /*
         * SYSCALL: waitpid(pid, &status, options)
         * 
         * Parent waits for child to finish and collects exit status.
         * 
         * Why wait?
         * - If parent doesn't wait, child becomes ZOMBIE
         * - Zombie: Process exited but still in kernel's process table
         * - Parent must reap zombie by calling wait() or waitpid()
         * - Prevents process table from filling up
         * 
         * Parameters:
         * - pid: Wait for this specific child
         * - status: Filled with child's exit info
         * - options: 0 for blocking wait
         */
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
        
        /* Check how child exited */
        if (WIFEXITED(status)) {
            /*
             * Child exited normally.
             * WEXITSTATUS() extracts the exit code.
             */
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                printf("[%d] Exit %d\n", pid, exit_code);
            }
        } else if (WIFSIGNALED(status)) {
            /*
             * Child was killed by signal.
             * WTERMSIG() extracts the signal number.
             */
            int sig = WTERMSIG(status);
            printf("[%d] Killed by signal %d\n", pid, sig);
        }
    }
    
    return 0;
}

int main(void) {
    char line[COMMAND_LEN];
    char *args[ARGS_LEN];
    int background;
    
    printf("Simple Shell\n");
    printf("Type 'exit' or 'quit' to leave\n\n");
    
    /* Setup signal handler for Ctrl+C */
    signal(SIGINT, handle_sigint);
    
    while (shell_running) {
        /* Print prompt */
        printf("%s", get_prompt());
        fflush(stdout);
        
        /* Read command from user */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* EOF (Ctrl+D) */
            printf("\nexit\n");
            break;
        }
        
        /* Parse command into arguments */
        int argc = parse_command(line, args, &background);
        
        if (argc == 0) {
            /* Empty line, just show prompt again */
            continue;
        }
        
        /* Execute command */
        execute_command(args, background);
    }
    
    printf("Goodbye!\n");
    return EXIT_SUCCESS;
}
```

---

## 🏗️ Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -g
PROGRAM = simple_shell

all: $(PROGRAM)

$(PROGRAM): simple_shell.c
	$(CC) $(CFLAGS) -o $(PROGRAM) simple_shell.c

clean:
	rm -f $(PROGRAM) *.o

run: $(PROGRAM)
	./$(PROGRAM)

debug: $(PROGRAM)
	gdb ./$(PROGRAM)

help:
	@echo "Targets:"
	@echo "  make      - Compile shell"
	@echo "  make run  - Run shell"
	@echo "  make clean- Remove binary"
	@echo "  make debug- Run under gdb debugger"

.PHONY: all clean run debug help
```

---

## 📚 Key Concepts Explained

### Understanding fork()

```c
pid_t pid = fork();
```

**What happens:**
1. Current process is duplicated (memory, files, state)
2. Two identical processes now running same code
3. `fork()` returns twice:
   - Parent gets child's PID (>0)
   - Child gets 0
   - On error, returns -1

**Example execution trace:**

```
Before fork:
    shell process running main()

fork() called

After fork:
    Parent shell                Child shell
    (continues after fork)      (continues after fork)
    pid = 1234                  pid = 0
    (1234 > 0, so parent)       (0, so child)

    Parent continues:           Child continues:
    "I'm parent, wait"          "I'm child, exec"
    waitpid(1234)               execvp("ls", [...])
    (blocks until child done)   (process image replaced)
```

### Understanding execvp()

```c
execvp(args[0], args);  // "ls -la /home"
```

**What happens:**
1. Load program from disk (`/bin/ls`)
2. Replace current process image:
   - Code segment → new program
   - Data segment → new program's data
   - BUT: PID stays same, FDs stay open
3. Start executing new program from `main()`
4. Never returns (unless error)

**Why this matters:**

```c
// WITHOUT fork+exec:
my_ls();           // My custom ls implementation
my_cat();          // My custom cat implementation
my_find();         // My custom find implementation
// Problem: Need to reimplement every Unix utility!

// WITH fork+exec:
fork();            // Create child
execvp("ls", ...); // Child runs actual /bin/ls
                   // Don't need to reimplement!
```

### Understanding wait()

```c
int status;
waitpid(pid, &status, 0);  // Block until child exits

if (WIFEXITED(status)) {
    int code = WEXITSTATUS(status);
    printf("Child exited with code %d\n", code);
}
```

**Why necessary:**

```
If parent doesn't wait:

Parent                    Child
(shell continues)         process running
(forks new child)         (finishes)
(forks another)           (becomes ZOMBIE)
...                       (waits for parent to reap)

Eventually:
ps aux shows:
  [1234] Z  child_program  <defunct>

This is a zombie process - wastes kernel resources.

With waitpid():

Parent                    Child
(shell continues)         process running
(calls waitpid)           (finishes)
(blocks until child done) (sends SIGCHLD)
(receives child status)   (becomes ZOMBIE briefly)
(child fully cleaned up)  (kernel removes entry)
```

---

## 🧪 Usage Examples

### Building and Running

```bash
make
./simple_shell
```

### Shell Commands to Try

```bash
> pwd
/home/user/week50/02_process_management

> ls -l
total 24
-rw-r--r-- 1 user user  1024 Dec  4 10:30 Makefile
-rw-r--r-- 1 user user 15000 Dec  4 10:30 simple_shell.c
...

> cd /tmp
/tmp> pwd
/tmp

> exit
Goodbye!
```

### Background Jobs

```bash
> sleep 10 &
[1234] sleep

> ps aux | grep sleep
user 1234  0.0  0.0  4224  656 pts/0 S+ 10:35 sleep 10

> # Shell is free to accept more commands
```

### Built-in Commands

```bash
> cd /home           # Change directory
> pwd                # Print working directory
> exit               # Exit shell
```

---

## 🎯 Learning Checkpoints

After this project, you should understand:

- ✅ What `fork()` does and how it returns twice
- ✅ Difference between fork and exec
- ✅ Why parent must call `wait()`/`waitpid()`
- ✅ What zombie processes are and why they're bad
- ✅ How to handle command-line parsing
- ✅ How to build interactive applications
- ✅ Signal handling basics

---

## 🚀 Extensions (Optional)

1. **Job control:** Implement `jobs`, `fg`, `bg` commands
2. **Pipes:** Support command piping (`ls | grep` foo)
3. **Redirects:** Support I/O redirection (`> output.txt`, `< input.txt`)
4. **History:** Keep command history with arrow keys
5. **Aliases:** Support command aliases
6. **Variables:** Support environment variable expansion

---

**Next project: IPC & Signals**
