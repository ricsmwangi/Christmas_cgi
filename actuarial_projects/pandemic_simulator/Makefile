# Pandemic Risk Simulator - Makefile
# High-performance pandemic simulation system
# Supports CUDA GPU acceleration, MPI distributed computing, and scientific libraries

# Compiler settings
CC = gcc
CXX = g++
NVCC = nvcc
MPICC = mpicc

# Check for CUDA availability
CUDA_AVAILABLE := $(shell command -v nvcc 2>/dev/null && echo 1 || echo 0)
ifeq ($(CUDA_AVAILABLE),1)
    CUDA_LDFLAGS = -lcudart -lcublas -lcurand -lcusolver
    CFLAGS += -DUSE_CUDA
else
    CUDA_LDFLAGS =
endif

# Check for MPI availability
MPI_AVAILABLE := $(shell command -v mpicc 2>/dev/null && echo 1 || echo 0)
ifeq ($(MPI_AVAILABLE),1)
    MPI_CFLAGS = $(shell mpicc --showme:compile)
    MPI_LDFLAGS = $(shell mpicc --showme:link)
    CFLAGS += -DUSE_MPI
else
    MPI_CFLAGS =
    MPI_LDFLAGS =
    MPI_SOURCES =
    MPI_OBJECTS =
endif

# Library paths and flags
GSL_CFLAGS = $(shell pkg-config --cflags gsl)
GSL_LDFLAGS = $(shell pkg-config --libs gsl)
BLAS_LDFLAGS = -lblas -llapack
OPENMP_LDFLAGS = -fopenmp

# Include directories
INCLUDES = -I./include -I./src -I/usr/local/cuda/include
CUDA_INCLUDES = -I/usr/local/cuda/include

# Target executable
TARGET = pandemic_simulator
LIB_TARGET = libpandemic.a

# Source files
C_SOURCES = \
	src/pandemic_simulator.c \
	src/models/epidemiological/seir_model.c \
	src/models/economic/input_output_model.c \
	src/models/actuarial/pandemic_loss_model.c \
	src/distributed/pandemic_cluster.c \
	src/visualization/plotting.c \
	src/utils/data_loader.c \
	src/utils/statistics.c \
	src/main.c

CUDA_SOURCES = \
	src/gpu/pandemic_gpu.cu \
	src/gpu/cuda_kernels.cu \
	src/gpu/agent_simulation.cu

CXX_SOURCES = \
#	src/models/epidemiological/agent_based_model.cpp \
#	src/models/economic/network_model.cpp

MPI_SOURCES = \
	src/distributed/mpi_communication.c \
	src/distributed/parallel_simulation.c

# Object files
C_OBJECTS = $(C_SOURCES:.c=.o)
CUDA_OBJECTS = $(CUDA_SOURCES:.cu=.o)
CXX_OBJECTS = $(CXX_SOURCES:.cpp=.o)
MPI_OBJECTS = $(MPI_SOURCES:.c=.o)

# All objects
OBJECTS = $(C_OBJECTS) $(CXX_OBJECTS)
ifeq ($(CUDA_AVAILABLE),1)
    OBJECTS += $(CUDA_OBJECTS)
endif
ifeq ($(MPI_AVAILABLE),1)
    OBJECTS += $(MPI_OBJECTS)
endif

# Dependencies
DEPS = $(OBJECTS:.o=.d)

# Build rules
.PHONY: all clean install test benchmark profile debug release docs help

all: $(TARGET)

# Main executable
$(TARGET): $(OBJECTS)
	@echo "🔗 Linking $(TARGET)..."
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o $@ $^ \
		$(GSL_LDFLAGS) $(BLAS_LDFLAGS) \
		$(if $(CUDA_AVAILABLE),$(CUDA_LDFLAGS)) \
		$(MPI_LDFLAGS) $(OPENMP_LDFLAGS) -lm -ldl
	@echo "✅ $(TARGET) built successfully!"

# Static library
$(LIB_TARGET): $(C_OBJECTS) $(CXX_OBJECTS)
	@echo "📚 Creating $(LIB_TARGET)..."
	ar rcs $@ $^
	@echo "✅ $(LIB_TARGET) created successfully!"

# C source compilation
%.o: %.c
	@echo "🔨 Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(MPI_CFLAGS) $(GSL_CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# C++ source compilation
%.o: %.cpp
	@echo "🔨 Compiling $<..."
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# CUDA source compilation
%.o: %.cu
	@echo "🔨 Compiling CUDA $<..."
	@mkdir -p $(dir $@)
	$(NVCC) $(NVCCFLAGS) $(CUDA_INCLUDES) $(INCLUDES) -dc $< -o $@ --compiler-options "-fPIC"

