# OpenCog Gitpod Troubleshooting Guide

This guide helps resolve common issues encountered when using the OpenCog Gitpod environment.

## 🚨 Common Issues & Solutions

### 1. Deployment Issues

#### Guix Installation Fails
**Symptoms:**
- Error during Guix installation
- Timeout during package installation
- "Permission denied" errors

**Solutions:**
```bash
# Check if Guix is actually installed
which guix

# If not found, try manual installation
cd /tmp
wget https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh
chmod +x guix-install.sh
yes "" | sudo ./guix-install.sh

# Update PATH
export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"
```

#### Manifest Installation Timeout
**Symptoms:**
- Guix manifest installation takes too long
- Process appears stuck

**Solutions:**
```bash
# Use fallback installation
sudo apt-get update
sudo apt-get install -y build-essential cmake libboost-all-dev \
    libcppunit-dev libguile-3.0-dev python3-dev python3-pip

# Install Python packages
pip3 install --user numpy scipy cython nose
```

#### Network Connectivity Issues
**Symptoms:**
- Download failures
- DNS resolution errors
- Certificate errors

**Solutions:**
```bash
# Test connectivity
ping 8.8.8.8
curl -I https://google.com

# If issues persist, use offline mode
export GUIX_BUILD_OPTIONS="--substitute-urls=''"
```

### 2. Build Issues

#### CMake Configuration Fails
**Symptoms:**
- "CMake Error" messages
- Missing dependency errors
- Configuration failures

**Solutions:**
```bash
# Clean build directory
cd /workspace/opencog-org/atomspace
rm -rf build
mkdir build && cd build

# Check dependencies
pkg-config --list-all | grep -E "(boost|guile|gsl)"

# Try basic configuration
cmake .. -DCMAKE_BUILD_TYPE=Debug

# If still failing, install missing dependencies
sudo apt-get install -y libboost-all-dev libguile-3.0-dev libgsl-dev
```

#### Build Errors
**Symptoms:**
- Compilation errors
- Linker errors
- "make" failures

**Solutions:**
```bash
# Build with verbose output
make VERBOSE=1

# Build single-threaded to see errors clearly
make -j1

# Check for missing includes
find /usr/include -name "*.h" | grep -E "(boost|guile)"

# Clean and rebuild
make clean && make
```

#### Out of Memory During Build
**Symptoms:**
- Build process killed
- "Killed" messages
- Gitpod workspace crashes

**Solutions:**
```bash
# Use single-threaded build
make -j1

# Build components separately
build-cogutil
build-atomspace  
build-cogserver

# Free memory before building
sudo apt-get clean
rm -rf /tmp/*
```

### 3. Runtime Issues

#### CogServer Won't Start
**Symptoms:**
- "Connection refused" on port 17001
- CogServer binary not found
- Segmentation faults

**Solutions:**
```bash
# Check if binary exists
ls -la /workspace/opencog-org/cogserver/build/opencog/cogserver/server/

# Check dependencies
ldd /workspace/opencog-org/cogserver/build/opencog/cogserver/server/cogserver

# Try direct execution
cd /workspace/opencog-org/cogserver/build
./opencog/cogserver/server/cogserver --verbose

# Check for port conflicts
netstat -tulpn | grep 17001
```

#### Python Bindings Not Working
**Symptoms:**
- "ImportError: No module named opencog"
- Python import failures
- Missing _opencog.so

**Solutions:**
```bash
# Check Python path
echo $PYTHONPATH

# Add OpenCog to Python path
export PYTHONPATH="/workspace/opencog-org:$PYTHONPATH"

# Check if bindings built
find /workspace/opencog-org -name "*opencog*.so"

# Rebuild Python bindings
cd /workspace/opencog-org/atomspace/build
make -j1 opencog_atom_types
```

#### Guile REPL Issues
**Symptoms:**
- Guile not found
- Scheme load path errors
- REPL crashes

