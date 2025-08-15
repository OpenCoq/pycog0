# OpenCog Organization Repository - Development Instructions

**ALWAYS follow these instructions first and only fallback to additional search and context gathering if the information below is incomplete or found to be in error.**

This repository contains the complete OpenCog ecosystem with 52+ components across 9 dependency layers, providing unified CMake build system, GitPod cloud development, and comprehensive automation tools.

## Working Effectively

### Bootstrap Environment and Dependencies
- **Install core dependencies**: `sudo apt-get update && sudo apt-get install -y libboost-all-dev cxxtest doxygen binutils-dev libiberty-dev guile-3.0-dev pkg-config libzmq3-dev uuid-dev build-essential`
- **Run adaptive boost installer**: `./scripts/adaptive-boost-install.sh` -- takes 10-15 seconds. NEVER CANCEL.
- **Update library cache**: `sudo ldconfig`

### Build System (CMake Unified Build)
- **Configure build system**: 
  ```bash
  mkdir -p /tmp/opencog-build && cd /tmp/opencog-build
  cmake /path/to/repository
  ```
  -- takes 1-2 seconds, discovers all 52 components automatically. NEVER CANCEL.

- **List available components**: `make list-components` -- shows individual components and layer targets
- **Build foundation layer**: `make Foundation-layer` -- builds cogutil, moses, blender_api_msgs, external-tools, ocpkg
- **Build individual components**: `make cogutil` -- takes 20-25 seconds with full dependencies. NEVER CANCEL.
- **Build atomspace**: `make atomspace` -- takes 4-5 minutes. NEVER CANCEL. Set timeout to 10+ minutes.
- **Install component**: `cd <component>-build && sudo make install && sudo ldconfig`

### GitPod Cloud Development
- **Use GitPod environment**: Click GitPod button in README.md for zero-configuration development
- **GitPod setup script**: `./setup-opencog-gitpod.sh` -- installs Guix packages and creates build scripts
- **Check GitPod status**: `opencog_status` -- verifies environment components
- **Build in GitPod**: `build-opencog` -- builds complete ecosystem using Guix packages

### Python Environment and Validation
- **Python version**: 3.12.3 (verified working)
- **Validate build system**: `python3 scripts/validate_dependency_build.py` -- runs in 1-2 seconds
- **Test automation**: `python3 scripts/auto_fix.py` -- automated dependency fixes

## Validation Scenarios

### CRITICAL: Always Test These Scenarios After Changes
1. **CMake Configuration Test**:
   ```bash
   mkdir -p /tmp/test-build && cd /tmp/test-build
   cmake /home/runner/work/pycog0/pycog0
   make list-components  # Should show 52 components
   ```

2. **Individual Component Build Test**:
   ```bash
   # Install dependencies first
   sudo apt-get install -y libboost-all-dev guile-3.0-dev
   
   # Build and install cogutil (required for other components)
   make cogutil  # ~18-22 seconds - NEVER CANCEL, set 60+ second timeout
   
   # Verify build succeeded (should show libcogutil.so)
   find . -name "libcogutil.so" && ldd ./cogutil-build/opencog/util/libcogutil.so | head -3
   
   # Install cogutil for dependency chain testing
   cd cogutil-build && sudo make install && sudo ldconfig
   
   # Build atomspace (validates dependency chain)
   cd /tmp/test-build && make atomspace  # ~4.5 minutes - NEVER CANCEL, set 10+ minute timeout
   ```

3. **Automation Script Test**:
   ```bash
   ./scripts/adaptive-boost-install.sh  # ~11 seconds - NEVER CANCEL
   python3 scripts/validate_dependency_build.py  # Should pass all tests
   ```

4. **GitPod Environment Test** (if in GitPod):
   ```bash
   opencog_status  # Check environment
   build-cogutil && test-cogutil  # Build and test components
   ```

### Complete End-to-End Validation Workflow
```bash
# 1. Clean environment setup
mkdir -p /tmp/validation && cd /tmp/validation

# 2. Install dependencies (30-60 seconds)
sudo apt-get update && sudo apt-get install -y libboost-all-dev guile-3.0-dev cxxtest

# 3. Configure build system (1-2 seconds)
cmake /home/runner/work/pycog0/pycog0

# 4. Verify component discovery (should list 52 components)
make list-components

# 5. Build foundation component (18-22 seconds)
make cogutil

# 6. Verify Python validation (1-2 seconds)
cd /home/runner/work/pycog0/pycog0 && python3 scripts/validate_dependency_build.py

# This workflow validates the core functionality described in these instructions
```

