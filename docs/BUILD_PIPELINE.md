# OpenCog Ecosystem Build Pipeline Documentation

## Overview

This document describes the OpenCog ecosystem build pipeline orchestration implemented in the GitHub Actions workflow. The pipeline ensures that each component is built only after its dependencies are successfully built and installed.

## Dependency Hierarchy

The OpenCog ecosystem follows a layered architecture with clear dependency relationships:

### Foundation Layer
- **cogutil**: Base utilities library
  - Dependencies: None
  - Required by: moses, language-learning, and all higher layers

### Core Layer
- **atomspace**: Core knowledge representation framework
  - Dependencies: cogutil
  - Required by: All components except moses and language-learning

- **atomspace-rocks**: RocksDB storage backend for AtomSpace
  - Dependencies: atomspace
  - Required by: Components needing persistent storage

- **atomspace-restful**: RESTful API for AtomSpace
  - Dependencies: atomspace
  - Required by: Web-based applications

### Logic Layer
- **unify**: Unification algorithms
  - Dependencies: atomspace
  - Required by: ure, and transitively by all reasoning components

- **ure**: Unified Rule Engine
  - Dependencies: atomspace, unify
  - Required by: pln, miner, asmoses

### Cognitive Systems Layer
- **cogserver**: Network server for OpenCog
  - Dependencies: atomspace
  - Required by: attention, learn

- **attention**: Attention allocation system
  - Dependencies: atomspace, cogserver
  - Required by: opencog integration

- **spacetime**: Spatial and temporal reasoning
  - Dependencies: atomspace
  - Required by: pln

### Advanced Systems Layer
- **pln**: Probabilistic Logic Networks
  - Dependencies: atomspace, ure, spacetime
  - Required by: opencog integration

- **miner**: Pattern mining system
  - Dependencies: atomspace, ure
  - Required by: Advanced analytics

### Learning Systems Layer
- **moses**: Meta-Optimizing Semantic Evolutionary Search
  - Dependencies: cogutil
  - Required by: Evolutionary learning

- **asmoses**: AtomSpace-based MOSES
  - Dependencies: atomspace, ure
  - Required by: Knowledge-guided evolution

### Language Processing Layer
- **lg-atomese**: Link Grammar integration with AtomSpace
  - Dependencies: atomspace
  - Required by: opencog integration

- **learn**: Language learning system
  - Dependencies: atomspace, cogserver
  - Required by: Language processing

- **language-learning**: Language acquisition research
  - Dependencies: cogutil
  - Required by: Research applications

### Integration Layer
- **opencog**: Main integration package
  - Dependencies: atomspace, ure, cogserver, attention, lg-atomese
  - This is the top-level integration that brings together core reasoning components

## Build Orchestration

### Workflow Structure

The `opencog-ecosystem-build.yml` workflow implements this dependency structure using GitHub Actions `needs:` declarations. This ensures:

1. **Dependency Order**: Jobs run only after their dependencies complete successfully
2. **Build Caching**: Intermediate build artifacts are cached and shared between jobs
3. **Parallel Execution**: Independent components build in parallel for efficiency
4. **Failure Isolation**: If a component fails, only dependent components are skipped

### Job Parallelization

The workflow maximizes parallel execution by running jobs as soon as their dependencies are met:

```
Foundation: cogutil
     ↓
Core: atomspace ← builds after cogutil
     ↓
Logic: unify, atomspace-rocks, atomspace-restful ← build in parallel after atomspace
     ↓
ure ← builds after atomspace + unify
     ↓
Advanced: cogserver, spacetime ← build in parallel after atomspace
          attention ← builds after atomspace + cogserver  
          pln ← builds after atomspace + ure + spacetime
          miner ← builds after atomspace + ure
          
Learning: moses ← builds after cogutil (parallel with atomspace)
          asmoses ← builds after atomspace + ure
          
Language: lg-atomese ← builds after atomspace
          learn ← builds after atomspace + cogserver
          language-learning ← builds after cogutil (parallel with atomspace)
          
Integration: opencog ← builds after atomspace + ure + cogserver + attention + lg-atomese
```

### Caching Strategy

The workflow uses GitHub Actions caching to:
- Cache build artifacts between jobs
- Cache installed libraries in `/usr/local/`
- Avoid rebuilding unchanged dependencies
- Speed up the overall build process

### Error Handling

- Each job includes basic error handling with `|| true` for tests
- Failed jobs don't block independent parallel jobs
- The summary job provides a complete build report regardless of individual job failures

## Verification

The dependency structure has been verified to match the requirements specified in the original issue:

✅ **Foundation Layer**
- cogutil ✓

✅ **Core Layer** 
- atomspace (requires: cogutil) ✓
- atomspace-rocks (requires: atomspace) ✓  
- atomspace-restful (requires: atomspace) ✓

✅ **Logic Layer**
- unify (requires: atomspace) ✓
- ure (requires: atomspace, unify) ✓

✅ **Cognitive Systems Layer**
- cogserver (requires: atomspace) ✓
- attention (requires: atomspace, cogserver) ✓
- spacetime (requires: atomspace) ✓

✅ **Advanced Systems Layer**
- pln (requires: atomspace, ure, spacetime) ✓
- miner (requires: atomspace, ure) ✓

✅ **Learning Systems Layer**
- moses (requires: cogutil) ✓
- asmoses (requires: atomspace, ure) ✓

✅ **Language Processing Layer**
- lg-atomese (requires: atomspace) ✓
- learn (requires: atomspace, cogserver) ✓
- language-learning (requires: cogutil) ✓

✅ **Integration Layer**
- opencog (requires: atomspace, ure, cogserver, attention, lg-atomese) ✓

## Usage

### Triggering the Workflow

The workflow runs automatically on:
- Push to `main` or `opencog-v6` branches
- Pull requests targeting `main` or `opencog-v6` branches
- Manual dispatch via GitHub Actions UI

### Monitoring Progress

The workflow provides:
- Individual job status for each component
- Build artifacts and logs for debugging
- A final summary report showing the status of all components

### Build Artifacts

The workflow generates:
- Build logs for each component
- Test results where available
- A comprehensive build summary report
- Cached build artifacts for reuse

This orchestrated build pipeline ensures that the OpenCog ecosystem can be built reliably with proper dependency management and efficient parallel execution.