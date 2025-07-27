# OpenCog Unified Build System

This document describes the unified CMake build system for the OpenCog Organization repository.

## Overview

The OpenCog Organization repository contains 46+ individual OpenCog components, each with their own CMakeLists.txt and build requirements. The unified build system provides:

- **Component Discovery**: Automatically finds all buildable components
- **Dependency-Aware Organization**: Components are organized by dependency layers
- **Individual Component Builds**: Each component builds in its own isolated directory
- **Collective Build Targets**: Build entire layers or all components at once
- **Clear Status Reporting**: Know exactly what's available and what's missing

## Quick Start

```bash
# Clone the repository (if not already done)
git clone https://github.com/OpenCoq/opencog-org.git
cd opencog-org

# Configure the unified build system
mkdir build && cd build
cmake ..

# See all available build targets
make list-components

# Build a specific component
make cogutil

# Build all foundation components
make Foundation-layer

# Configure all available components
make configure-all

# Build all available components (may take a long time!)
make all-components
```

## Available Build Targets

### Individual Components

Build any individual component:
```bash
make <component-name>
```

Available components include:
- **Foundation Layer**: cogutil, moses, blender_api_msgs
- **Core Layer**: atomspace, atomspace-rocks, atomspace-pgres, atomspace-ipfs, etc.
- **Logic Layer**: unify, ure
- **Cognitive Layer**: cogserver, attention, spacetime, pattern-index, etc.
- **Advanced Layer**: pln, miner, asmoses, benchmark
- **Learning Layer**: learn, generate
- **Language Layer**: lg-atomese
- **Robotics Layer**: vision, perception, sensory, ros-behavior-scripting, etc.
- **Integration Layer**: opencog, TinyCog
- **Specialized Components**: visualization, cheminformatics, agi-bio, etc.

### Layer-Based Targets

Build entire dependency layers:
```bash
make Foundation-layer    # Build all foundation components
make Core-layer         # Build all core components  
make Logic-layer        # Build all logic components
make Cognitive-layer    # Build all cognitive components
make Advanced-layer     # Build all advanced components
make Learning-layer     # Build all learning components
make Language-layer     # Build all language components
make Robotics-layer     # Build all robotics components
make Integration-layer  # Build all integration components
make Specialized-layer  # Build all specialized components
```

### Collective Targets

```bash
make configure-all      # Configure all available components
make all-components     # Build all available components
make list-components    # Show available build targets and help
```

## How It Works

### Component Discovery

The unified build system automatically discovers available components by:
1. Checking each expected component directory for a `CMakeLists.txt` file
2. Registering components that are available for building
3. Creating individual build targets for each available component

### Isolated Builds

Each component is built in its own isolated directory (`<component>-build/`) to:
- Avoid conflicts between different component configurations
- Allow components to have different dependency requirements
- Enable parallel builds of independent components
- Maintain clean separation of build artifacts

### Dependency Layers

Components are organized into dependency layers based on the OpenCog dependency diagrams:

1. **Foundation**: Core utilities and base components
2. **Core**: AtomSpace and storage backends  
3. **Logic**: Unification and rule engines
4. **Cognitive**: Cognitive architecture components
5. **Advanced**: Advanced reasoning and mining
6. **Learning**: Machine learning frameworks
7. **Language**: Natural language processing
8. **Robotics**: Robotic and perception components
9. **Integration**: Main OpenCog applications
10. **Specialized**: Domain-specific applications

## Component Status

During configuration, you'll see a summary showing:
- **Available Components**: Components with CMakeLists.txt that can be built
- **Missing Components**: Expected components without CMakeLists.txt files
- **Availability Rate**: Percentage of expected components that are available

Example output:
```
Available Components (46):
  - cogutil
  - moses
  - atomspace
  [... more components ...]

Missing Components (6):
  - external-tools
  - ocpkg
  - profile
  [... missing components ...]

Discovery Statistics:
  Total components checked: 52
  Available for building: 46
  Missing CMakeLists.txt: 6
  Availability rate: 88% (46/52)
```

## Building Individual Components

When you build an individual component:

```bash
make cogutil
```

The system will:
1. Configure the component in `cogutil-build/`
2. Build the component using its own CMakeLists.txt
3. Report the build result

If a component fails to build due to missing dependencies, the error will be contained to that component and won't affect others.

## System Requirements

Components have their own individual requirements. Common requirements across the OpenCog ecosystem include:

- **C++ Compiler**: GCC 7+ or Clang 6+
- **CMake**: 3.12+
- **Boost**: 1.60+
- **Python**: 3.6+
- **Guile**: 2.2+ or 3.0+

Individual components may have additional requirements like:
- Various databases (PostgreSQL, RocksDB)
- Machine learning libraries
- Robotics frameworks (ROS)
- Specialized libraries for specific domains

Check each component's README for specific requirements.

## Troubleshooting

### Component Won't Configure
If `make configure-<component>` fails:
1. Check the component's README for specific dependencies
2. Install missing dependencies for that component
3. Look at the error output in the component's build directory

### Component Won't Build  
If `make <component>` fails after configuration:
1. Check the build log in `<component>-build/`
2. Ensure all dependencies are properly installed
3. Try building dependencies first (e.g., build `cogutil` before `atomspace`)

### Missing Components
If a component shows as "Missing" in the discovery:
- The component may not have been ported to CMake yet
- The component may be deprecated or moved
- The component name may have changed

## Contributing

To add support for a new component:

1. Ensure the component has a working `CMakeLists.txt`
2. Add the component to the appropriate layer in the main `CMakeLists.txt`
3. Test that the component can be configured and built
4. Update this documentation

## Integration with Existing Workflows

This unified build system is designed to complement, not replace, existing build workflows:

- **Individual Development**: Developers can still build components individually in their own directories
- **CI/CD Integration**: The unified system provides clear targets for automated builds
- **Dependency Management**: Layer-based organization helps with dependency order
- **Status Monitoring**: Easy to see what's available and what's broken

The unified build system provides a "bird's eye view" of the entire OpenCog ecosystem while maintaining the flexibility of individual component builds.