## Build Timing Expectations

### NEVER CANCEL - Critical Timing Information
- **CMake configuration**: 1-2 seconds - NEVER CANCEL, set 60+ second timeout
- **Cogutil build**: 20-25 seconds - NEVER CANCEL, set 60+ second timeout  
- **Atomspace build**: 4-5 minutes - NEVER CANCEL, set 10+ minute timeout
- **Foundation layer**: 1-2 minutes - NEVER CANCEL, set 5+ minute timeout
- **Complete ecosystem**: 15-30 minutes - NEVER CANCEL, set 60+ minute timeout
- **Dependency installation**: 30-60 seconds - NEVER CANCEL, set 5+ minute timeout

### Build Commands with Timeouts
Always use these exact timeout values based on comprehensive validation:
```bash
# CMake configuration (use 300 seconds timeout minimum)
timeout 300 cmake /path/to/repository

# Individual components (use 1800 seconds timeout minimum)  
timeout 1800 make cogutil         # Actually ~18-22 seconds, but use conservative timeout
timeout 3600 make atomspace       # Actually ~4.5 minutes, but builds can vary

# Complete builds (use 3600 seconds timeout minimum)
timeout 3600 make all-components
timeout 3600 make Foundation-layer
```

### Validated Performance Benchmarks
Based on comprehensive testing on Ubuntu 24.04 with standard GitHub Actions runners:
- **CMake Configuration**: 0.3-1.75 seconds (52 components discovered)
- **Cogutil Build**: 18-22 seconds (with Boost, Doxygen, full dependencies)
- **Atomspace Build**: 4 minutes 32 seconds (complex component with Guile bindings)
- **Foundation Layer**: 1-2 minutes (parallel builds of 5 components)
- **Dependency Installation**: 10-60 seconds (apt-get with boost, guile, etc.)
- **Python Validation**: 1-2 seconds (validate_dependency_build.py)

## Repository Structure and Navigation

### Key Directories
- **`.github/`**: GitHub workflows and this instructions file
- **`.gitpod/`**: GitPod configuration and deployment scripts
- **`scripts/`**: Automation tools and validation scripts
- **`docs/`**: Comprehensive documentation (BUILD_PIPELINE.md, QUICK_START_GUIDE.md)
- **Component directories**: 52 individual OpenCog components organized by layers

### Component Dependency Layers
1. **Foundation Layer**: cogutil, moses, blender_api_msgs, external-tools, ocpkg
2. **Core Layer**: atomspace, atomspace-rocks, atomspace-pgres, atomspace-ipfs, etc.
3. **Logic Layer**: unify, ure
4. **Cognitive Layer**: cogserver, attention, spacetime, pattern-index, etc.
5. **Advanced Layer**: pln, miner, asmoses, benchmark
6. **Learning Layer**: learn, generate, language-learning
7. **Language Layer**: lg-atomese, relex, link-grammar
8. **Robotics Layer**: vision, perception, sensory, ros-behavior-scripting, etc.
9. **Integration Layer**: opencog, TinyCog

### Navigation Commands
- **Go to workspace**: `cd /workspace/opencog-org` (in GitPod) or repository root
- **List all components**: `ls -la` shows 95+ directories
- **Find component**: `find . -name "*atomspace*" -type d` 
- **Component documentation**: `cat <component>/README.md`

## Common Development Tasks

### Before Making Changes
- **Always run**: `python3 scripts/validate_dependency_build.py` -- ensures build system integrity
- **Check component status**: `make list-components` -- verify all components are available
- **Install test dependencies**: `sudo apt-get install -y libboost-all-dev guile-3.0-dev cxxtest`

### After Making Changes  
- **Validate changes**: Re-run all validation scenarios above
- **Test component builds**: Build affected components individually to verify no breakage
- **Run validation script**: `python3 scripts/validate_dependency_build.py` -- must pass all tests
- **GitPod test**: If available, test in GitPod environment with `opencog_status && build-opencog`

