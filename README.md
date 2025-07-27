# OpenCog Organization Repository

This repository contains the OpenCog ecosystem components and comprehensive build automation.

## 🚀 Quick Start with GitPod

Launch a complete OpenCog development environment in your browser with one click:

[![Open in GitPod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/OpenCoq/opencog-org)

The GitPod environment includes:
- **Complete OpenCog ecosystem** with all dependencies pre-installed
- **Guix package manager** for reproducible builds  
- **Python 3.10** development environment
- **Pre-configured build scripts** for AtomSpace, CogServer, and more
- **VS Code extensions** for C++, Python, and Nix development
- **Port forwarding** for OpenCog services (CogServer, REST API, web demos)

📖 **[Complete GitPod Setup Guide](docs/GITPOD_SETUP.md)**

## 🏗️ OpenCog Dependency Build Matrix

We've implemented a comprehensive GitHub Actions workflow that builds and installs all OpenCog components according to dependency diagrams with matrix discovery of hidden dependencies.

### Key Features:
- **40+ Component Coverage**: Builds the entire OpenCog ecosystem
- **9 Dependency Layers**: Foundation → Core → Logic → Cognitive → Advanced → Learning → Language → Robotics → Integration  
- **Fail-Never Approach**: Discovers all possible build issues without stopping
- **Matrix Build Strategy**: Parallel builds within dependency categories
- **Hidden Dependency Discovery**: Systematic identification of undocumented dependencies
- **Comprehensive Reporting**: Automated issue creation and detailed build summaries

### Quick Start:
1. Go to [Actions tab](../../actions)
2. Select "OpenCog Dependency Build Matrix"
3. Click "Run workflow"
4. Choose build category (or "all" for complete build)

### Documentation:
- [Complete Documentation](docs/OPENCOG_DEPENDENCY_BUILD.md)
- [Quick Start Guide](docs/QUICK_START_GUIDE.md)
- [Dependency Diagrams](MERMAID_DIAGRAMS.md)

## Repository Structure

This repository contains OpenCog components organized by functional categories as defined in the dependency diagrams.