**Solutions:**
```bash
# Check Guile installation
which guile
guile --version

# Set Guile load path
export GUILE_LOAD_PATH="/workspace/opencog-org:$GUILE_LOAD_PATH"

# Test basic Guile
guile -c "(display \"Hello from Guile\")"

# Check OpenCog Guile bindings
cd /workspace/opencog-org/atomspace/build
find . -name "*.scm" | head -5
```

### 4. Environment Issues

#### Gitpod Workspace Timeout
**Symptoms:**
- Workspace stops automatically
- Session expires
- Loss of work

**Solutions:**
```bash
# Keep workspace active
while true; do echo "staying alive"; sleep 300; done &

# Save work frequently
git add . && git commit -m "WIP: saving progress"

# Use Gitpod's workspace snapshot feature
# (available in Gitpod interface)
```

#### Port Access Issues
**Symptoms:**
- Cannot access services via browser
- "Connection refused" errors
- Port forwarding not working

**Solutions:**
```bash
# Check if services are running
netstat -tulpn | grep -E "(17001|18001|5000|8080)"

# Check Gitpod port configuration
gp ports list

# Open port manually
gp ports expose 17001

# Check firewall (if applicable)
sudo ufw status
```

#### File Permission Issues
**Symptoms:**
- "Permission denied" errors
- Cannot write to directories
- Build failures due to permissions

**Solutions:**
```bash
# Fix ownership
sudo chown -R gitpod:gitpod /workspace/opencog-org
sudo chown -R gitpod:gitpod $HOME/.local

# Fix permissions
chmod -R u+w /workspace/opencog-org
chmod +x /workspace/opencog-org/.gitpod/*.sh

# Check current permissions
ls -la /workspace/opencog-org/.gitpod/
```

### 5. Performance Issues

#### Slow Build Times
**Symptoms:**
- Builds take very long
- System appears unresponsive
- High CPU usage

**Solutions:**
```bash
# Monitor system resources
htop

# Reduce parallel jobs
make -j1

# Build only essential components
build-cogutil
build-atomspace
# Skip optional components initially

# Clean unnecessary files
sudo apt-get autoclean
rm -rf /workspace/opencog-org/*/build/CMakeFiles/*.dir/*.o
```

#### Memory Pressure
**Symptoms:**
- Slow performance
- Swap usage
- OOM (Out of Memory) errors

**Solutions:**
```bash
# Check memory usage
free -h

# Clear caches
sudo sync
echo 3 | sudo tee /proc/sys/vm/drop_caches

# Kill unnecessary processes
killall -9 chrome
killall -9 firefox

# Build sequentially
build-cogutil && build-atomspace && build-cogserver
```

### 6. Advanced Troubleshooting

#### Debug Logging
**Enable detailed logging:**
```bash
# Set debug environment
export OPENCOG_LOG_LEVEL=DEBUG
export CMAKE_VERBOSE_MAKEFILE=ON

# Run deployment with debug
bash -x .gitpod/deploy.sh

# Check all logs
cat /tmp/opencog-deploy.log
```

#### Container Inspection
**Check container environment:**
```bash
# Check container info
cat /etc/os-release
df -h
mount | grep -E "(guix|nix)"

# Check processes
ps aux | grep -E "(guix|opencog)"

# Check environment
env | grep -E "(GUIX|OPENCOG|PYTHON|GUILE)"
```

#### Dependency Analysis
**Analyze missing dependencies:**
```bash
# Check library dependencies
ldd /usr/bin/cmake
ldconfig -p | grep boost

# Check pkg-config
pkg-config --modversion guile-3.0
pkg-config --libs --cflags gsl

# Manual dependency check
apt list --installed | grep -E "(boost|guile|gsl)"
```

### 7. Recovery Procedures

#### Complete Environment Reset
**If all else fails:**
```bash
# Clean everything
rm -rf /workspace/opencog-org/*/build
rm -rf $HOME/.local/*
rm -rf $HOME/.guix-profile

# Restart deployment
source .gitpod/setup.sh
.gitpod/deploy.sh
```

