# OpenCog Dependency Build Matrix

This document describes the comprehensive GitHub Actions workflow for building and installing all OpenCog components according to the dependency diagrams and critical path analysis.

## Overview

The OpenCog Dependency Build Matrix workflow implements a sophisticated build system that:

- **Groups components by category** according to critical path dependencies
- **Uses matrix strategy** for parallel builds within each category
- **Implements fail-never approach** to discover all possible build issues
- **Discovers hidden dependencies** through systematic build attempts
- **Provides comprehensive reporting** of build status and failures

## Workflow Structure

### Build Categories

The workflow organizes components into the following dependency layers:

#### 1. Foundation Layer
- `cogutil` - Core utilities and base classes
- `moses` - Machine learning algorithms
- `blender_api_msgs` - Blender integration messages
- `external-tools` - External tool integrations
- `ocpkg` - Package management utilities

#### 2. Core Layer (depends on Foundation)
- `atomspace` - Core knowledge representation
- `atomspace-rocks` - RocksDB persistence
- `atomspace-ipfs` - IPFS storage backend
- `atomspace-websockets` - WebSocket interface
- `atomspace-restful` - REST API interface
- `atomspace-bridge` - Bridge functionality
- `atomspace-metta` - MeTTa language support
- `atomspace-rpc` - RPC interface
- `atomspace-cog` - Cog integration
- `atomspace-agents` - Agent framework
- `atomspace-dht` - Distributed hash table

#### 3. Logic Layer (depends on Foundation + Core)
- `unify` - Unification algorithms
- `ure` - Unified Rule Engine

#### 4. Cognitive Layer (depends on Foundation + Core + Logic)
- `cogserver` - Cognitive server framework
- `attention` - Attention allocation mechanism
- `spacetime` - Spatial and temporal reasoning
- `pattern-index` - Pattern matching optimization
- `dimensional-embedding` - Dimensional analysis
- `profile` - Performance profiling

#### 5. Advanced Layer (depends on Foundation + Core + Logic + Cognitive)
- `pln` - Probabilistic Logic Networks
- `miner` - Pattern mining algorithms
- `asmoses` - Advanced MOSES implementation
- `benchmark` - Performance benchmarking

#### 6. Learning Layer (depends on Foundation + Core + Logic + Cognitive)
- `learn` - Machine learning framework
- `generate` - Content generation
- `language-learning` - Natural language learning

#### 7. Language Layer (depends on Foundation + Core)
- `lg-atomese` - Link Grammar integration
- `relex` - Relation extraction
- `link-grammar` - Link Grammar parser

#### 8. Robotics Layer (depends on Foundation + Core)
- `vision` - Computer vision processing
- `perception` - Perceptual processing
- `sensory` - Sensory data handling
- `ros-behavior-scripting` - ROS behavior scripts
- `robots_config` - Robot configuration
- `pau2motors` - Motor control interface

#### 9. Integration Layer (depends on most previous layers)
- `opencog` - Main OpenCog integration

## Features

### Matrix Build Strategy

Each category uses GitHub Actions matrix strategy to build multiple components in parallel:

```yaml
strategy:
  fail-fast: false
  matrix:
    component:
      - component1
      - component2
      - component3
```

### Fail-Never Approach

All jobs use `continue-on-error: true` to ensure that failures in one component don't prevent discovery of issues in other components.

### Hidden Dependency Discovery

The workflow systematically attempts to build each component and records failures, allowing discovery of:
- Missing system dependencies
- Undocumented inter-component dependencies
- Build order requirements
- Configuration incompatibilities

### Comprehensive Error Handling

- **Build artifacts** are collected for each component
- **Detailed logging** captures configuration, build, test, and install phases
- **Automatic issue creation** for build failures
- **Pull request comments** with build summaries

## Usage

### Manual Trigger

The workflow can be manually triggered with options:

```yaml
workflow_dispatch:
  inputs:
    build_category:
      description: 'Category to build (all, foundation, core, logic, cognitive, advanced, learning, language, robotics, integration, packaging)'
      required: false
      default: 'all'
    fail_fast:
      description: 'Fail fast on first error'
      required: false
      default: 'false'
```

### Automatic Triggers

- **Push to main/develop**: Full build matrix
- **Pull requests**: Full build matrix with PR comments

## Build Environment

### System Dependencies

Each layer installs appropriate system dependencies:
- Build tools: `build-essential`, `cmake`
- Boost libraries: `libboost-all-dev`
- Language support: `guile-3.0-dev`, `python3-dev`
- Specialized libraries: `libopencv-dev`, `librocksdb-dev`, etc.

### Configuration

All components are built with:
- **Build type**: Release
- **Install prefix**: `/usr/local`
- **Python executable**: System python3
- **Parallel builds**: Using all available cores

## Outputs

### Artifacts

- **Build info**: Status, timestamps, dependencies for each component
- **CMake caches**: Configuration details for debugging
- **Build summary**: Overall results and dependency analysis

### Reporting

- **GitHub Issues**: Created automatically for build failures
- **PR Comments**: Detailed build status for pull requests
- **Workflow logs**: Comprehensive build information

## Dependency Discovery Mechanisms

### 1. Build Order Analysis
Components are built in dependency order, revealing missing prerequisites.

### 2. Error Pattern Recognition
Common failure patterns indicate:
- Missing system packages
- Incorrect build configurations
- Version incompatibilities

### 3. Cross-Component Testing
Each layer validates that previous layers are properly installed.

### 4. Artifact Analysis
Build artifacts contain dependency information for post-build analysis.

## Customization

### Adding New Components

1. Add component to appropriate category matrix
2. Ensure component directory exists
3. Add any special build handling in the build step
4. Update documentation

### Modifying Dependencies

1. Adjust the `needs:` field for affected categories
2. Update prerequisite installation steps
3. Test dependency changes

### Build Configuration

Environment variables control build behavior:
- `OPENCOG_FAIL_NEVER`: Continue on errors
- `OPENCOG_MATRIX_DISCOVERY`: Enable dependency discovery
- `CMAKE_BUILD_TYPE`: Build configuration
- `MAKEFLAGS`: Parallel build settings

## Troubleshooting

### Common Issues

1. **Missing system dependencies**: Check category-specific install steps
2. **Component not found**: Verify component directory exists
3. **CMake configuration fails**: Check prerequisite installation
4. **Build failures**: Review component-specific build logs

### Debugging

1. Check workflow logs for detailed error messages
2. Download build artifacts for configuration details
3. Review dependency installation order
4. Test components individually if needed

## Integration with Existing Workflows

This workflow complements the existing `cogci.yml` by providing:
- More comprehensive component coverage
- Better dependency management
- Enhanced error reporting
- Systematic dependency discovery

The two workflows can run in parallel, providing different perspectives on the build health of the OpenCog ecosystem.