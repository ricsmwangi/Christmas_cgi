# AI Underwriting Engine Makefile
# Build system for the AI underwriting engine with AI/ML integration

# Compiler and flags
CC = gcc
CFLAGS = -std=c99 -Wall -Wextra -Werror -pedantic -O2 -g -fPIC
LDFLAGS = -lpthread -lm -ldl

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
OBJ_DIR = $(BUILD_DIR)/obj
BIN_DIR = $(BUILD_DIR)/bin
LIB_DIR = $(BUILD_DIR)/lib

# Source files
CORE_SOURCES = \
	$(SRC_DIR)/main.c \
	$(SRC_DIR)/core/engine.c \
	$(SRC_DIR)/core/decision_maker.c \
	$(SRC_DIR)/core/rule_engine.c \
	$(SRC_DIR)/ai/model_manager.c \
	$(SRC_DIR)/ai/inference_engine.c \
	$(SRC_DIR)/ai/feature_engineering.c \
	$(SRC_DIR)/data/data_processor.c \
	$(SRC_DIR)/api/api_server.c

# Object files
CORE_OBJECTS = $(CORE_SOURCES:$(SRC_DIR)/%.c=$(OBJ_DIR)/%.o)

# Executable
TARGET = $(BIN_DIR)/ai_underwriting_engine

# Include directories
INCLUDES = -I$(INC_DIR) -I$(INC_DIR)/core -I$(INC_DIR)/ai -I$(INC_DIR)/data -I$(INC_DIR)/api

# Dependencies
DEPS = $(CORE_OBJECTS:.o=.d)

# Default target
.PHONY: all
all: directories $(TARGET)

# Create directories
.PHONY: directories
directories:
	@mkdir -p $(OBJ_DIR)/core $(OBJ_DIR)/ai $(OBJ_DIR)/data $(OBJ_DIR)/api $(BIN_DIR) $(LIB_DIR)

# Main executable
$(TARGET): $(CORE_OBJECTS)
	@echo "Linking $@..."
	$(CC) $(CORE_OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build complete: $@"

# Compile source files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -rf $(BUILD_DIR)

# Clean everything including dependencies
.PHONY: distclean
distclean: clean
	@echo "Cleaning all generated files..."
	rm -f $(SRC_DIR)/*~ $(INC_DIR)/*~

# Run the engine
.PHONY: run
run: all
	@echo "Running AI Underwriting Engine..."
	./$(TARGET)

# Run in test mode
.PHONY: test
test: all
	@echo "Running AI Underwriting Engine in test mode..."
	./$(TARGET) --test --verbose

# Run with custom config
.PHONY: run-config
run-config: all
	@echo "Running AI Underwriting Engine with custom config..."
	./$(TARGET) --config config.json --verbose

# Debug build
.PHONY: debug
debug: CFLAGS += -DDEBUG -g3 -O0
debug: clean all

# Release build
.PHONY: release
release: CFLAGS += -DNDEBUG -O3 -march=native
release: clean all

# Install (copy to system)
.PHONY: install
install: all
	@echo "Installing AI Underwriting Engine..."
	install -d $(DESTDIR)/usr/local/bin
	install $(TARGET) $(DESTDIR)/usr/local/bin/
	install -d $(DESTDIR)/usr/local/include/ai_underwriting_engine
	cp -r $(INC_DIR)/* $(DESTDIR)/usr/local/include/ai_underwriting_engine/

# Uninstall
.PHONY: uninstall
uninstall:
	@echo "Uninstalling AI Underwriting Engine..."
	rm -f $(DESTDIR)/usr/local/bin/ai_underwriting_engine
	rm -rf $(DESTDIR)/usr/local/include/ai_underwriting_engine

# Code analysis
.PHONY: analyze
analyze:
	@echo "Running static analysis..."
	cppcheck --enable=all --std=c99 --language=c --platform=unix64 \
		--suppress=missingIncludeSystem $(INCLUDES) $(CORE_SOURCES)

# Format code
.PHONY: format
format:
	@echo "Formatting code..."
	find $(SRC_DIR) $(INC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format -i

# Check formatting
.PHONY: check-format
check-format:
	@echo "Checking code formatting..."
	find $(SRC_DIR) $(INC_DIR) -name "*.c" -o -name "*.h" | xargs clang-format --dry-run --Werror

# Generate documentation
.PHONY: docs
docs:
	@echo "Generating documentation..."
	doxygen Doxyfile

# Count lines of code
.PHONY: cloc
cloc:
	@echo "Counting lines of code..."
	cloc $(SRC_DIR) $(INC_DIR)

# Show build info
.PHONY: info
info:
	@echo "AI Underwriting Engine Build Information:"
	@echo "  Compiler: $(CC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  LDFLAGS: $(LDFLAGS)"
	@echo "  Source files: $(words $(CORE_SOURCES))"
	@echo "  Target: $(TARGET)"

# Help
.PHONY: help
help:
	@echo "AI Underwriting Engine Makefile"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the engine (default)"
	@echo "  clean        - Remove build artifacts"
	@echo "  distclean    - Remove all generated files"
	@echo "  run          - Build and run the engine"
	@echo "  test         - Build and run in test mode"
	@echo "  debug        - Build with debug flags"
	@echo "  release      - Build optimized release"
	@echo "  install      - Install to system"
	@echo "  uninstall    - Remove from system"
	@echo "  analyze      - Run static analysis"
	@echo "  format       - Format code with clang-format"
	@echo "  docs         - Generate documentation"
	@echo "  cloc         - Count lines of code"
	@echo "  info         - Show build information"
	@echo "  help         - Show this help message"

# Dependency tracking
$(OBJ_DIR)/%.d: $(SRC_DIR)/%.c
	@mkdir -p $(dir $@)
	$(CC) $(INCLUDES) -MM -MT $(@:.d=.o) $< > $@

# Prevent make from deleting intermediate files
.PRECIOUS: $(OBJ_DIR)/%.o

# Default goal
.DEFAULT_GOAL := all