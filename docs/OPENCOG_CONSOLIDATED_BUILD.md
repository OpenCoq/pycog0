# OpenCog Consolidated Build Implementation

This document describes the implementation of the OpenCog Consolidated CircleCI Configuration as a GitHub Actions workflow.

## Overview

The workflow file `.github/workflows/opencog-consolidated-build.yml` implements the exact CircleCI configuration provided in issue #71, converting it to GitHub Actions syntax while maintaining identical functionality and dependency structure.

## Architecture

The build follows a layered architecture with proper dependency management:

### 1. Foundation Layer
- **cogutil**: Core utilities and base functionality

### 2. Core Layer  
- **atomspace**: Core knowledge representation framework
- **atomspace-rocks**: RocksDB storage backend for AtomSpace
- **atomspace-restful**: RESTful API for AtomSpace

### 3. Logic Layer
- **unify**: Unification algorithm implementation  
- **ure**: Unified Reasoning Engine

### 4. Cognitive Systems Layer
- **cogserver**: Cognitive server infrastructure
- **attention**: Attention allocation system
- **spacetime**: Spatial and temporal reasoning

### 5. Advanced Systems Layer
- **pln**: Probabilistic Logic Networks
- **miner**: Pattern mining system

### 6. Learning Systems Layer
- **moses**: Machine learning and evolution
- **asmoses**: AtomSpace-based MOSES

### 7. Language Processing Layer
- **lg-atomese**: Link Grammar integration
- **learn**: Language learning framework
- **language-learning**: Advanced language learning tools

### 8. Integration Layer
- **opencog**: Main OpenCog integration

### 9. Packaging Layer
- **package**: Debian package creation (master branch only)

## Key Features

### CircleCI to GitHub Actions Conversion

| CircleCI Feature | GitHub Actions Equivalent | Implementation |
|------------------|---------------------------|----------------|
| `docker` | `container` | Ubuntu 22.04 containers with root user |
| `persist_to_workspace`/`attach_workspace` | `upload-artifact`/`download-artifact` | Workspace persistence between jobs |
| `restore_cache`/`save_cache` | `cache` action | Build caching with ccache |
| `services` | `services` | PostgreSQL service container |
| `when: always` | `if: always()` | Conditional execution |

### Build Optimizations

1. **Ccache Integration**: Maintains build cache across runs
2. **Workspace Persistence**: Shares build artifacts between dependent jobs
3. **Container Optimization**: Uses consistent Ubuntu 22.04 environment
4. **Dependency Management**: Proper installation order with ldconfig

### Trigger Configuration

The workflow triggers on:
- Push to `main` or `master` branches
- Pull requests to `main` or `master` branches  
- Manual workflow dispatch

### Environment Variables

- `CCACHE_DIR`: `/ws/ccache` - Cache directory for build optimization
- `MAKEFLAGS`: `-j2` - Parallel make with 2 jobs
- `WORKING_DIRECTORY`: `/ws` - Common workspace directory

## Testing

The workflow includes comprehensive testing:
- CMake configuration validation
- Build process execution
- Unit test execution with `make check`
- Test log preservation (always executed)

## Dependencies

Each job properly declares its dependencies using the `needs` keyword, ensuring correct build order:

```yaml
# Example dependency chain
cogutil → atomspace → ure → pln
         ↓
         cogserver → attention
```

## Package Distribution

The `package` job creates Debian packages and only runs:
- On the `master` branch
- After successful completion of all other jobs
- Stores artifacts for distribution

## Monitoring and Debugging

- All test logs are preserved using `if: always()`
- Build artifacts are uploaded for inspection
- Container logs provide detailed build information
- Workspace artifacts allow debugging failed builds

This implementation provides a complete, production-ready CI/CD pipeline that maintains full compatibility with the original CircleCI configuration while leveraging GitHub Actions' native features.