### Common Build Fixes
- **Missing Boost**: `./scripts/adaptive-boost-install.sh`
- **Missing Guile**: `sudo apt-get install -y guile-3.0-dev`
- **Library not found**: `sudo ldconfig`
- **Component dependency missing**: Install prerequisite component first (e.g., cogutil before atomspace)

## GitHub Actions and CI

### Workflow System
- **Dependency build matrix**: `.github/workflows/opencog-dependency-build.yml` -- builds all components
- **Manual trigger**: Go to Actions tab, select workflow, click "Run workflow"
- **Build categories**: foundation, core, logic, cognitive, advanced, learning, language, robotics, integration
- **Validation**: All workflows use same CMake system validated above

### Workflow Timing Expectations
- **Individual components**: 5-15 minutes each - NEVER CANCEL
- **Layer builds**: 20-45 minutes - NEVER CANCEL  
- **Complete matrix**: 60-120 minutes - NEVER CANCEL

## Critical Configuration Files

### Build System Files (Frequently Updated)
- **`CMakeLists.txt`**: Main unified build configuration
- **`CMAKE_BUILD_GUIDE.md`**: Detailed build system documentation  
- **`scripts/validate_dependency_build.py`**: Build validation (always run after changes)

### GitPod Files (Environment Setup)
- **`.gitpod.yml`**: GitPod configuration with port forwarding
- **`.gitpod.Dockerfile`**: Environment image definition
- **`setup-opencog-gitpod.sh`**: Automated Guix-based setup

### Documentation Files (Reference These)
- **`.gitpod/README.md`**: Complete GitPod deployment guide
- **`docs/QUICK_START_GUIDE.md`**: Manual workflow execution
- **`README.md`**: Repository overview and quick start

## Environment Variables and Paths

### GitPod Environment
- **`OPENCOG_WORKSPACE`**: `/workspace/opencog-org`
- **`OPENCOG_BUILD_DIR`**: `$HOME/opencog-build`  
- **`GUIX_LOCPATH`**: `$HOME/.guix-profile/lib/locale`
- **`PATH`**: Includes `/var/guix/profiles/per-user/gitpod/current-guix/bin`

### Local Development
- **Build directory**: Use `/tmp/opencog-build` for testing
- **Install prefix**: Components install to `/usr/local/` by default
- **Library path**: Run `sudo ldconfig` after component installation

## Troubleshooting Common Issues

### Build Failures
- **"CogUtil missing"**: Install cogutil first: `make cogutil && cd cogutil-build && sudo make install`
- **"Guile was not found"**: `sudo apt-get install -y guile-3.0-dev`  
- **"Boost not found"**: `./scripts/adaptive-boost-install.sh` or `sudo apt-get install -y libboost-all-dev`
- **Library linking errors**: `sudo ldconfig`

### GitPod Issues
- **"command not found"**: `./fix-cogserver-scripts.sh && source ~/.bashrc`
- **Build fails**: `build-opencog` then check `/tmp/opencog-deploy.log`
- **Services not starting**: Check port forwarding in GitPod interface (17001, 18001, 5000, 8080)

### Performance Issues
- **Slow builds**: Use `make -j$(nproc)` for parallel compilation
- **Memory issues**: Build components individually rather than `make all-components`
- **GitPod timeout**: Builds may take 15-30 minutes, never cancel early

## Language-Specific Notes

### C++ Development
- **Compiler**: GCC 13.3.0 (verified working)
- **Standard**: C++17 used throughout codebase
- **Dependencies**: Boost 1.83.0, Guile 3.0.9 for bindings
- **Build flags**: Release mode by default, debug with `-DCMAKE_BUILD_TYPE=Debug`

### Python Development
- **Version**: Python 3.12.3 (verified working)
- **Key scripts**: All in `scripts/` directory
- **Validation**: Always run `python3 scripts/validate_dependency_build.py`
- **Bindings**: Available in components like atomspace (requires Cython)

### Scheme/Guile Development  
- **Version**: Guile 3.0.9 (verified working)
- **Integration**: Deep integration with atomspace and cognitive components
- **REPL**: Available via `start-atomspace-repl` in GitPod
- **Load path**: Components install Scheme modules to system paths

Remember: These instructions are based on comprehensive validation. Follow them exactly and use the specified timeouts to avoid premature cancellation of builds.