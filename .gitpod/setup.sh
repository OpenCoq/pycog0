#!/bin/bash

# OpenCog Gitpod Environment Setup Script
# Handles initial environment configuration and preparation

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

log_info() {
    echo -e "${BLUE}ℹ️  $1${NC}"
}

log_success() {
    echo -e "${GREEN}✅ $1${NC}"
}

log_warning() {
    echo -e "${YELLOW}⚠️  $1${NC}"
}

log_error() {
    echo -e "${RED}❌ $1${NC}"
}

# Check if running in Gitpod
check_gitpod_environment() {
    if [ -n "$GITPOD_WORKSPACE_ID" ]; then
        log_success "Running in Gitpod environment"
        log_info "Workspace ID: $GITPOD_WORKSPACE_ID"
        return 0
    else
        log_warning "Not running in Gitpod - some features may not work as expected"
        return 1
    fi
}

# Set up environment variables for OpenCog development
setup_environment_variables() {
    log_info "Setting up OpenCog environment variables..."
    
    # Core paths
    export OPENCOG_WORKSPACE="/workspace/opencog-org"
    export OPENCOG_BUILD_DIR="$HOME/opencog-build"
    export PATH="$HOME/.local/bin:$PATH"
    
    # Guix environment
    export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"
    export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
    export GUIX_PROFILE="$HOME/.guix-profile"
    
    # OpenCog specific
    export OPENCOG_GITPOD_OPTIMIZED="true"
    export CMAKE_BUILD_TYPE="Release"
    
    # Python paths for OpenCog bindings
    export PYTHONPATH="$OPENCOG_WORKSPACE:$PYTHONPATH"
    export GUILE_LOAD_PATH="$OPENCOG_WORKSPACE:$GUILE_LOAD_PATH"
    
    log_success "Environment variables configured"
}

# Create necessary directories
setup_directories() {
    log_info "Creating OpenCog workspace directories..."
    
    local directories=(
        "$HOME/.local/bin"
        "$HOME/.local/lib"
        "$HOME/.local/include"
        "$OPENCOG_BUILD_DIR"
        "$HOME/opencog-workspace"
    )
    
    for dir in "${directories[@]}"; do
        if mkdir -p "$dir"; then
            log_success "Created directory: $dir"
        else
            log_warning "Failed to create directory: $dir"
        fi
    done
}

# Install essential system packages
install_system_packages() {
    log_info "Installing essential system packages..."
    
    # Update package list
    if sudo apt-get update >/dev/null 2>&1; then
        log_success "Package list updated"
    else
        log_warning "Failed to update package list"
    fi
    
    # Essential packages for OpenCog development
    local packages=(
        "build-essential"
        "cmake"
        "git"
        "wget"
        "curl"
        "vim"
        "nano"
        "tree"
        "htop"
        "unzip"
        "locales"
    )
    
    for package in "${packages[@]}"; do
        if sudo apt-get install -y "$package" >/dev/null 2>&1; then
            log_success "Installed: $package"
        else
            log_warning "Failed to install: $package"
        fi
    done
}

# Configure Git for development
setup_git_configuration() {
    log_info "Setting up Git configuration for development..."
    
    # Set default editor to VS Code (available in Gitpod)
    git config --global core.editor "code --wait" 2>/dev/null || log_warning "Could not set VS Code as Git editor"
    
    # Set default branch name
    git config --global init.defaultBranch main 2>/dev/null || log_warning "Could not set default branch name"
    
    # Set up useful aliases
    git config --global alias.st status 2>/dev/null
    git config --global alias.co checkout 2>/dev/null
    git config --global alias.br branch 2>/dev/null
    git config --global alias.ci commit 2>/dev/null
    git config --global alias.lg "log --oneline --graph --decorate" 2>/dev/null
    
    log_success "Git configuration completed"
}

