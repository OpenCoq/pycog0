# OpenCog Organization Repository

This repository contains the OpenCog ecosystem components and comprehensive build automation.

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
