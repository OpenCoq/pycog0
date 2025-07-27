# GitPod Development Environment for OpenCog

This document describes the GitPod configuration for the OpenCog organization repository, providing a complete cloud-based development environment.

## 🚀 Quick Start

[![Open in GitPod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/OpenCoq/opencog-org)

1. **Click the GitPod button** above to launch the environment
2. **Wait for initialization** (2-3 minutes for first launch)
3. **Run the setup script**: `./setup-opencog-gitpod.sh`
4. **Start building**: Use `build-opencog` to build the complete ecosystem

## 📋 What's Included

### Base Environment
- **Python 3.10** development environment (as specified in issue #7)
- **Ubuntu-based** GitPod workspace with all development tools
- **Guix Package Manager** for reproducible builds
- **Git** pre-configured for OpenCog development

### OpenCog Dependencies
- **Build Tools**: GCC, CMake, Make, pkg-config
- **Libraries**: Boost, CppUnit, GSL, CBLAS, LAPACK
- **Language Support**: Guile, Python, Cython
- **System Libraries**: ZMQ, BZ2, Expat, Hyphen

### Development Tools
- **VS Code Extensions**: C/C++, CMake, Python, Nix
- **Pre-configured Ports**: CogServer (17001), REST API (5000), Web UI (8080)
- **Build Scripts**: Automated build for AtomSpace, CogServer, CogUtil
- **Aliases**: Quick navigation and common commands

## 🏗️ Build Process

The GitPod environment supports building the complete OpenCog ecosystem:

### Automated Build (Recommended)
```bash
./setup-opencog-gitpod.sh  # Complete environment setup
build-opencog              # Build everything (10-15 minutes)
```

### Manual Build Steps
```bash
# 1. Build CogUtil (foundation)
build-cogutil

# 2. Build AtomSpace (core)
build-atomspace

# 3. Build CogServer (runtime)
build-cogserver
```

### Individual Component Building
```bash
# AtomSpace only
cd atomspace && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j$(nproc)

# CogServer only  
cd cogserver && mkdir build && cd build
cmake .. -DCMAKE_INSTALL_PREFIX=$HOME/.local
make -j$(nproc)
```

## 🎮 Running Demos

After building, various OpenCog demos are available:

### 1. AtomSpace Python API
```python
python3 -c "
from opencog.atomspace import *
a = AtomSpace()
print('AtomSpace created:', a)
"
```

### 2. Guile Scheme Interface
```bash
cd atomspace/build
./opencog/guile/opencog-guile
```

### 3. CogServer Interactive Session
```bash
cd cogserver/build
./opencog/cogserver/server/cogserver
# In another terminal: telnet localhost 17001
```

### 4. Language Learning Components
```bash
cd learn
python3 -c "import opencog; print('Learning modules available')"
```

## ⚙️ Configuration Details

### GitPod Workspace Configuration (`.gitpod.yml`)
- **Image**: Custom Dockerfile with Python 3.10 and Guix
- **Ports**: 17001 (CogServer), 5000 (REST API), 8080 (Web UI)
- **Tasks**: Automated environment initialization
- **Extensions**: C++, Python, CMake, Nix support

### Docker Environment (`.gitpod.Dockerfile`)
- **Base**: `gitpod/workspace-python-3.10:2025-07-23-06-50-33`
- **Package Manager**: Guix for reproducible builds
- **Dependencies**: Complete OpenCog build toolchain
- **User Setup**: Preconfigured gitpod user with development tools

### Environment Variables
```bash
OPENCOG_WORKSPACE="/workspace/opencog-org"
OPENCOG_BUILD_DIR="$HOME/opencog-build"
PATH="$HOME/.local/bin:$PATH"
GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"
```

### Helpful Aliases
- `og` - Navigate to OpenCog workspace
- `ogb` - Navigate to build directory
- `build-all` - Build complete OpenCog ecosystem
- `demos` - Show available demos

## 🔧 Troubleshooting

### Common Issues

**Guix installation fails:**
```bash
# Fallback to system packages
sudo apt-get update
sudo apt-get install build-essential cmake libboost-all-dev
```

**Build errors:**
```bash
# Clean build directory
rm -rf build/
mkdir build && cd build
cmake .. && make clean && make
```

**Permission errors:**
```bash
# Fix file permissions
chmod +x setup-opencog-gitpod.sh
sudo chown -R gitpod:gitpod $HOME/.local
```

**Port access issues:**
```bash
# Check if services are running
netstat -tlnp | grep :17001
# Restart GitPod workspace if needed
```

### Performance Tips

1. **Use prebuilt images**: GitPod caches Docker layers for faster startup
2. **Parallel builds**: Use `make -j$(nproc)` for faster compilation
3. **Incremental builds**: Only rebuild changed components
4. **Resource monitoring**: Use `htop` to monitor CPU/memory usage

## 📊 Component Overview

### Core Components (Built in order)
1. **CogUtil** - Utility libraries and logging
2. **AtomSpace** - Core knowledge representation
3. **CogServer** - Network server and shell interface

### Optional Components
- **Language Learning** - Natural language processing
- **PLN** - Probabilistic Logic Networks
- **Pattern Miner** - Pattern discovery algorithms
- **MOSES** - Meta-optimizing semantic evolutionary search

### External Integrations
- **Guile Scheme** - Functional programming interface
- **Python Bindings** - Python API for AtomSpace
- **REST API** - HTTP interface for web applications
- **Docker** - Containerized deployment options

## 🌐 Network Architecture

```
GitPod Workspace
├── Port 17001: CogServer Telnet Interface
├── Port 18001: CogServer Web Interface  
├── Port 5000:  REST API Server
├── Port 8080:  Web Demonstrations
└── Internal:   Inter-component communication
```

## 📈 Development Workflow

### 1. Initial Setup
```bash
# Launch GitPod environment
# Run: ./setup-opencog-gitpod.sh
# Wait for Guix package installation
```

### 2. Development Cycle
```bash
# Edit source code using VS Code
# Build specific components: build-atomspace
# Test changes: run unit tests
# Commit changes using Git
```

### 3. Testing & Validation
```bash
# Run component tests
cd atomspace/build && make test

# Integration testing
run-opencog-demos

# Performance testing
cd benchmark && ./run-benchmarks.sh
```

## 🤝 Contributing

When developing in the GitPod environment:

1. **Follow OpenCog coding standards**
2. **Run tests before committing**: `make test`
3. **Update documentation** for significant changes
4. **Use descriptive commit messages**
5. **Test in clean environment** using GitPod

## 📚 Additional Resources

- **OpenCog Wiki**: https://wiki.opencog.org/
- **AtomSpace Documentation**: https://wiki.opencog.org/w/AtomSpace
- **GitPod Documentation**: https://www.gitpod.io/docs/
- **Guix Manual**: https://guix.gnu.org/manual/
- **OpenCog GitHub**: https://github.com/opencog

## 🏷️ Version Information

- **GitPod Base Image**: `gitpod/workspace-python-3.10:2025-07-23-06-50-33`
- **Python Version**: 3.10.x
- **Guix**: Latest stable release
- **OpenCog**: Development branches from opencog-org repository

---

*This GitPod configuration implements the requirements from issue #7, providing a complete automated Guix build environment for OpenCog development.*