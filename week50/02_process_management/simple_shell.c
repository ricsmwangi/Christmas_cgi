#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

#define COMMAND_LEN 1024
#define ARGS_LEN 128

/* Global flag to control shell loop */
int shell_running = 1;

/**
 * Signal handler for Ctrl+C (SIGINT)
 * 
 * Default behavior: kills the shell
 * Our behavior: just print newline and return to prompt
 * 
 * This way:
 * - Ctrl+C kills the running command (not shell)
 * - Shell continues accepting new commands
 */
void handle_sigint(int sig) {
    (void)sig;  /* Unused parameter */
    printf("\n");
    fflush(stdout);
}

/**
 * Parse a command line into arguments array.
 * 
 * Example input:  "ls -l /home &"
 * Output args:    ["ls", "-l", "/home", NULL]
 * Output background: 1 (because ends with &)
 * 
 * Returns: Number of arguments parsed
 */
int parse_command(char *line, char *args[], int *background) {
    int argc = 0;
    *background = 0;
    
    /* Remove trailing newline */
    size_t len = strlen(line);
    if (len > 0 && line[len - 1] == '\n') {
        line[len - 1] = '\0';
    }
    
    /* Tokenize using strtok() */
    char *token = strtok(line, " \t");
    
    while (token != NULL && argc < ARGS_LEN - 1) {
        /* Check for background job indicator */
        if (strcmp(token, "&") == 0) {
            *background = 1;
            break;
        }
        
        args[argc++] = token;
        token = strtok(NULL, " \t");
    }
    
    /* 
     * IMPORTANT: execvp expects NULL-terminated argument array!
     * So we must add NULL at the end.
     */
    args[argc] = NULL;
    
    return argc;
}

/**
 * Get current working directory and format as shell prompt.
 * Shows ~ for home directory for cleaner display.
 */
const char *get_prompt(void) {
    static char prompt[256];
    char cwd[256];
    const char *home = getenv("HOME");
    
    if (getcwd(cwd, sizeof(cwd)) == NULL) {
        snprintf(prompt, sizeof(prompt), "shell> ");
    } else {
        /* Replace home directory with ~ for cleaner display */
        if (home && strncmp(cwd, home, strlen(home)) == 0) {
            snprintf(prompt, sizeof(prompt), "~%s$ ", cwd + strlen(home));
        } else {
            snprintf(prompt, sizeof(prompt), "%s$ ", cwd);
        }
    }
    
    return prompt;
}

/**
 * Execute a command.
 * Returns 0 on success, -1 on error.
 * 
 * Handles:
 * - Built-in commands (cd, pwd, exit)
 * - External commands (ls, cat, etc.) via fork + exec
 */
