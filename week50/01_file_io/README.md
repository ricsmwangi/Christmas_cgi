# Project 1: File I/O Mastery - mycp Utility

## 📖 Overview

Build a file copy utility using low-level POSIX syscalls. This project teaches:
- How to open, read, and write files using `open()`, `read()`, `write()`
- File permissions and metadata with `stat()`
- Efficient buffering and large file handling
- Progress tracking (unique angle!)
- Proper error handling

**Learning outcomes:**
- Master file descriptors (FD architecture)
- Understand Unix file I/O paradigm
- Handle edge cases (large files, special files, permissions)
- Optimize buffer sizes for performance

---

## 🎯 Project Goals

**Basic version:**
- Copy file from source to destination
- Preserve file permissions
- Handle errors gracefully

**Extended version:**
- Show progress bar (bytes copied / total)
- Display transfer speed (MB/s)
- Verbose output with timestamps

---

## 📝 Design Plan

```
mycp source.txt destination.txt

Steps:
1. Check if source exists and is readable (stat + open)
2. Get source file size (stat)
3. Open destination for writing (O_CREAT | O_WRONLY | O_TRUNC)
4. Loop: read chunk, write chunk, update progress
5. Preserve source permissions (chmod after closing destination)
6. Close both files and report results
```

---

## 💻 Complete Implementation

### mycp.c - Main Program

```c
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <errno.h>
#include <time.h>

#define BUFFER_SIZE (64 * 1024)  // 64KB buffer for efficiency

// Function to display progress bar
void print_progress(off_t copied, off_t total) {
    if (total == 0) return;
    
    int percent = (int)((copied * 100) / total);
    int filled = percent / 2;  // 50-char bar
    
    printf("\r[");
    for (int i = 0; i < 50; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %d%% (%ld / %ld bytes)", percent, copied, total);
    fflush(stdout);
}

// Function to calculate transfer speed
double calculate_speed(off_t bytes, time_t elapsed_sec) {
    if (elapsed_sec == 0) return 0;
    return (bytes / (1024.0 * 1024.0)) / elapsed_sec;  // MB/s
}

int main(int argc, char *argv[]) {
    // === STEP 1: Validate arguments ===
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *source = argv[1];
    const char *dest = argv[2];
    
    // === STEP 2: Open source file ===
    // Use open() syscall with O_RDONLY flag
    int src_fd = open(source, O_RDONLY);
    if (src_fd == -1) {
        perror("open (source)");
        return EXIT_FAILURE;
    }
    
    // === STEP 3: Get source file stats ===
    // Use stat() to get file size and permissions
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1) {
        perror("fstat");
        close(src_fd);
        return EXIT_FAILURE;
    }
    
    off_t file_size = src_stat.st_size;
    mode_t permissions = src_stat.st_mode;
    
    printf("Copying '%s' to '%s' (%ld bytes)\n", source, dest, file_size);
    
    // === STEP 4: Open destination file ===
    // O_CREAT: create if doesn't exist
    // O_WRONLY: write only
    // O_TRUNC: truncate to zero (overwrite existing)
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("open (destination)");
        close(src_fd);
        return EXIT_FAILURE;
    }
    
    // === STEP 5: Copy file in chunks ===
    // Allocate buffer on heap
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        close(src_fd);
        close(dest_fd);
        return EXIT_FAILURE;
    }
    
    time_t start_time = time(NULL);
    off_t total_copied = 0;
    
    while (1) {
        // Read a chunk from source
        // read() returns number of bytes read, 0 at EOF, -1 on error
        ssize_t bytes_read = read(src_fd, buffer, BUFFER_SIZE);
        
        if (bytes_read == -1) {
            perror("read");
            free(buffer);
            close(src_fd);
            close(dest_fd);
            return EXIT_FAILURE;
        }
        
        // Break if end of file
        if (bytes_read == 0) break;
        
        // Write chunk to destination
        // Note: write() might write fewer bytes than requested!
        ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
        
        if (bytes_written == -1) {
            perror("write");
            free(buffer);
            close(src_fd);
            close(dest_fd);
            return EXIT_FAILURE;
        }
        
        if (bytes_written != bytes_read) {
            fprintf(stderr, "Warning: incomplete write (%ld of %ld bytes)\n",
                    bytes_written, bytes_read);
        }
        
        total_copied += bytes_written;
        print_progress(total_copied, file_size);
    }
    
    time_t end_time = time(NULL);
    time_t elapsed = end_time - start_time;
    
    printf("\n✓ Copy complete!\n");
    printf("  Total: %ld bytes in %ld seconds\n", total_copied, elapsed);
    printf("  Speed: %.2f MB/s\n", calculate_speed(total_copied, elapsed));
    
    // === STEP 6: Preserve permissions ===
    // Use fchmod() to set destination file permissions same as source
    if (fchmod(dest_fd, permissions & 0777) == -1) {
        perror("fchmod");
    }
    
    // === STEP 7: Cleanup ===
    free(buffer);
    close(src_fd);
    close(dest_fd);
    
    return EXIT_SUCCESS;
}
```

---

## 🏗️ Makefile

