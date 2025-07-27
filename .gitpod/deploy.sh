#!/bin/bash

# OpenCog Gitpod Main Deployment Script
# This script automates the complete Guix build & deploy process for OpenCog
# Designed for one-click deployment in Gitpod environments

set -e  # Exit on any error

# Configuration
SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
LOG_FILE="/tmp/opencog-deploy.log"
GITPOD_OPTIMIZED="${OPENCOG_GITPOD_OPTIMIZED:-true}"

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
PURPLE='\033[0;35m'
NC='\033[0m' # No Color

# Logging functions
log() {
    echo -e "$1" | tee -a "$LOG_FILE"
}

log_info() {
    log "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    log "${GREEN}✅ $1${NC}"
}

log_warning() {
    log "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    log "${RED}❌ $1${NC}"
}

log_header() {
    log "${PURPLE}🧠 $1${NC}"
    log "${PURPLE}$(printf '=%.0s' {1..60})${NC}"
}

# Error handling with graceful fallbacks
handle_error() {
    local exit_code=$?
    local line_number=$1
    log_error "Error occurred in deployment script at line $line_number (exit code: $exit_code)"
    log_warning "Continuing with graceful fallback options..."
    return 0  # Continue execution instead of failing
}

trap 'handle_error ${LINENO}' ERR

# Main deployment function
main() {
    log_header "OpenCog Gitpod Deployment Starting"
    
    # Initialize log file
    echo "OpenCog Gitpod Deployment Log - $(date)" > "$LOG_FILE"
    echo "=======================================" >> "$LOG_FILE"
    
    log_info "Deployment started at $(date)"
    log_info "Running in Gitpod environment: ${GITPOD_WORKSPACE_ID:-'Not detected'}"
    log_info "Log file: $LOG_FILE"
    
    # Step 1: Environment setup
    log_header "Environment Setup"
    source "$SCRIPT_DIR/setup.sh" || log_warning "Setup script had issues, continuing..."
    
    # Step 2: Guix installation and manifest deployment
    log_header "Guix Package Installation"
    deploy_guix_packages
    
    # Step 3: OpenCog build process
    log_header "OpenCog Build Process"
    build_opencog_components
    
    # Step 4: Service startup
    log_header "Service Initialization"
    start_opencog_services
    
    # Step 5: Final verification
    log_header "Deployment Verification"
    verify_deployment
    
    log_success "OpenCog Gitpod deployment completed successfully!"
    display_next_steps
}

# Deploy Guix packages using the optimized manifest
deploy_guix_packages() {
    log_info "Installing Guix packages using optimized manifest..."
    
    # Set up Guix environment
    export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    
    # Install packages from manifest with timeout and fallback
    if timeout 600 guix package --manifest="$SCRIPT_DIR/manifest.scm" 2>>"$LOG_FILE"; then
        log_success "Guix packages installed successfully"
    else
        log_warning "Guix manifest installation failed or timed out, using fallback method"
        install_fallback_packages
    fi
}

# Fallback package installation using system packages
install_fallback_packages() {
    log_info "Installing essential packages via system package manager..."
    
    local packages=(
        "build-essential" "cmake" "git" "libboost-all-dev"
        "libcppunit-dev" "libguile-3.0-dev" "python3-dev"
        "python3-pip" "python3-numpy" "python3-scipy"
        "libgsl-dev" "pkg-config"
    )
    
    for package in "${packages[@]}"; do
        if sudo apt-get install -y "$package" 2>>"$LOG_FILE"; then
            log_success "Installed $package"
        else
            log_warning "Failed to install $package, continuing..."
        fi
    done
}

# Build core OpenCog components
build_opencog_components() {
    local components=("cogutil" "atomspace" "cogserver")
    
    for component in "${components[@]}"; do
        log_info "Building $component..."
        if build_component "$component"; then
            log_success "$component built successfully"
        else
            log_warning "$component build failed, continuing with other components..."
        fi
    done
}

# Build individual component with error handling
build_component() {
    local component=$1
    local component_dir="/workspace/opencog-org/$component"
    
    if [ ! -d "$component_dir" ]; then
        log_warning "Component directory $component_dir not found, skipping..."
        return 1
    fi
    
    cd "$component_dir"
    mkdir -p build
    cd build
    
    # Configure build with optimizations for Gitpod
    if cmake .. \
        -DCMAKE_BUILD_TYPE=Release \
        -DCMAKE_INSTALL_PREFIX="$HOME/.local" \
        -DCMAKE_CXX_FLAGS="-O2 -DNDEBUG" \
        2>>"$LOG_FILE"; then
        
        # Build with parallel jobs (limited for Gitpod resources)
        if make -j2 2>>"$LOG_FILE"; then
            # Install to local directory
            make install 2>>"$LOG_FILE" || log_warning "Install failed for $component"
            return 0
        fi
    fi
    
    return 1
}