# Set up helpful bash aliases and functions
setup_bash_environment() {
    log_info "Setting up bash environment with OpenCog aliases..."
    
    # Create bashrc additions
    cat >> ~/.bashrc << 'EOF'

# OpenCog Gitpod Environment Configuration
# ========================================

# Environment variables
export OPENCOG_WORKSPACE="/workspace/opencog-org"
export OPENCOG_BUILD_DIR="$HOME/opencog-build"
export PATH="$HOME/.local/bin:$PATH"
export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"
export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
export GUIX_PROFILE="$HOME/.guix-profile"
export PYTHONPATH="$OPENCOG_WORKSPACE:$PYTHONPATH"
export GUILE_LOAD_PATH="$OPENCOG_WORKSPACE:$GUILE_LOAD_PATH"

# OpenCog navigation aliases
alias og="cd /workspace/opencog-org"
alias ogb="cd $HOME/opencog-build"
alias ogl="cd /workspace/opencog-org && ls -la"

# Build aliases
alias build-all="build-opencog"
alias rebuild="cd /workspace/opencog-org && rm -rf */build && build-opencog"

# Service aliases
alias cog="start-cogserver"
alias repl="start-atomspace-repl"
alias demos="run-opencog-demos"

# Development utilities
alias ll="ls -la"
alias la="ls -A"
alias l="ls -CF"
alias ..="cd .."
alias ...="cd ../.."
alias grep="grep --color=auto"
alias fgrep="fgrep --color=auto"
alias egrep="egrep --color=auto"

# OpenCog specific functions
opencog_status() {
    echo "🧠 OpenCog Gitpod Environment Status"
    echo "===================================="
    echo "Workspace: $OPENCOG_WORKSPACE"
    echo "Build Dir: $OPENCOG_BUILD_DIR"
    echo "Guix Profile: $GUIX_PROFILE"
    echo ""
    echo "Available Components:"
    for component in cogutil atomspace atomspace-storage cogserver; do
        if [ -d "$OPENCOG_WORKSPACE/$component" ]; then
            echo "  ✅ $component"
        else
            echo "  ❌ $component (not found)"
        fi
    done
    echo ""
    echo "Build Status:"
    for component in cogutil atomspace atomspace-storage cogserver; do
        if [ -d "$OPENCOG_WORKSPACE/$component/build" ] && [ "$(ls -A "$OPENCOG_WORKSPACE/$component/build" 2>/dev/null)" ]; then
            echo "  🔧 $component (built)"
        else
            echo "  ⏳ $component (not built)"
        fi
    done
}

# Welcome message
opencog_welcome() {
    echo ""
    echo "🧠 Welcome to OpenCog Gitpod Environment!"
    echo "========================================"
    echo ""
    echo "Quick commands:"
    echo "  og              - Go to OpenCog workspace"
    echo "  build-opencog   - Build complete OpenCog ecosystem"
    echo "  opencog_status  - Show environment status"
    echo "  demos          - Show available demos"
    echo ""
    echo "For help: cat /workspace/opencog-org/.gitpod/README.md"
    echo ""
}

EOF

    log_success "Bash environment configured"
}

# Create build helper scripts
create_build_scripts() {
    log_info "Creating OpenCog build helper scripts..."
    
    local bin_dir="$HOME/.local/bin"
    
    # Generic component build script
    cat > "$bin_dir/build-component" << 'EOF'
#!/bin/bash
# Generic OpenCog component builder

if [ $# -eq 0 ]; then
    echo "Usage: build-component <component-name>"
    echo "Available components: cogutil, atomspace, atomspace-storage, cogserver"
    exit 1
fi

COMPONENT=$1
COMPONENT_DIR="/workspace/opencog-org/$COMPONENT"

if [ ! -d "$COMPONENT_DIR" ]; then
    echo "❌ Component '$COMPONENT' not found in $COMPONENT_DIR"
    exit 1
fi

echo "🔧 Building $COMPONENT..."
cd "$COMPONENT_DIR"
mkdir -p build
cd build

if cmake .. -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="$HOME/.local"; then
    if make -j2; then
        echo "✅ $COMPONENT built successfully"
        make install 2>/dev/null || echo "⚠️ Install step had warnings (this may be normal)"
    else
        echo "❌ $COMPONENT build failed"
        exit 1
    fi
else
    echo "❌ $COMPONENT configuration failed"
    exit 1
fi
EOF
    chmod +x "$bin_dir/build-component"
    
    # Individual component build scripts
    for component in cogutil atomspace atomspace-storage cogserver; do
        cat > "$bin_dir/build-$component" << EOF
#!/bin/bash
echo "🔧 Building $component..."
build-component $component
EOF
        chmod +x "$bin_dir/build-$component"
    done
    
    # Complete build script
    cat > "$bin_dir/build-opencog" << 'EOF'
#!/bin/bash
echo "🏗️ Building complete OpenCog ecosystem..."
echo "This will build components in dependency order."
echo ""

COMPONENTS=("cogutil" "atomspace" "atomspace-storage" "cogserver")
FAILED_COMPONENTS=()

for component in "${COMPONENTS[@]}"; do
    echo "Building $component..."
    if build-component "$component"; then
        echo "✅ $component completed"
    else
        echo "❌ $component failed"
        FAILED_COMPONENTS+=("$component")
    fi
    echo ""
done

echo "🏁 Build Summary:"
echo "================"
if [ ${#FAILED_COMPONENTS[@]} -eq 0 ]; then
    echo "✅ All components built successfully!"
    echo ""
    echo "Next steps:"
    echo "  - Start CogServer: start-cogserver"
    echo "  - Launch AtomSpace REPL: start-atomspace-repl"
    echo "  - View demos: demos"
else
    echo "❌ Failed components: ${FAILED_COMPONENTS[*]}"
    echo "Check logs and try building individual components."
fi
EOF
    chmod +x "$bin_dir/build-opencog"
    
    log_success "Build scripts created"
}

# Main setup function
main() {
    log_info "Starting OpenCog Gitpod environment setup..."
    echo ""
    
    # Run setup steps
    check_gitpod_environment
    setup_environment_variables
    setup_directories
    install_system_packages
    setup_git_configuration
    setup_bash_environment
    create_build_scripts
    
    echo ""
    log_success "OpenCog Gitpod environment setup completed!"
    echo ""
    echo "🎯 Next steps:"
    echo "  1. Source the new environment: source ~/.bashrc"
    echo "  2. Check status: opencog_status" 
    echo "  3. Build OpenCog: build-opencog"
    echo "  4. Explore: og && ls"
    echo ""
}

# Run main function if script is executed directly
if [[ "${BASH_SOURCE[0]}" == "${0}" ]]; then
    main "$@"
fi