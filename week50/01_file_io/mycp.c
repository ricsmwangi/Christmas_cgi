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

/**
 * SYSCALL EXPLANATION: read() and write()
 * 
 * These are the fundamental I/O operations in Unix.
 * read() reads data FROM a file descriptor INTO a buffer
 * write() writes data FROM a buffer TO a file descriptor
 * 
 * Key insight: Everything is a file - regular files, pipes, sockets, devices
 */

/* Print a progress bar showing copy progress */
void print_progress(off_t copied, off_t total) {
    if (total == 0) return;
    
    int percent = (int)((copied * 100) / total);
    int filled = percent / 2;  // 50-character bar
    
    printf("\r[");
    for (int i = 0; i < 50; i++) {
        if (i < filled) printf("=");
        else printf(" ");
    }
    printf("] %d%% (%ld / %ld bytes)", percent, copied, total);
    fflush(stdout);
}

/* Calculate transfer speed in MB/s */
double calculate_speed(off_t bytes, time_t elapsed_sec) {
    if (elapsed_sec == 0) return 0;
    return (bytes / (1024.0 * 1024.0)) / elapsed_sec;  // MB/s
}

int main(int argc, char *argv[]) {
    /* === STEP 1: Validate command-line arguments === */
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <source> <destination>\n", argv[0]);
        fprintf(stderr, "Example: %s input.txt output.txt\n", argv[0]);
        return EXIT_FAILURE;
    }
    
    const char *source = argv[1];
    const char *dest = argv[2];
    
    /* === STEP 2: Open source file with read-only flag === */
    /*
     * SYSCALL: open(filename, flags, mode)
     * 
     * For source:
     * - O_RDONLY (0): Open for reading only
     * 
     * Returns: file descriptor (0-1023) on success, -1 on error
     * 
     * Common errors:
     * - ENOENT: File doesn't exist
     * - EACCES: Permission denied
     * - EISDIR: It's a directory, not a file
     */
    int src_fd = open(source, O_RDONLY);
    if (src_fd == -1) {
        perror("open (source)");  // Prints error message like "No such file"
        return EXIT_FAILURE;
    }
    
    /* === STEP 3: Get source file metadata using fstat() === */
    /*
     * SYSCALL: fstat(fd, &stat_buf)
     * 
     * Retrieves file metadata (size, permissions, timestamps, etc.)
     * Unlike stat() which takes a filename, fstat() uses file descriptor
     * 
     * struct stat contains:
     * - st_size: File size in bytes
     * - st_mode: File permissions and type
     * - st_mtime: Last modification time
     * - And many more fields...
     */
    struct stat src_stat;
    if (fstat(src_fd, &src_stat) == -1) {
        perror("fstat");
        close(src_fd);
        return EXIT_FAILURE;
    }
    
    off_t file_size = src_stat.st_size;
    mode_t permissions = src_stat.st_mode;
    
    printf("Copying '%s' to '%s' (%ld bytes)\n", source, dest, file_size);
    
    /* === STEP 4: Open destination file for writing === */
    /*
     * SYSCALL: open(filename, flags, mode)
     * 
     * For destination:
     * - O_WRONLY: Open for writing only
     * - O_CREAT: Create the file if it doesn't exist
     * - O_TRUNC: Truncate to zero bytes if it exists (overwrite)
     * 
     * Third argument (0644) is permissions for newly created file:
     * - 0: Octal prefix
     * - 6 (rw-): Owner can read/write
     * - 4 (r--): Group can read
     * - 4 (r--): Others can read
     * 
     * Binary: 110 100 100
     *         rw- r-- r--
     */
    int dest_fd = open(dest, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dest_fd == -1) {
        perror("open (destination)");
        close(src_fd);
        return EXIT_FAILURE;
    }
    
    /* === STEP 5: Allocate buffer for file chunks === */
    /*
     * WHY BUFFER?
     * 
     * Without buffer:
     * - Copy 1 byte at a time → 1000+ syscalls for 1KB file → SLOW
     * 
     * With 64KB buffer:
     * - Copy 64KB at a time → ~16 syscalls for 1MB file → FAST
     * 
     * Buffer size tradeoff:
     * - Too small (1KB): Many syscalls, slow
     * - Too large (1MB): More memory, diminishing speed gains
     * - 64KB: Sweet spot for most systems
     */
    char *buffer = malloc(BUFFER_SIZE);
    if (buffer == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        close(src_fd);
        close(dest_fd);
        return EXIT_FAILURE;
    }
    
    /* === STEP 6: Copy file in chunks === */
    time_t start_time = time(NULL);
    off_t total_copied = 0;
    
    while (1) {
        /*
         * SYSCALL: read(fd, buffer, count)
         * 
         * Reads UP TO count bytes from file descriptor fd into buffer.
         * Important: May read fewer bytes than requested!
         * 
         * Returns:
         * - > 0: Number of bytes actually read
         * - 0: End of file (EOF) reached
         * - -1: Error occurred (check errno)
         * 
         * Why not always full buffer?
         * - Network files: Partial packets
         * - Pipes: Limited data available
         * - Signals: Interrupt before completion
         */
        ssize_t bytes_read = read(src_fd, buffer, BUFFER_SIZE);
        
        if (bytes_read == -1) {
            perror("read");
            free(buffer);
            close(src_fd);
            close(dest_fd);
            return EXIT_FAILURE;
        }
        
        /* Break loop when we reach end of file */
        if (bytes_read == 0) {
            break;
        }
        
        /*
         * SYSCALL: write(fd, buffer, count)
         * 
         * Writes UP TO count bytes from buffer to file descriptor fd.
         * 
         * Returns:
         * - > 0: Number of bytes actually written
         * - -1: Error occurred
         * 
         * IMPORTANT: write() may write fewer bytes than requested!
         * This is especially true for:
         * - Pipes (buffer full)
         * - Network sockets (slow connection)
         * - Disks (running out of space)
         * 
         * Solution: Loop until all bytes written (not implemented here
         * for simplicity, but production code should do this)
         */
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
    
    /* === STEP 7: Preserve source file permissions === */
    /*
     * SYSCALL: fchmod(fd, mode)
     * 
     * Changes permissions of file referenced by file descriptor.
     * 
     * Why necessary?
     * - open() with mode 0644 creates world-readable file
     * - But source might have mode 0600 (owner-only readable)
     * - We copy the stat, but file was already created, so we chmod
     * 
     * & 0777:
     * - Mask to keep only permission bits (remove file type bits)
     * - st_mode contains both type (regular file, directory, etc.) and perms
     * - 0777 in binary: 111 111 111 (all permission bits)
     */
    if (fchmod(dest_fd, permissions & 0777) == -1) {
        perror("fchmod");
    }
    
    /* === STEP 8: Cleanup - Close file descriptors === */
    /*
     * SYSCALL: close(fd)
     * 
     * Closes file descriptor and frees it for reuse.
     * 
     * Important:
     * - Always close files when done (resource leak prevention)
     * - Kernel automatically closes on process exit (but don't rely on it)
     * - Closing doesn't delete file, just closes the FD
     */
    free(buffer);
    close(src_fd);
    close(dest_fd);
    
    return EXIT_SUCCESS;
}

/*
 * ============================================================================
 * SUMMARY OF SYSCALLS USED
 * ============================================================================
 * 
 * 1. open() - Open/create file, returns file descriptor
 * 2. fstat() - Get file metadata (size, permissions, etc.)
 * 3. read() - Read bytes from file descriptor into buffer
 * 4. write() - Write bytes from buffer to file descriptor
 * 5. fchmod() - Change file permissions via file descriptor
 * 6. close() - Close file descriptor
 * 
 * ============================================================================
 * KEY CONCEPTS
 * ============================================================================
 * 
 * FILE DESCRIPTORS (FD):
 * - Small integers (0, 1, 2, 3, ...) that reference open files
 * - 0 = stdin, 1 = stdout, 2 = stderr (always reserved)
 * - Your program gets 3+ for files it opens
 * - Unique per process (different processes can have same FD for different files)
 * 
 * BUFFERING:
 * - Small reads/writes = many syscalls = slow
 * - Large buffer = few syscalls = fast
 * - Optimal size depends on hardware (disk block size, network MTU, etc.)
 * - 64KB is good default for modern systems
 * 
 * ERROR HANDLING:
 * - Always check return values
 * - -1 usually means error
 * - Use perror() to print error message
 * - Use errno variable for detailed error code
 * 
 * EVERYTHING IS A FILE:
 * - Regular files
 * - Directories
 * - Pipes
 * - Sockets
 * - Devices (/dev/null, /dev/zero, /dev/random)
 * - All accessed through same read()/write() interface
 */
