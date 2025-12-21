CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O2
DEBUG_FLAGS = -g -DDEBUG

# Default target
all: mycp

# Build main program
mycp: mycp.c
	$(CC) $(CFLAGS) -o mycp mycp.c

# Debug build (with symbols for gdb)
debug: mycp.c
	$(CC) $(DEBUG_FLAGS) $(CFLAGS) -o mycp mycp.c

# Cleanup
clean:
	rm -f mycp *.o test_output.* test_input.*

# Test with 10MB file
test: mycp
	@echo "Creating 10MB test file..."
	dd if=/dev/zero of=test_input.bin bs=1M count=10 2>/dev/null
	@echo "Running mycp..."
	./mycp test_input.bin test_output.bin
	@echo ""
	@echo "Verifying files are identical..."
	cmp test_input.bin test_output.bin && echo "✓ Files match perfectly" || echo "✗ Files differ"
	@echo ""
	@echo "Checking file sizes:"
	ls -lh test_input.bin test_output.bin

# Test with large file
test-large: mycp
	@echo "Creating 100MB test file..."
	dd if=/dev/zero of=test_large.bin bs=1M count=100 2>/dev/null
	@echo "Running mycp..."
	time ./mycp test_large.bin test_large_output.bin
	@echo "Verifying..."
	cmp test_large.bin test_large_output.bin && echo "✓ Large file copy successful" || echo "✗ Files differ"
	rm -f test_large.bin test_large_output.bin

# Test error handling
test-errors: mycp
	@echo "Testing error handling..."
	@echo ""
	@echo "1. Non-existent source:"
	./mycp /nonexistent/file.txt /tmp/output.txt 2>&1 || true
	@echo ""
	@echo "2. Source is a directory:"
	./mycp . /tmp/output.txt 2>&1 || true
	@echo ""
	@echo "3. Invalid destination (read-only directory):"
	./mycp /etc/hostname /proc/test.txt 2>&1 || true

# Show help
help:
	@echo "Targets:"
	@echo "  make            - Build mycp"
	@echo "  make debug      - Build with debug symbols (use with gdb)"
	@echo "  make clean      - Remove binaries and test files"
	@echo "  make test       - Test with 10MB file"
	@echo "  make test-large - Test with 100MB file (measures speed)"
	@echo "  make test-errors- Test error handling"
	@echo "  make help       - Show this help"
	@echo ""
	@echo "Usage:"
	@echo "  ./mycp source.txt destination.txt"

.PHONY: all clean debug test test-large test-errors help
