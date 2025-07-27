# OpenCog Dependency Build Matrix - Quick Start Guide

## Manual Workflow Execution

You can trigger the OpenCog Dependency Build Matrix workflow manually through GitHub's web interface:

### 1. Navigate to Actions Tab
Go to the Actions tab in your repository: `https://github.com/OpenCoq/opencog-org/actions`

### 2. Select the Workflow
Find and click on "OpenCog Dependency Build Matrix" in the workflow list.

### 3. Run Workflow
Click "Run workflow" and configure options:

- **Branch**: Choose the branch to run on (usually `main` or `develop`)
- **Build Category**: 
  - `all` (default) - Build all components in dependency order
  - `foundation` - Build only foundation components
  - `core` - Build only core components
  - `logic` - Build only logic components
  - `cognitive` - Build only cognitive components
  - `advanced` - Build only advanced components
  - `learning` - Build only learning components
  - `language` - Build only language components
  - `robotics` - Build only robotics components
  - `integration` - Build only integration components
- **Fail Fast**: 
  - `false` (default) - Continue building all components even if some fail
  - `true` - Stop on first failure

### 4. Monitor Progress

The workflow will show progress across multiple parallel jobs:

```
🏗️ Foundation Components
├── cogutil ✅
├── moses ✅  
├── blender_api_msgs ⚠️
├── external-tools ✅
└── ocpkg ⚠️

🏗️ Core Components  
├── atomspace ✅
├── atomspace-rocks ✅
├── atomspace-ipfs ⚠️
└── ... (parallel execution)

📊 Build Summary & Dependency Discovery
└── Final report with discovered dependencies
```

## Expected Outputs

### Build Artifacts
- Component-specific build information
- CMake configuration caches
- Error logs and dependency analysis

### Automated Reports
- **GitHub Issues**: Created for any build failures
- **PR Comments**: Build summary for pull requests
- **Workflow Summary**: Overall build health dashboard

### Dependency Discovery
The workflow will identify:
- Missing system dependencies
- Undocumented component dependencies  
- Build order optimizations
- Configuration conflicts

## Example Workflow Run

A typical full build will execute approximately 40+ parallel jobs across 9 dependency layers, taking around 30-45 minutes to complete with comprehensive dependency analysis and failure reporting.

## Integration with Development

This workflow complements existing development processes by:
- Providing early detection of dependency issues
- Validating changes across the entire OpenCog ecosystem
- Discovering hidden dependencies through systematic testing
- Generating actionable failure reports for developers

## Next Steps

After implementing this workflow, you can:
1. **Monitor build health** across all components
2. **Analyze dependency patterns** from build artifacts  
3. **Optimize build order** based on discovered dependencies
4. **Enhance component isolation** by addressing hidden dependencies
5. **Improve documentation** with discovered dependency information