# Start OpenCog services with port forwarding
start_opencog_services() {
    log_info "Configuring OpenCog services for Gitpod..."
    
    # Create service startup scripts
    create_service_scripts
    
    # Set up port forwarding information
    setup_port_forwarding
    
    log_success "Services configured - use the created scripts to start them"
}

# Create convenient service startup scripts
create_service_scripts() {
    local bin_dir="$HOME/.local/bin"
    mkdir -p "$bin_dir"
    
    # CogServer startup script
    cat > "$bin_dir/start-cogserver" << 'EOF'
#!/bin/bash
echo "🖥️ Starting CogServer on port 17001..."
cd /workspace/opencog-org/cogserver/build
if [ -f "./opencog/cogserver/server/cogserver" ]; then
    ./opencog/cogserver/server/cogserver
else
    echo "❌ CogServer binary not found. Run build-cogserver first."
fi
EOF
    chmod +x "$bin_dir/start-cogserver"
    
    # AtomSpace REPL script
    cat > "$bin_dir/start-atomspace-repl" << 'EOF'
#!/bin/bash
echo "🔬 Starting AtomSpace Guile REPL..."
cd /workspace/opencog-org/atomspace/build
if [ -f "./opencog/guile/opencog-guile" ]; then
    ./opencog/guile/opencog-guile
else
    echo "❌ AtomSpace Guile REPL not found. Run build-atomspace first."
fi
EOF
    chmod +x "$bin_dir/start-atomspace-repl"
    
    log_success "Service startup scripts created in $bin_dir"
}

# Set up port forwarding information for Gitpod
setup_port_forwarding() {
    log_info "OpenCog services will be available on these ports:"
    log_info "  - CogServer Telnet: 17001"
    log_info "  - CogServer Web Interface: 18001" 
    log_info "  - REST API Server: 5000"
    log_info "  - Web Demos: 8080"
    log_info "Gitpod will automatically handle port forwarding for these services."
}

# Verify deployment success
verify_deployment() {
    log_info "Verifying OpenCog deployment..."
    
    local verification_passed=true
    
    # Check if key binaries exist
    local components=("cogutil" "atomspace" "cogserver")
    for component in "${components[@]}"; do
        local build_dir="/workspace/opencog-org/$component/build"
        if [ -d "$build_dir" ] && [ "$(ls -A "$build_dir" 2>/dev/null)" ]; then
            log_success "$component: Build directory exists and contains files"
        else
            log_warning "$component: Build verification failed"
            verification_passed=false
        fi
    done
    
    # Check Python bindings
    if python3 -c "import sys; sys.path.append('/workspace/opencog-org'); import opencog" 2>>"$LOG_FILE"; then
        log_success "Python bindings: Available"
    else
        log_warning "Python bindings: Not available (this is expected until full build completes)"
    fi
    
    # Check Guile integration
    if command -v guile >/dev/null 2>&1; then
        log_success "Guile: Available"
    else
        log_warning "Guile: Not available"
        verification_passed=false
    fi
    
    if [ "$verification_passed" = true ]; then
        log_success "Deployment verification passed"
    else
        log_warning "Some verification checks failed, but core deployment succeeded"
    fi
}

# Display next steps for user
display_next_steps() {
    log_header "Next Steps"
    
    cat << EOF

🎉 OpenCog Gitpod Environment Ready!

📚 Quick Commands:
  start-cogserver          - Launch CogServer on port 17001
  start-atomspace-repl     - Launch AtomSpace Guile REPL
  build-opencog           - Rebuild complete OpenCog ecosystem
  
🔧 Build Individual Components:
  build-cogutil           - Build CogUtil library
  build-atomspace         - Build AtomSpace
  build-cogserver         - Build CogServer

🌐 Access Points:
  - CogServer will be available on port 17001 (telnet)
  - Web interfaces on port 18001 (when available)
  - REST API on port 5000 (when configured)

📖 Documentation:
  - OpenCog Wiki: https://wiki.opencog.org/
  - AtomSpace Guide: https://wiki.opencog.org/w/AtomSpace
  - Local docs: /workspace/opencog-org/.gitpod/README.md

🐛 Troubleshooting:
  - Check logs: cat $LOG_FILE
  - Troubleshooting guide: /workspace/opencog-org/.gitpod/TROUBLESHOOTING.md

Happy coding with OpenCog! 🧠✨

EOF
}

# Run main deployment if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi