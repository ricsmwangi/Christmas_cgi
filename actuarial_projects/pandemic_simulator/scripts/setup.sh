#!/bin/bash
# Pandemic Risk Simulator - Setup Script
# Automated installation and configuration script

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Logging functions
log_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

log_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

log_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

log_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Check if running as root
check_root() {
    if [[ $EUID -eq 0 ]]; then
        log_error "This script should not be run as root"
        exit 1
    fi
}

# Detect operating system
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        if command -v apt-get >/dev/null 2>&1; then
            OS="ubuntu"
        elif command -v yum >/dev/null 2>&1; then
            OS="centos"
        elif command -v zypper >/dev/null 2>&1; then
            OS="opensuse"
        else
            OS="linux"
        fi
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        OS="macos"
    else
        log_error "Unsupported operating system: $OSTYPE"
        exit 1
    fi

    log_info "Detected OS: $OS"
}

# Install system dependencies
install_dependencies() {
    log_info "Installing system dependencies..."

    case $OS in
        ubuntu)
            sudo apt-get update
            sudo apt-get install -y \
                build-essential \
                cmake \
                git \
                libgsl-dev \
                libopenmpi-dev \
                openmpi-bin \
                python3 \
                python3-pip \
                python3-dev \
                clang-format \
                cppcheck \
                doxygen \
                graphviz \
                valgrind \
                linux-tools-common \
                nvidia-cuda-toolkit
            ;;
        centos)
            sudo yum groupinstall -y "Development Tools"
            sudo yum install -y \
                cmake \
                git \
                gsl-devel \
                openmpi-devel \
                python3 \
                python3-pip \
                python3-devel \
                clang \
                doxygen \
                graphviz \
                valgrind \
                cuda-toolkit
            ;;
        opensuse)
            sudo zypper install -y -t pattern devel_basis
            sudo zypper install -y \
                cmake \
                git \
                gsl-devel \
                openmpi-devel \
                python3 \
                python3-pip \
                python3-devel \
                clang \
                doxygen \
                graphviz \
                valgrind \
                cuda-toolkit
            ;;
        macos)
            if ! command -v brew >/dev/null 2>&1; then
                log_error "Homebrew is required on macOS. Please install it first."
                exit 1
            fi

            brew install \
                cmake \
                git \
                gsl \
                open-mpi \
                python3 \
                clang-format \
                cppcheck \
                doxygen \
                graphviz \
                valgrind

            # CUDA not available on macOS
            log_warning "CUDA not available on macOS, GPU acceleration disabled"
            ;;
        *)
            log_error "Unsupported OS for automatic dependency installation"
            log_info "Please manually install: cmake, git, gsl, openmpi, python3, clang-format, cppcheck, doxygen"
            exit 1
            ;;
    esac

    log_success "System dependencies installed"
}

# Install Python dependencies
install_python_deps() {
    log_info "Installing Python dependencies..."

    pip3 install --user \
        numpy \
        scipy \
        pandas \
        matplotlib \
        seaborn \
        plotly \
        dash \
        networkx \
        mesa \
        dask \
        distributed \
        pytest \
        pytest-cov \
        black \
        mypy \
        sphinx \
        sphinx-rtd-theme

    log_success "Python dependencies installed"
}

# Check CUDA installation
check_cuda() {
    if command -v nvidia-smi >/dev/null 2>&1; then
        log_info "CUDA detected:"
        nvidia-smi --query-gpu=name,memory.total --format=csv,noheader,nounits
        return 0
    else
        log_warning "CUDA not detected, GPU acceleration will be disabled"
        return 1
    fi
}

# Check MPI installation
check_mpi() {
    if command -v mpirun >/dev/null 2>&1; then
        log_info "MPI detected:"
        mpirun --version | head -1
        return 0
    else
        log_warning "MPI not detected, distributed computing will be disabled"
        return 1
    fi
}

# Build the project
build_project() {
    log_info "Building Pandemic Simulator..."

    # Create build directory
    mkdir -p build
    cd build

    # Configure with CMake
    local cmake_args="-DCMAKE_BUILD_TYPE=Release"

    if check_cuda; then
        cmake_args="$cmake_args -DUSE_CUDA=ON"
    else
        cmake_args="$cmake_args -DUSE_CUDA=OFF"
    fi

    if check_mpi; then
        cmake_args="$cmake_args -DUSE_MPI=ON"
    else
        cmake_args="$cmake_args -DUSE_MPI=OFF"
    fi

    log_info "CMake configuration: $cmake_args"
    cmake $cmake_args ..

    # Build
    local num_cores=$(nproc 2>/dev/null || echo 4)
    log_info "Building with $num_cores cores..."
    make -j$num_cores

    cd ..
    log_success "Build completed"
}