```makefile
CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
DEBUG_FLAGS = -g -DDEBUG

# Default target
all: mycp

# Build main program
mycp: mycp.c
	$(CC) $(CFLAGS) -o mycp mycp.c

# Debug build (with symbols)
debug: mycp.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -o mycp mycp.c

# Cleanup
clean:
	rm -f mycp *.o test_output.*

# Test with sample files
test: mycp
	@echo "Creating test file (10MB)..."
	dd if=/dev/zero of=test_input.bin bs=1M count=10 2>/dev/null
	@echo "Running mycp..."
	./mycp test_input.bin test_output.bin
	@echo "Verifying (should show nothing if identical):"
	cmp test_input.bin test_output.bin && echo "✓ Files are identical"

# Help
help:
	@echo "Targets:"
	@echo "  make        - Build mycp"
	@echo "  make debug  - Build with debug symbols"
	@echo "  make clean  - Remove binaries"
	@echo "  make test   - Run with 10MB test file"
	@echo "  make help   - Show this help"

.PHONY: all clean debug test help
```

---

## 📚 Detailed Explanations

### Understanding Buffer Size

**Why 64KB?**
- Too small (1KB): Many syscalls, slower
- Too large (1MB): More memory, diminishing returns
- 64KB: Sweet spot between speed and memory

```c
#define BUFFER_SIZE (64 * 1024)  // 64 * 1024 bytes = 65536 bytes
```

### Understanding `open()` Flags

```c
// O_RDONLY = read only (value: 0)
int src_fd = open(source, O_RDONLY);

// O_WRONLY | O_CREAT | O_TRUNC for destination
// Meaning:
// - O_WRONLY: write only mode
// - O_CREAT: create file if it doesn't exist
// - O_TRUNC: if file exists, truncate to zero bytes (overwrite)
int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
```

**Permissions (`0644`):**
- `0` = octal prefix
- `6` = owner (rw-)
- `4` = group (r--)
- `4` = others (r--)

### Understanding Read/Write Loop

```c
while (1) {
    // read() returns:
    // - > 0: number of bytes read
    // - 0: end of file (EOF)
    // - -1: error
    ssize_t bytes_read = read(src_fd, buffer, BUFFER_SIZE);
    
    if (bytes_read == -1) {
        // Error occurred
        perror("read");
        return EXIT_FAILURE;
    }
    
    if (bytes_read == 0) {
        // EOF reached, exit loop
        break;
    }
    
    // Write exactly bytes_read bytes
    // Note: write() might return < bytes_read in rare cases
    ssize_t bytes_written = write(dest_fd, buffer, bytes_read);
    
    if (bytes_written == -1) {
        perror("write");
        return EXIT_FAILURE;
    }
}
```

### Error Handling with `errno`

```c
int fd = open("file.txt", O_RDONLY);
if (fd == -1) {
    // perror() prints:
    // "open: <description of errno>"
    // E.g., "open: No such file or directory"
    perror("open");
    
    // Or manually check errno:
    if (errno == ENOENT) {
        printf("File not found\n");
    } else if (errno == EACCES) {
        printf("Permission denied\n");
    }
    
    return EXIT_FAILURE;
}
```

---

## 🧪 Testing & Validation

### Build & Run

```bash
# Compile
make

# Copy a real file
./mycp /etc/passwd passwd_copy

# Show progress (copy large file)
dd if=/dev/zero of=large_file.bin bs=1M count=100
./mycp large_file.bin large_file_copy.bin

# Verify identical
cmp large_file.bin large_file_copy.bin && echo "✓ Identical"

# Check permissions preserved
ls -l large_file.bin large_file_copy.bin
```

### Edge Cases to Handle

1. **Source file doesn't exist**
   ```bash
   ./mycp nonexistent.txt output.txt
   # Should print: open (source): No such file or directory
   ```

2. **Permission denied**
   ```bash
   ./mycp /root/secret.txt output.txt
   # Should print: open (source): Permission denied
   ```

3. **Destination is read-only directory**
   ```bash
   ./mycp large_file.bin /proc/output.txt
   # Should print: open (destination): Permission denied
   ```

4. **Large files**
   ```bash
   dd if=/dev/zero of=1GB_file.bin bs=1M count=1024
   ./mycp 1GB_file.bin 1GB_copy.bin
   # Should handle smoothly with progress bar
   ```

---

## 🎯 Learning Checkpoints

After this project, you should understand:

- ✅ What file descriptors are and why they matter
- ✅ How `open()` flags work (`O_RDONLY`, `O_WRONLY`, `O_CREAT`, `O_TRUNC`)
- ✅ Why you must handle partial reads/writes
- ✅ How to check return values and use `perror()`
- ✅ Difference between `stat()` and `fstat()`
- ✅ How to preserve file permissions
- ✅ Why buffer size matters for performance

---

## 🚀 Extensions (Optional)

1. **Add recursive directory copy:** Handle directories with `-r` flag
2. **Add progress percentage:** Already in code!
3. **Add verification mode:** Compare files after copy
4. **Add compression:** Compress while copying with zlib
5. **Parallel copying:** Fork multiple processes for large files

---

**Next project: Process Management & Shells**
