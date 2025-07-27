# OpenCog Gitpod Deployment Guide

Welcome to the OpenCog Gitpod development environment! This guide provides comprehensive documentation for the one-click deployment solution.

## 🚀 Quick Start

### One-Click Launch
Click the "Open in Gitpod" button in the repository README to launch a complete OpenCog environment in your browser.

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/OpenCoq/opencog-org)

### What Happens Automatically
1. **Environment Initialization** - Gitpod creates a workspace with Python 3.10 and essential tools
2. **Guix Installation** - GNU Guix package manager is installed for reproducible builds
3. **Dependency Resolution** - All OpenCog dependencies are installed via optimized manifest
4. **Component Building** - Core OpenCog components (CogUtil, AtomSpace, CogServer) are built
5. **Service Configuration** - Ports are configured and forwarded for web access

## 📁 Deployment Architecture

### File Structure
```
.gitpod/
├── manifest.scm          # Gitpod-optimized Guix package manifest
├── deploy.sh             # Main deployment automation script
├── setup.sh              # Environment setup and configuration
├── README.md             # This comprehensive guide
└── TROUBLESHOOTING.md    # Detailed troubleshooting guide
```

### Integration Points
- **Root `.gitpod.yml`** - Main workspace configuration with automated tasks
- **Root `.gitpod.Dockerfile`** - Custom Docker image with Guix integration
- **Root `setup-opencog-gitpod.sh`** - Legacy script (maintained for compatibility)

## 🔧 Technical Implementation

### Deployment Process
The deployment follows this automated sequence:

1. **Environment Setup** (`.gitpod/setup.sh`)
   - Check Gitpod environment
   - Configure environment variables
   - Create workspace directories
   - Install system packages
   - Set up Git configuration
   - Configure bash environment

2. **Package Installation** (`.gitpod/deploy.sh`)
   - Install Guix packages from optimized manifest
   - Fallback to system packages if Guix fails
   - Configure build environment

3. **Component Building**
   - Build CogUtil (utility library)
   - Build AtomSpace (knowledge representation)
   - Build CogServer (reasoning server)

4. **Service Configuration**
   - Set up port forwarding
   - Create service startup scripts
   - Configure web interfaces

### Guix Integration

#### Optimized Manifest
The `.gitpod/manifest.scm` file contains a carefully curated list of packages optimized for cloud environments:

- **Core Build Tools**: gcc-toolchain, cmake, make, pkg-config
- **Essential Libraries**: boost, cppunit, guile-3.0, gsl
- **Python Environment**: python-3.10, python-cython, python-numpy
- **Development Tools**: git, binutils, doxygen

#### Fallback Strategy
If Guix installation fails (due to network issues or resource constraints), the system automatically falls back to:
- Ubuntu system packages via apt-get
- Pre-compiled binaries where available
- Minimal dependency installation for basic functionality

### Error Handling

The deployment system includes comprehensive error handling:

- **Graceful Degradation** - Continues deployment even if individual components fail
- **Detailed Logging** - All operations logged to `/tmp/opencog-deploy.log`
- **Status Reporting** - Clear feedback on deployment progress
- **Fallback Options** - Alternative installation methods for failed components

## 🌐 Service Access

### Port Configuration
The following ports are automatically configured and forwarded by Gitpod:

| Port  | Service                    | Access Method                |
|-------|---------------------------|------------------------------|
| 17001 | CogServer Telnet          | `telnet localhost 17001`     |
| 18001 | CogServer Web Interface   | Browser via Gitpod preview  |
| 5000  | REST API Server           | Browser via Gitpod preview  |
| 8080  | Web Demos & Visualizations| Browser via Gitpod preview  |

### Service Management
Convenient scripts are created for service management:

```bash
# Start services
start-cogserver          # Launch CogServer
start-atomspace-repl     # Launch AtomSpace Guile REPL

# Build components
build-opencog           # Build complete ecosystem
build-atomspace         # Build AtomSpace only
build-cogserver         # Build CogServer only
build-cogutil           # Build CogUtil only

# Development utilities
opencog_status          # Show environment status
og                      # Navigate to workspace
demos                   # Show available demos
```

## 📚 Development Workflow

### Initial Setup
After Gitpod launches:

1. **Wait for Initialization** - The deployment runs automatically
2. **Check Status** - Run `opencog_status` to verify components
3. **Build if Needed** - Run `build-opencog` if automatic build failed
4. **Explore** - Use `og && ls` to explore the codebase

### Development Cycle
1. **Edit Code** - Use VS Code interface to modify OpenCog components
2. **Build Changes** - Use `build-<component>` scripts to rebuild
3. **Test Changes** - Use service scripts to test functionality
4. **Debug** - Check logs at `/tmp/opencog-deploy.log`

### Available Components
The environment includes these OpenCog ecosystem components:

#### Core Infrastructure
- **CogUtil** - Utility libraries and basic data structures
- **AtomSpace** - Knowledge representation and storage
- **CogServer** - Network reasoning server

#### Language & Learning
- **Language Learning** - Natural language processing tools
- **Link Grammar** - Syntactic parsing system
- **PLN** - Probabilistic Logic Networks