# Run tests
run_tests() {
    log_info "Running tests..."

    cd build
    if make test; then
        log_success "All tests passed"
    else
        log_error "Some tests failed"
        exit 1
    fi
    cd ..
}

# Create data directories
setup_data_dirs() {
    log_info "Setting up data directories..."

    mkdir -p data/{epidemiological,economic,insurance,geographic}
    mkdir -p models/{trained,checkpoints}
    mkdir -p results/{simulations,plots,reports}
    mkdir -p logs

    log_success "Data directories created"
}

# Create configuration files
create_config() {
    log_info "Creating default configuration..."

    cat > pandemic_config.json << EOF
{
    "simulation": {
        "population_size": 8000000000,
        "num_regions": 195,
        "time_horizon": 365,
        "num_scenarios": 1000
    },
    "epidemiological": {
        "beta": 0.3,
        "gamma": 0.1,
        "mu": 0.02,
        "intervention_strength": 0.0
    },
    "economic": {
        "num_sectors": 50,
        "fiscal_stimulus": 0.1,
        "monetary_easing": 0.05
    },
    "actuarial": {
        "mortality_multiplier": 2.0,
        "morbidity_multiplier": 1.5,
        "reinsurance_retention": 0.1
    },
    "compute": {
        "use_gpu": true,
        "use_mpi": false,
        "num_threads": 8,
        "random_seed": 42
    },
    "output": {
        "save_results": true,
        "generate_plots": true,
        "export_csv": true,
        "export_json": true
    }
}
EOF

    log_success "Default configuration created"
}

# Print usage information
print_usage() {
    echo "Pandemic Risk Simulator - Setup Script"
    echo ""
    echo "Usage: $0 [OPTIONS]"
    echo ""
    echo "Options:"
    echo "  --help          Show this help message"
    echo "  --no-deps       Skip dependency installation"
    echo "  --no-python     Skip Python dependency installation"
    echo "  --no-build      Skip building the project"
    echo "  --no-test       Skip running tests"
    echo "  --clean         Clean build artifacts before setup"
    echo ""
    echo "Examples:"
    echo "  $0                    # Full setup"
    echo "  $0 --no-deps         # Skip system dependencies"
    echo "  $0 --clean           # Clean and rebuild"
}

# Main setup function
main() {
    local install_deps=true
    local install_python=true
    local build=true
    local run_test=true
    local clean=false

    # Parse command line arguments
    while [[ $# -gt 0 ]]; do
        case $1 in
            --help)
                print_usage
                exit 0
                ;;
            --no-deps)
                install_deps=false
                shift
                ;;
            --no-python)
                install_python=false
                shift
                ;;
            --no-build)
                build=false
                shift
                ;;
            --no-test)
                run_test=false
                shift
                ;;
            --clean)
                clean=true
                shift
                ;;
            *)
                log_error "Unknown option: $1"
                print_usage
                exit 1
                ;;
        esac
    done

    log_info "🌍 Pandemic Risk Simulator Setup"
    log_info "================================="

    check_root
    detect_os

    if $clean; then
        log_info "Cleaning previous build..."
        rm -rf build/
        rm -rf results/
        rm -rf logs/
    fi

    if $install_deps; then
        install_dependencies
    fi

    if $install_python; then
        install_python_deps
    fi

    setup_data_dirs
    create_config

    if $build; then
        build_project
    fi

    if $run_test; then
        run_tests
    fi

    log_success "🎉 Setup completed successfully!"
    echo ""
    log_info "Next steps:"
    echo "  1. Run './build/pandemic_simulator --help' to see usage options"
    echo "  2. Run './build/pandemic_simulator --test' to run basic tests"
    echo "  3. Run './build/pandemic_simulator --benchmark' for performance testing"
    echo "  4. Check the README.md for detailed usage instructions"
    echo ""
    log_info "Documentation: https://pandemic-simulator.readthedocs.io"
    log_info "Support: info@pandemic-simulator.org"
}

# Run main function with all arguments
main "$@"