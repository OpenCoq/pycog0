# OpenCog Ecosystem Build Scripts

This directory contains scripts that enhance the OpenCog ecosystem build pipeline with adaptive, self-healing capabilities.

## Workflow Validation

### `validate_workflow.py`

A validation script that ensures the `generate-next-steps.yml` workflow components are working correctly.

#### Features

- **File Structure Validation**: Checks that `AGENT-ZERO-GENESIS.md` exists and has correct format
- **Content Parsing**: Validates phase headers and task lines can be parsed correctly  
- **JSON Generation**: Tests JSON generation and base64 encoding/decoding
- **Data Integrity**: Ensures no data is lost during encoding process

#### Usage

```bash
python3 scripts/validate_workflow.py
```

#### Integration

This script is automatically run as part of the `generate-next-steps.yml` workflow to catch issues early.

## Adaptive Boost Installation

### `adaptive-boost-install.sh`

An intelligent script that adapts Boost library installation based on the target Ubuntu version, with special handling for Ubuntu Noble's t64 ABI transition.

#### Features

- **OS Version Detection**: Automatically detects Ubuntu version and applies appropriate package strategy
- **Ubuntu Noble t64 Support**: Handles the t64 ABI transition introduced in Ubuntu 24.04 (Noble)
- **Cognitive Logging**: Comprehensive logging for meta-cognitive adaptation and debugging
- **Self-Healing Pipeline**: Built-in diagnostics and error recovery mechanisms
- **Resource Monitoring**: Tracks memory usage and system health during installation
- **Metrics Collection**: Generates JSON metrics for pipeline optimization

#### Usage

```bash
# Basic usage
./scripts/adaptive-boost-install.sh

# The script will automatically:
# 1. Detect the Ubuntu version
# 2. Select appropriate Boost packages (handling t64 ABI if needed)
# 3. Install packages with error handling and retry logic
# 4. Generate comprehensive logs and metrics
```

#### Supported Ubuntu Versions

- **Ubuntu 24.04 (Noble)**: Uses t64-aware package strategy
- **Ubuntu 22.04 (Jammy)**: Standard package strategy
- **Ubuntu 20.04 (Focal)**: Standard package strategy
- **Ubuntu 18.04 (Bionic)**: Standard package strategy
- **Other versions**: Falls back to standard strategy with logging

#### Output Files

- **Log File**: `/tmp/adaptive-boost-install.log` - Detailed installation logs
- **Metrics File**: `/tmp/boost-install-metrics.json` - Machine-readable metrics

#### Cognitive Framework Integration

This script implements the cognitive framework outlined in the original issue:

1. **Memory System**: Package and version resolution, dependency tracking
2. **Task System**: Automated build pipeline orchestration
3. **AI System**: Adaptive installation logic, version detection, error parsing
4. **Autonomy System**: Self-modifying workflow to maintain compatibility

#### Library Dependencies and ABI Mapping

The script understands the following Boost library mappings for Ubuntu Noble:

| Generic Package | Noble t64 Package | Purpose |
|-----------------|-------------------|---------|
| libboost-chrono-dev | libboost-chrono1.83.0t64 | Time duration and clocks |
| libboost-container-dev | libboost-container1.83.0 | STL-like containers |
| libboost-all-dev | (meta-package) | All Boost libraries |

#### Error Handling

The script implements multiple layers of error handling:

- **Package Verification**: Checks package availability before installation
- **Retry Logic**: Retries failed installations with exponential backoff
- **System Diagnostics**: Validates system state and recovers from common issues
- **Cognitive Metrics**: Records all errors for future adaptation

#### Integration with GitHub Actions

The script is designed to replace manual `apt-get install` commands in GitHub Actions workflows:

```yaml
# Old approach
- name: Install dependencies
  run: |
    sudo apt-get update
    sudo apt-get install -y build-essential cmake libboost-all-dev

# New adaptive approach
- name: Install dependencies with adaptive boost strategy
  run: |
    ./scripts/adaptive-boost-install.sh
```

## Future Enhancements

As mentioned in the cognitive framework, future versions will include:

1. **GGML Kernel Integration**: Record tensor dimensions of Boost libraries and their coupling in the build graph
2. **Hypergraph Pattern Encoding**: Transform dependency relationships into distributed cognition framework
3. **Dynamic Attention Allocation**: Automatically adjust resource allocation based on build complexity
4. **Distributed Cognition**: Enable the pipeline to learn from across multiple repository builds

## Contributing

When contributing to these scripts, please:

1. Maintain the cognitive logging framework
2. Add comprehensive error handling
3. Update metrics collection for new features
4. Test across multiple Ubuntu versions
5. Document ABI transitions and package mappings

## Meta-Cognitive Adaptation

The scripts are designed to learn and adapt over time by:

- Recording successful and failed installation patterns
- Tracking system resource usage during builds
- Monitoring package availability changes
- Adapting to new Ubuntu releases automatically

This forms the foundation for a truly self-healing, cognitive build pipeline that embodies the OpenCog vision of artificial general intelligence applied to software engineering.