#### Cognitive Architecture
- **Attention** - Attention allocation mechanisms
- **Ghost** - Goal-oriented chat system
- **Pattern Mining** - Pattern discovery algorithms

#### Integration
- **Python Bindings** - Python API for OpenCog
- **Guile Bindings** - Scheme/Guile integration
- **REST API** - HTTP interface for external integration

## 🧠 OpenCog Concepts

### AtomSpace Basics
The AtomSpace is OpenCog's knowledge representation system:

```scheme
; Create atoms (knowledge representations)
(ConceptNode "Dog")
(ConceptNode "Animal")

; Create relationships
(InheritanceLink
    (ConceptNode "Dog")
    (ConceptNode "Animal"))
```

### CogServer Usage
The CogServer provides a network interface:

```bash
# Connect via telnet
telnet localhost 17001

# In CogServer shell:
opencog> help
opencog> sql-open
opencog> (+ 2 3)
```

### Python Integration
```python
from opencog.atomspace import AtomSpace, types
from opencog.type_constructors import *

# Create AtomSpace instance
atomspace = AtomSpace()

# Add knowledge
dog = ConceptNode("Dog")
animal = ConceptNode("Animal")
inheritance = InheritanceLink(dog, animal)
```

## 🔄 Build System

### CMake Configuration
OpenCog uses CMake with these common options:

```bash
cmake .. \
    -DCMAKE_BUILD_TYPE=Release \
    -DCMAKE_INSTALL_PREFIX=$HOME/.local \
    -DCMAKE_CXX_FLAGS="-O2 -DNDEBUG"
```

### Dependency Management
Components must be built in order:
1. **CogUtil** - Base utilities (no dependencies)
2. **AtomSpace** - Depends on CogUtil
3. **CogServer** - Depends on AtomSpace

### Parallel Building
For Gitpod's resource constraints:
```bash
make -j2  # Use 2 parallel jobs
```

## 🐍 Python Environment

### Virtual Environment
The deployment creates a Python environment with:
- Python 3.10 (optimized for OpenCog)
- NumPy, SciPy (scientific computing)
- Cython (performance extensions)
- Jupyter (interactive development)

### OpenCog Python Modules
```python
# Core modules
from opencog.atomspace import AtomSpace
from opencog.type_constructors import *
from opencog.bindlink import execute_atom

# Utilities
from opencog.utilities import initialize_opencog
from opencog.logger import log
```

## 🔍 Debugging & Development

### Log Files
- **Deployment Log**: `/tmp/opencog-deploy.log`
- **Build Logs**: `<component>/build/CMakeFiles/CMakeError.log`
- **Runtime Logs**: Check component-specific documentation

### Common Issues
See the [TROUBLESHOOTING.md](TROUBLESHOOTING.md) file for detailed solutions to common issues.

### VS Code Integration
The environment includes helpful VS Code extensions:
- C/C++ Tools (Microsoft)
- CMake Tools
- Python support
- Nix/Guix integration
- Code spell checker

## 🌟 Advanced Features

### Custom Manifest
To modify the Guix package list, edit `.gitpod/manifest.scm`:

```scheme
(packages->manifest
  (list
    ;; Add your packages here
    gcc-toolchain
    cmake
    ;; ... existing packages
    your-custom-package))
```

### Environment Customization
Modify `.gitpod/setup.sh` to add custom environment setup:

```bash
# Add custom setup steps
setup_my_custom_tools() {
    log_info "Setting up custom tools..."
    # Your setup code here
}
```

### Port Configuration
Add custom ports in `.gitpod.yml`:

```yaml
ports:
  - port: 9090
    onOpen: open-preview
    description: My Custom Service
```

## 📖 Resources

### Documentation
- [OpenCog Wiki](https://wiki.opencog.org/)
- [AtomSpace Documentation](https://wiki.opencog.org/w/AtomSpace)
- [CogServer Guide](https://wiki.opencog.org/w/CogServer)
- [PLN Documentation](https://wiki.opencog.org/w/PLN)

### Community
- [OpenCog GitHub](https://github.com/opencog)
- [OpenCog Forums](https://groups.google.com/g/opencog)
- [OpenCog IRC](irc://irc.freenode.net/opencog)

### Development
- [Contribution Guidelines](https://github.com/opencog/atomspace/blob/master/CONTRIBUTING.md)
- [Coding Standards](https://wiki.opencog.org/w/Coding_standards)
- [Testing Guidelines](https://wiki.opencog.org/w/Unit_testing)

## 🤝 Contributing

### Development Workflow
1. Fork the repository
2. Create a feature branch
3. Make changes in Gitpod environment
4. Test changes using provided scripts
5. Submit pull request

### Testing Changes
```bash
# Test individual components
build-cogutil && test-cogutil
build-atomspace && test-atomspace

# Test complete system
build-opencog && demos
```

### Code Style
Follow OpenCog coding standards:
- C++: Follow existing style conventions
- Python: PEP 8 compliance
- Scheme: Standard Lisp formatting
- CMake: Consistent indentation and naming

---

Happy coding with OpenCog in Gitpod! 🧠✨

For issues or questions, please refer to the [TROUBLESHOOTING.md](TROUBLESHOOTING.md) guide or open an issue in the repository.