int execute_command(char *args[], int background) {
    if (args[0] == NULL) {
        return 0;  /* Empty command, no-op */
    }
    
    /* === BUILT-IN COMMANDS === */
    /* These run in the shell process itself, no fork needed */
    
    if (strcmp(args[0], "cd") == 0) {
        /* Change directory */
        const char *target = args[1] != NULL ? args[1] : getenv("HOME");
        if (chdir(target) == -1) {
            perror("cd");
        }
        return 0;
    }
    
    if (strcmp(args[0], "pwd") == 0) {
        /* Print working directory */
        char cwd[256];
        if (getcwd(cwd, sizeof(cwd)) != NULL) {
            printf("%s\n", cwd);
        } else {
            perror("getcwd");
        }
        return 0;
    }
    
    if (strcmp(args[0], "exit") == 0 || strcmp(args[0], "quit") == 0) {
        shell_running = 0;
        return 0;
    }
    
    if (strcmp(args[0], "help") == 0) {
        printf("Built-in commands:\n");
        printf("  cd <dir>     - Change directory\n");
        printf("  pwd          - Print working directory\n");
        printf("  exit, quit   - Exit shell\n");
        printf("  help         - Show this help\n");
        printf("\nOther commands are executed from /bin, /usr/bin, etc.\n");
        printf("Use & at end to run in background: ls -l &\n");
        return 0;
    }
    
    /* === EXTERNAL COMMANDS === */
    /* Run in child process created by fork */
    
    /*
     * SYSCALL: fork()
     * 
     * Creates a new process by duplicating the current process.
     * 
     * Returns:
     * - Parent: child's PID (a positive number)
     * - Child: 0
     * - Error: -1
     * 
     * Key point: fork() returns to BOTH parent and child!
     * So code after fork() executes in both processes.
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
         * Child is an exact copy of parent:
         * - Same code
         * - Same memory
         * - Same open file descriptors
         * - BUT: Different PID
         * - BUT: Separate memory (changes don't affect parent)
         */
        
        /*
         * SYSCALL: execvp(program, argv)
         * 
         * Replaces current process image with new program.
         * 
         * Parameters:
         * - program: Name of executable to run
         * - argv: Argument array (must end with NULL)
         * 
         * Behavior:
         * - Searches PATH for the program (unlike execve which needs full path)
         * - Replaces current process code/data with new program
         * - PID remains same
         * - Open file descriptors remain open
         * - Never returns on success (process replaced)
         * - Returns -1 on error
         * 
         * Example:
         *   args = ["ls", "-l", "/tmp", NULL]
         *   execvp("ls", args)
         *   → Runs /bin/ls -l /tmp
         *   → Child process replaced with ls
         *   → Parent still running shell
         */
        execvp(args[0], args);
        
        /* 
         * If we reach here, execvp() failed.
         * This happens if:
         * - Program not found in PATH
         * - Permission denied
         * - Executable is corrupted
         */
        perror("execvp");
        exit(EXIT_FAILURE);
        
    } else {
        /* === PARENT PROCESS === */
        /*
         * We're back in the original shell process.
         * Child process is running in parallel (or queued by scheduler).
         */
        
        if (background) {
            /*
             * Background job: don't wait for child
             * Print job info and return to prompt immediately
             */
            printf("[%d] %s\n", pid, args[0]);
            return 0;
        }
        
        /*
         * SYSCALL: waitpid(pid, &status, options)
         * 
         * Parent waits for child process to finish.
         * 
         * Parameters:
         * - pid: Wait for this specific child
         * - status: Pointer to int, filled with child's exit info
         * - options: 0 for blocking wait
         * 
         * Blocking wait means:
         * - Parent is suspended (put to sleep)
         * - Parent wakes when child exits
         * - Returns immediately with child's PID
         * 
         * Why wait?
         * 
         * If parent doesn't wait:
         * - Child exits
         * - Child becomes ZOMBIE (dead but not reaped)
         * - Kernel keeps process table entry
         * - Eventually process table fills up → system breaks
         * 
         * With waitpid:
         * - Child exits
         * - Parent is notified
         * - Parent reaps child (removes from process table)
         * - Kernel frees resources
         */
        int status;
        if (waitpid(pid, &status, 0) == -1) {
            perror("waitpid");
            return -1;
        }
        
        /* 
         * Check how child exited.
         * status contains encoded exit information.
         * Use macros to extract the information.
         */
        
        if (WIFEXITED(status)) {
            /*
             * WIFEXITED(status): true if child exited normally
             * 
             * Normal exit means:
             * - Child called exit(code)
             * - Child returned from main()
             * 
             * WEXITSTATUS(status): Extract the exit code
             * - 0 usually means success
             * - Non-zero means error
             */
            int exit_code = WEXITSTATUS(status);
            if (exit_code != 0) {
                printf("[%d] Exit code %d\n", pid, exit_code);
            }
            
        } else if (WIFSIGNALED(status)) {
            /*
             * WIFSIGNALED(status): true if child killed by signal
             * 
             * Example: User pressed Ctrl+C while ls running
             * - Shell sends SIGINT to child
             * - Child dies
             * - Status reflects this
             * 
             * WTERMSIG(status): Extract the signal number
             * - SIGTERM (15): Graceful termination
             * - SIGKILL (9): Forced kill
             * - SIGINT (2): User pressed Ctrl+C
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
    
    printf("=== Simple Shell ===\n");
    printf("Type 'help' for built-in commands\n");
    printf("Type 'exit' to quit\n\n");
    
    /* 
     * SYSCALL: signal(signum, handler)
     * 
     * Registers a function to handle a signal.
     * 
     * Signals are asynchronous events (like interrupts).
     * 
     * Common signals:
     * - SIGINT (2): Ctrl+C
     * - SIGTERM (15): Graceful termination
     * - SIGKILL (9): Forced kill (can't be caught)
     * - SIGCHLD (17): Child process exited
     * 
     * Here we register handle_sigint() for SIGINT.
     * Now when user presses Ctrl+C:
     * 1. Kernel sends SIGINT to shell process
     * 2. Shell stops whatever it's doing
     * 3. Kernel calls handle_sigint()
     * 4. Shell prints newline and returns
     * 5. Shell continues from where it was interrupted
     */
    signal(SIGINT, handle_sigint);
    
    while (shell_running) {
        /* Display prompt */
        printf("%s", get_prompt());
        fflush(stdout);
        
        /* Read command from user */
        if (fgets(line, sizeof(line), stdin) == NULL) {
            /* 
             * EOF reached (user pressed Ctrl+D).
             * This doesn't send a signal, fgets just returns NULL.
             */
            printf("\nexit\n");
            break;
        }
        
        /* Parse command into arguments */
        int argc = parse_command(line, args, &background);
        
        if (argc == 0) {
            /* Empty line (user just pressed Enter) */
            continue;
        }
        
        /* Execute the command */
        execute_command(args, background);
    }
    
    printf("Shell exiting. Goodbye!\n");
    return EXIT_SUCCESS;
}

/*
 * ============================================================================
 * PROCESS MODEL EXPLANATION
 * ============================================================================
 * 
 * Unix uses a fork-exec model:
 * 
 * FORK: Create child process
 * ├─ Child is exact copy of parent
 * ├─ Both processes continue from fork() point
 * ├─ fork() returns 0 in child, child's PID in parent
 * └─ Both can run in parallel (scheduler time-slices)
 * 
 * EXEC: Replace process image
 * ├─ Load new program from disk
 * ├─ Replace code, data, stack
 * ├─ PID doesn't change
 * ├─ Open file descriptors don't change (inherited)
 * └─ Environment variables inherited
 * 
 * WAIT: Parent waits for child
 * ├─ Parent blocked (sleeping)
 * ├─ Child runs independently
 * ├─ When child exits, parent wakes
 * ├─ Parent reaps child's exit status
 * └─ Prevents zombie processes
 * 
 * Example: User types "ls -l"
 * 
 * 1. Shell in getline/fgets(), waiting for input
 * 2. User types "ls -l\n"
 * 3. Shell reads the command
 * 4. Shell calls fork()
 *    → Two processes now exist (parent=shell, child=shell_copy)
 * 5. Child calls execvp("ls", ["ls", "-l", NULL])
 *    → Child's process image replaced with /bin/ls
 *    → Child now runs /bin/ls code
 * 6. Parent calls waitpid(child_pid, &status, 0)
 *    → Parent sleeps waiting for child
 * 7. /bin/ls runs, produces output, exits
 * 8. Kernel wakes parent (child has exited)
 * 9. Parent continues from waitpid()
 * 10. Parent returns to prompt
 * 
 * WHY FORK+EXEC?
 * 
 * Alternative design (wrong):
 * ├─ Shell has /bin/ls code built-in
 * ├─ Shell has /bin/cat code built-in
 * ├─ Shell has /bin/find code built-in
 * └─ Would require reimplementing entire Unix!
 * 
 * Unix design (correct):
 * ├─ Shell only knows how to fork+exec
 * ├─ Each utility is separate executable
 * ├─ Shell just launches executables
 * └─ Very flexible - can run any program ever written!
 * 
 * ============================================================================
 * SIGNALS AND ZOMBIE PROCESSES
 * ============================================================================
 * 
 * ZOMBIE PROCESS: Child process that has exited but parent hasn't reaped.
 * 
 * Without wait():
 * 
 *   shell (parent)
 *   ├─ fork() → child created
 *   ├─ fork() returns, parent continues
 *   ├─ Parent doesn't wait
 *   ├─ Parent forks another child
 *   ├─ Parent forks another child
 *   └─ Meanwhile, first child has exited
 *       └─ Kernel keeps it as ZOMBIE (waiting for parent to reap)
 * 
 * ps aux output:
 *   user  1234  0.0  0.0     0     0 pts/0 Z+ 10:00 <defunct>
 *   ^                                         ^
 *   PID                                        Z = zombie state
 * 
 * With wait():
 * 
 *   shell (parent)
 *   ├─ fork() → child created
 *   ├─ wait() → parent sleeps
 *   ├─ Child exits
 *   ├─ Kernel wakes parent
 *   ├─ Parent reaps child
 *   └─ Child removed from process table
 * 
 * SIGNALS:
 * 
 * Signals are software interrupts:
 * ├─ SIGINT (Ctrl+C): Interrupt signal
 * ├─ SIGTERM (kill command): Termination signal
 * ├─ SIGKILL (kill -9): Forced kill (can't catch)
 * └─ SIGCHLD: Child process exited
 * 
 * Without signal handler:
 * ├─ User presses Ctrl+C
 * ├─ Kernel sends SIGINT to shell
 * ├─ Default handler: terminate shell
 * └─ Shell process dies
 * 
 * With signal handler:
 * ├─ User presses Ctrl+C
 * ├─ Kernel sends SIGINT to shell
 * ├─ Kernel calls our handle_sigint()
 * ├─ We print newline and return
 * └─ Shell continues running
 * 
 * ============================================================================
 */