#### Minimal Working Setup
**Get basic functionality:**
```bash
# Install only essentials
sudo apt-get install -y build-essential cmake git
sudo apt-get install -y libboost-dev libguile-3.0-dev

# Build minimal AtomSpace
cd /workspace/opencog-org/cogutil
mkdir build && cd build
cmake .. && make -j1

cd /workspace/opencog-org/atomspace  
mkdir build && cd build
cmake .. && make -j1
```

#### Backup & Restore
**Save your work:**
```bash
# Create backup
tar -czf ~/opencog-backup.tar.gz \
    /workspace/opencog-org \
    ~/.local \
    ~/.bashrc

# Restore from backup
cd /
tar -xzf ~/opencog-backup.tar.gz
```

## 🔍 Diagnostic Commands

### System Information
```bash
# System specs
cat /proc/cpuinfo | grep "model name" | head -1
cat /proc/meminfo | grep "MemTotal"
df -h /workspace

# Gitpod info
echo "Workspace ID: $GITPOD_WORKSPACE_ID"
echo "Workspace URL: $GITPOD_WORKSPACE_URL"
```

### OpenCog Status Check
```bash
# Check installation
opencog_status

# Check builds
for component in cogutil atomspace cogserver; do
    echo "=== $component ==="
    ls -la /workspace/opencog-org/$component/build/ 2>/dev/null || echo "Not built"
done

# Check services
ps aux | grep -E "(cogserver|guile)"
netstat -tulpn | grep -E "(17001|18001)"
```

### Build Diagnostics
```bash
# CMake cache
cat /workspace/opencog-org/atomspace/build/CMakeCache.txt | grep -E "(FOUND|ERROR)"

# Build logs
tail -50 /workspace/opencog-org/atomspace/build/CMakeFiles/CMakeError.log

# Linker issues
objdump -p /workspace/opencog-org/atomspace/build/opencog/guile/opencog-guile | grep NEEDED
```

## 📞 Getting Help

### Log Collection
**When reporting issues, include:**
```bash
# Collect diagnostic info
{
    echo "=== System Info ==="
    uname -a
    cat /etc/os-release
    
    echo "=== Environment ==="
    env | grep -E "(OPENCOG|GUIX|PYTHON|GUILE)"
    
    echo "=== Deployment Log ==="
    tail -100 /tmp/opencog-deploy.log
    
    echo "=== Build Status ==="
    opencog_status
} > ~/diagnostic-info.txt
```

### Community Support
- **GitHub Issues**: [OpenCog Issues](https://github.com/opencog/atomspace/issues)
- **Forums**: [OpenCog Google Group](https://groups.google.com/g/opencog)
- **Chat**: [OpenCog IRC](irc://irc.freenode.net/opencog)

### Documentation
- **OpenCog Wiki**: [https://wiki.opencog.org/](https://wiki.opencog.org/)
- **Gitpod Docs**: [https://www.gitpod.io/docs/](https://www.gitpod.io/docs/)
- **Guix Manual**: [https://guix.gnu.org/manual/](https://guix.gnu.org/manual/)

---

## 🛠️ Quick Reference

### Essential Commands
```bash
# Environment
source ~/.bashrc                    # Reload environment
opencog_status                     # Check status
og                                 # Go to workspace

# Building
build-opencog                      # Build everything
build-atomspace                    # Build AtomSpace
build-cogserver                   # Build CogServer

# Services
start-cogserver                    # Start CogServer
start-atomspace-repl              # Start Guile REPL
demos                             # Show demos

# Debugging
cat /tmp/opencog-deploy.log       # View deployment log
htop                              # Monitor resources
netstat -tulpn                    # Check ports
```

### File Locations
```
/workspace/opencog-org/           # Main workspace
/tmp/opencog-deploy.log          # Deployment log
~/.local/bin/                    # Custom scripts
~/.bashrc                        # Environment config
.gitpod/                         # Gitpod configuration
```

Remember: Most issues can be resolved by restarting the deployment process or using fallback installation methods. Don't hesitate to ask for help in the OpenCog community!