# CUDA device linking
src/gpu/%.o: src/gpu/%.cu
	@echo "🔗 CUDA linking $<..."
	@mkdir -p $(dir $@)
	$(NVCC) $(NVCCFLAGS) $(CUDA_INCLUDES) $(INCLUDES) -dlink $< -o $@

# MPI compilation
src/distributed/%.o: src/distributed/%.c
	@echo "🔨 Compiling distributed $<..."
	@mkdir -p $(dir $@)
ifeq ($(MPI_AVAILABLE),1)
	$(MPICC) $(CFLAGS) $(MPI_CFLAGS) $(GSL_CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@
else
	$(CC) $(CFLAGS) $(GSL_CFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@
endif

# Include dependencies
-include $(DEPS)

# Build configurations
debug: CFLAGS += -g -O0 -DDEBUG
debug: CXXFLAGS += -g -O0 -DDEBUG
debug: NVCCFLAGS += -g -G
debug: all

release: CFLAGS += -DNDEBUG -funroll-loops -fomit-frame-pointer
release: CXXFLAGS += -DNDEBUG -funroll-loops -fomit-frame-pointer
release: NVCCFLAGS += --use_fast_math
release: all

profile: CFLAGS += -pg -g
profile: CXXFLAGS += -pg -g
profile: all

# Testing
test: $(TARGET)
	@echo "🧪 Running test suite..."
	./scripts/run_tests.sh

benchmark: $(TARGET)
	@echo "⚡ Running benchmarks..."
	./scripts/run_benchmarks.sh

# Documentation
docs:
	@echo "📖 Generating documentation..."
	doxygen Doxyfile
	@echo "✅ Documentation generated in docs/html/"

# Installation
install: $(TARGET) $(LIB_TARGET)
	@echo "📦 Installing $(TARGET)..."
	install -d $(DESTDIR)/usr/local/bin
	install -m 755 $(TARGET) $(DESTDIR)/usr/local/bin/
	install -d $(DESTDIR)/usr/local/lib
	install -m 644 $(LIB_TARGET) $(DESTDIR)/usr/local/lib/
	install -d $(DESTDIR)/usr/local/include/pandemic
	cp -r include/* $(DESTDIR)/usr/local/include/pandemic/
	ldconfig
	@echo "✅ Installation complete!"

# Cleaning
clean:
	@echo "🧹 Cleaning build artifacts..."
	rm -rf $(OBJECTS) $(DEPS) $(TARGET) $(LIB_TARGET)
	rm -rf build/
	find . -name "*.o" -delete
	find . -name "*.d" -delete
	find . -name "*.a" -delete
	find . -name "*~" -delete
	find . -name "*.swp" -delete
	find . -name "*.swo" -delete
	@echo "✅ Clean complete!"

distclean: clean
	@echo "🧹 Deep cleaning..."
	rm -rf docs/html/
	rm -rf test_results/
	rm -rf benchmark_results/
	rm -rf profiling_data/
	@echo "✅ Deep clean complete!"

# Development helpers
format:
	@echo "🎨 Formatting code..."
	find src/ include/ -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" | \
		xargs clang-format -i -style=file
	@echo "✅ Code formatted!"

lint:
	@echo "🔍 Running linter..."
	find src/ include/ -name "*.c" -o -name "*.cpp" | \
		xargs cppcheck --enable=all --std=c11 --std=c++17 --quiet
	@echo "✅ Linting complete!"

check: format lint test
	@echo "✅ All checks passed!"

# Performance analysis
perf: $(TARGET)
	@echo "📊 Running performance analysis..."
	perf record -g ./$(TARGET) --benchmark
	perf report

# CUDA-specific targets
cuda-check:
	@echo "🔍 Checking CUDA installation..."
	$(NVCC) --version
	nvidia-smi
	@echo "✅ CUDA check complete!"

cuda-build: $(CUDA_OBJECTS)
	@echo "🔗 Building CUDA components..."
	$(NVCC) $(NVCCFLAGS) -lib -o libpandemic_cuda.a $^
	@echo "✅ CUDA library built!"

# MPI-specific targets
mpi-check:
	@echo "🔍 Checking MPI installation..."
	mpirun --version
	ompi_info | head -20
	@echo "✅ MPI check complete!"

mpi-test: $(TARGET)
	@echo "🔍 Testing MPI functionality..."
	mpirun -np 4 ./$(TARGET) --test-mpi
	@echo "✅ MPI test complete!"

# Dependency installation
deps:
	@echo "📦 Installing dependencies..."
	./scripts/install_dependencies.sh

# Help target
help:
	@echo "🌍 Pandemic Risk Simulator - Build System"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - Build the main executable (default)"
	@echo "  lib          - Build static library"
	@echo "  debug        - Build with debug symbols"
	@echo "  release      - Build optimized release version"
	@echo "  profile      - Build with profiling support"
	@echo "  test         - Run test suite"
	@echo "  benchmark    - Run performance benchmarks"
	@echo "  docs         - Generate documentation"
	@echo "  install      - Install to system"
	@echo "  clean        - Clean build artifacts"
	@echo "  distclean    - Deep clean including docs and results"
	@echo "  format       - Format code with clang-format"
	@echo "  lint         - Run static analysis"
	@echo "  check        - Run format, lint, and tests"
	@echo "  perf         - Run performance profiling"
	@echo "  cuda-check   - Verify CUDA installation"
	@echo "  cuda-build   - Build CUDA components only"
	@echo "  mpi-check    - Verify MPI installation"
	@echo "  mpi-test     - Test MPI functionality"
	@echo "  deps         - Install build dependencies"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make debug          # Build debug version"
	@echo "  make test           # Run tests"
	@echo "  make benchmark      # Run benchmarks"
	@echo "  make install        # Install to /usr/local"
	@echo "  make clean && make  # Clean and rebuild"
	@echo ""
	@echo "For development:"
	@echo "  make check          # Full quality check"
	@echo "  make perf           # Performance profiling"
	@echo "  make docs           # Generate docs"

# Default target
.DEFAULT_GOAL := all

# Special build configurations
fast: CFLAGS += -Ofast -ffast-math
fast: CXXFLAGS += -Ofast -ffast-math
fast: NVCCFLAGS += --use_fast_math
fast: all

small: CFLAGS += -Os
small: CXXFLAGS += -Os
small: all

# Print build info
build-info:
	@echo "🔧 Build Information:"
	@echo "  CC: $(CC)"
	@echo "  CXX: $(CXX)"
	@echo "  NVCC: $(NVCC)"
	@echo "  MPICC: $(MPICC)"
	@echo "  CFLAGS: $(CFLAGS)"
	@echo "  CXXFLAGS: $(CXXFLAGS)"
	@echo "  NVCCFLAGS: $(NVCCFLAGS)"
	@echo "  Target: $(TARGET)"
	@echo "  Sources: $(words $(C_SOURCES) $(CXX_SOURCES) $(CUDA_SOURCES) $(MPI_SOURCES))"
	@echo "  Objects: $(words $(OBJECTS))"

# Silent build (less verbose)
silent: CFLAGS += -Wno-all
silent: CXXFLAGS += -Wno-all
silent: all

# Verbose build (more detailed)
verbose: CFLAGS += -v
verbose: CXXFLAGS += -v
verbose: NVCCFLAGS += -v
verbose: all

# Cross-compilation support (example for ARM64)
arm64: CC = aarch64-linux-gnu-gcc
arm64: CXX = aarch64-linux-gnu-g++
arm64: CFLAGS += -march=armv8-a
arm64: CXXFLAGS += -march=armv8-a
arm64: all

# Docker build
docker-build:
	@echo "🐳 Building Docker image..."
	docker build -t pandemic-simulator .
	@echo "✅ Docker image built!"

docker-run: docker-build
	@echo "🐳 Running in Docker..."
	docker run --gpus all -it pandemic-simulator

# CI/CD support
ci: deps check benchmark
	@echo "✅ CI checks passed!"

# Emergency clean (removes everything except source)
emergency-clean:
	@echo "🚨 Emergency clean - removing ALL generated files!"
	find . -name "*.o" -delete
	find . -name "*.d" -delete
	find . -name "*.a" -delete
	find . -name "$(TARGET)" -delete
	rm -rf build/ docs/html/ test_results/ benchmark_results/
	@echo "✅ Emergency clean complete!"

# Print file sizes
size:
	@echo "📏 Build artifact sizes:"
	@ls -lh $(TARGET) $(LIB_TARGET) 2>/dev/null || echo "No build artifacts found"
	@echo "Source code:"
	@find src/ include/ -name "*.c" -o -name "*.cpp" -o -name "*.cu" -o -name "*.h" | wc -l | xargs echo "  Files:"
	@find src/ include/ -name "*.c" -o -name "*.cpp" -o -name "*.cu" -o -name "*.h" | xargs wc -l | tail -1 | awk '{print "  Lines: " $$1}'

# Dependency graph
deps-graph:
	@echo "📊 Generating dependency graph..."
	$(CC) $(CFLAGS) $(INCLUDES) -MM $(C_SOURCES) > deps.mk
	@echo "✅ Dependency graph generated in deps.mk"

# Memory check
memcheck: $(TARGET)
	@echo "🔍 Running memory checks..."
	valgrind --leak-check=full --show-leak-kinds=all ./$(TARGET) --test
	@echo "✅ Memory check complete!"

# Thread sanitizer
tsan: CFLAGS += -fsanitize=thread -g
tsan: CXXFLAGS += -fsanitize=thread -g
tsan: LDFLAGS += -fsanitize=thread
tsan: all

# Address sanitizer
asan: CFLAGS += -fsanitize=address -g
asan: CXXFLAGS += -fsanitize=address -g
asan: LDFLAGS += -fsanitize=address
asan: all

# Undefined behavior sanitizer
ubsan: CFLAGS += -fsanitize=undefined -g
ubsan: CXXFLAGS += -fsanitize=undefined -g
ubsan: LDFLAGS += -fsanitize=undefined
ubsan: all

# All sanitizers
sanitize: tsan asan ubsan

# Code coverage
coverage: CFLAGS += --coverage -g -O0
coverage: CXXFLAGS += --coverage -g -O0
coverage: LDFLAGS += --coverage
coverage: all
	@echo "📊 Generating coverage report..."
	lcov --capture --directory . --output-file coverage.info
	genhtml coverage.info --output-directory coverage_report/
	@echo "✅ Coverage report generated in coverage_report/"

# Static analysis
analyze:
	@echo "🔍 Running static analysis..."
	cppcheck --enable=all --std=c11 --std=c++17 --quiet src/ include/
	clang-tidy src/*.c include/*.h -- -I./include
	@echo "✅ Static analysis complete!"

# Performance profiling with callgrind
callgrind: $(TARGET)
	@echo "📊 Running callgrind profiling..."
	valgrind --tool=callgrind --callgrind-out-file=callgrind.out ./$(TARGET) --benchmark
	kcachegrind callgrind.out
	@echo "✅ Callgrind profiling complete!"

# Heap profiling
heap: $(TARGET)
	@echo "📊 Running heap profiling..."
	valgrind --tool=massif --massif-out-file=massif.out ./$(TARGET) --benchmark
	ms_print massif.out > heap_profile.txt
	@echo "✅ Heap profile generated in heap_profile.txt"

# Build time measurement
time: clean
	@echo "⏱️  Measuring build time..."
	time make -j$(nproc)
	@echo "✅ Build time measured!"

# Parallel build test
parallel-test:
	@echo "🔄 Testing parallel builds..."
	for jobs in 1 2 4 8 16; do \
		echo "Testing with $$jobs jobs..."; \
		time make -j$$jobs clean all >/dev/null 2>&1; \
	done
	@echo "✅ Parallel build test complete!"

# Build system validation
validate:
	@echo "✅ Validating build system..."
	@test -f Makefile || (echo "❌ Makefile not found"; exit 1)
	@test -d src/ || (echo "❌ src/ directory not found"; exit 1)
	@test -d include/ || (echo "❌ include/ directory not found"; exit 1)
	@grep -q "pandemic_simulator" Makefile || (echo "❌ Target not found in Makefile"; exit 1)
	@echo "✅ Build system validation passed!"

# Final status
status:
	@echo "📊 Project Status:"
	@echo "  Sources: $(shell find src/ -name "*.c" -o -name "*.cpp" -o -name "*.cu" | wc -l) files"
	@echo "  Headers: $(shell find include/ -name "*.h" -o -name "*.hpp" | wc -l) files"
	@echo "  Tests: $(shell find tests/ -name "*.c" -o -name "*.cpp" | wc -l) files"
	@echo "  Lines of code: $(shell find src/ include/ -name "*.c" -o -name "*.h" -o -name "*.cpp" -o -name "*.hpp" | xargs wc -l | tail -1 | awk '{print $$1}')"
	@echo "  Build target: $(TARGET)"
	@echo "  Library target: $(LIB_TARGET)"
	@if [ -f $(TARGET) ]; then echo "  ✅ Executable built"; else echo "  ❌ Executable not built"; fi
	@if [ -f $(LIB_TARGET) ]; then echo "  ✅ Library built"; else echo "  ❌ Library not built"; fi