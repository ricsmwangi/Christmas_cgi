# Catastrophe Engine Makefile

CC = gcc
CFLAGS = -Wall -Wextra -pedantic -O3 -std=c11 -march=native
CFLAGS += -D_GNU_SOURCE -D_FORTIFY_SOURCE=2
CFLAGS += -I./include -I/usr/include/postgresql -I/usr/include/gdal

# Linker flags (basic version - add libraries as you install them)
LDFLAGS = -lm -lpthread
# LDFLAGS += -lcurl -lwebsockets -lpq -lgdal -lproj  # Uncomment when libraries are installed

# Directories
SRC_DIR = src
OBJ_DIR = obj
BIN_DIR = bin
INCLUDE_DIR = include

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.c) \
       $(wildcard $(SRC_DIR)/calculation/*.c) \
       $(wildcard $(SRC_DIR)/data/*.c) \
       $(wildcard $(SRC_DIR)/analysis/*.c) \
       $(wildcard $(SRC_DIR)/distributed/*.c)
OBJS = $(SRCS:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)
DEPS = $(OBJS:.o=.d)

# Main executable
TARGET = $(BIN_DIR)/catastrophe_engine

# Test executable
TEST_TARGET = $(BIN_DIR)/catastrophe_test

# Default target
all: directories $(TARGET)

# Create directories
directories:
	@mkdir -p $(OBJ_DIR) $(BIN_DIR)

# Main build rule
$(TARGET): $(OBJS)
	@echo "Linking $@..."
	@$(CC) $(OBJS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Object file compilation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Test build
test: directories $(TEST_TARGET)
	@echo "Running tests..."
	@./$(TEST_TARGET)

$(TEST_TARGET): $(filter-out $(OBJ_DIR)/main.o, $(OBJS)) $(OBJ_DIR)/test_main.o
	@echo "Linking test executable..."
	@$(CC) $^ -o $@ $(LDFLAGS) -lcriterion

$(OBJ_DIR)/test_main.o: tests/test_main.c
	@echo "Compiling test main..."
	@$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Development targets
debug: CFLAGS += -g -O0 -DDEBUG
debug: clean all

profile: CFLAGS += -pg
profile: LDFLAGS += -pg
profile: clean all

# Code quality
lint:
	@echo "Running clang-tidy..."
	@clang-tidy $(SRCS) -- $(CFLAGS)

format:
	@echo "Formatting code..."
	@clang-format -i $(SRCS) $(wildcard $(INCLUDE_DIR)/*.h)

# Performance analysis
perf: $(TARGET)
	@echo "Running performance analysis..."
	@perf record ./$(TARGET) --benchmark
	@perf report

# Memory checking
memcheck: $(TARGET)
	@echo "Running valgrind..."
	@valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --test

# Benchmarking
benchmark: $(TARGET)
	@echo "Running benchmarks..."
	@./$(TARGET) --benchmark-mode --iterations=1000

# Documentation
docs:
	@echo "Generating documentation..."
	@doxygen Doxyfile

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(OBJ_DIR) $(BIN_DIR)
	@rm -f *.gcda *.gcno *.gcov gmon.out perf.data*

# Deep clean (including generated files)
distclean: clean
	@echo "Deep cleaning..."
	@rm -rf docs/html docs/latex
	@rm -f *.log *.tmp

# Installation
install: $(TARGET)
	@echo "Installing to /usr/local/bin..."
	@install -m 755 $(TARGET) /usr/local/bin/
	@install -m 644 config/*.json /etc/catastrophe_engine/

# Uninstall
uninstall:
	@echo "Uninstalling..."
	@rm -f /usr/local/bin/catastrophe_engine
	@rm -rf /etc/catastrophe_engine

# Package creation
package: all
	@echo "Creating distribution package..."
	@tar -czf catastrophe_engine-$(shell date +%Y%m%d).tar.gz \
		--exclude='.git' --exclude='*.o' --exclude='bin/*' \
		--exclude='docs' --exclude='*.log' .

# Development server (for testing)
dev-server: $(TARGET)
	@echo "Starting development server..."
	@./$(TARGET) --dev-mode --port=8080 --log-level=debug

# Production deployment check
deploy-check: all test lint
	@echo "Deployment checks passed!"
	@echo "- Build: ✓"
	@echo "- Tests: ✓"
	@echo "- Code quality: ✓"

# Help target
help:
	@echo "Available targets:"
	@echo "  all          - Build the main executable"
	@echo "  test         - Build and run tests"
	@echo "  debug        - Build with debug symbols"
	@echo "  profile      - Build with profiling support"
	@echo "  lint         - Run static analysis"
	@echo "  format       - Format source code"
	@echo "  perf         - Run performance analysis"
	@echo "  memcheck     - Run memory leak check"
	@echo "  benchmark    - Run performance benchmarks"
	@echo "  docs         - Generate documentation"
	@echo "  clean        - Remove build artifacts"
	@echo "  install      - Install to system"
	@echo "  package      - Create distribution package"
	@echo "  dev-server   - Start development server"
	@echo "  deploy-check - Run all pre-deployment checks"
	@echo "  help         - Show this help message"

.PHONY: all directories test debug profile lint format perf memcheck benchmark docs clean distclean install uninstall package dev-server deploy-check help

# Default target reminder
.DEFAULT_GOAL := all