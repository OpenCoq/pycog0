# CI Workflow Fix: Permissions and Dependencies

## Problem Summary

The `agent-zero-ci.yml` workflow was experiencing several critical failures:

### 1. Permission Errors
```
/usr/bin/tar: Cannot utime: Operation not permitted
/usr/bin/tar: Cannot change mode to rwxr-xr-x: Operation not permitted
/usr/bin/tar: Cannot open: File exists
```
**Root Cause**: The workflow attempted to cache system directories (`/usr/local/lib`, `/usr/local/include`, etc.) which require elevated permissions that GitHub Actions cache operations cannot provide in non-root contexts.

### 2. Cache Restoration Failures
```
Failed to restore: "/usr/bin/tar" failed with exit code 2
```
**Root Cause**: Cache restore operations failed due to permission conflicts when trying to extract cached files to system directories.

### 3. Missing Dependencies
```
CMake Error: Package 'atomspace' required by 'virtual:world', not found
CMake Error: Package 'cogserver' required by 'virtual:world', not found
```
**Root Cause**: When cache restoration failed, subsequent jobs could not find the required OpenCog dependencies, causing CMake configuration to fail.

## Solution Implemented

### Artifact-Based Dependency Sharing

Instead of caching system directories, the solution uses GitHub Actions artifacts to share built dependencies between jobs:

#### 1. Build Job (build-opencog-dependencies)
```yaml
- name: Build and install cogutil
  run: |
    cd cogutil
    mkdir -p build && cd build
    cmake .. \
      -DCMAKE_BUILD_TYPE=${{ env.CMAKE_BUILD_TYPE }} \
      -DCMAKE_INSTALL_PREFIX=${{ github.workspace }}/opencog-install \
      -DCMAKE_C_COMPILER_LAUNCHER=ccache \
      -DCMAKE_CXX_COMPILER_LAUNCHER=ccache
    make -j$(nproc)
    make install
    
    # Also install to system for subsequent builds in this job
    sudo make install
    sudo ldconfig

- name: Upload OpenCog dependencies as artifact
  uses: actions/upload-artifact@v4
  with:
    name: opencog-dependencies-${{ runner.os }}
    path: ${{ github.workspace }}/opencog-install/
    retention-days: 1
```

**Key Changes**:
- Install to local directory first: `-DCMAKE_INSTALL_PREFIX=${{ github.workspace }}/opencog-install`
- Still install to system for same-job builds: `sudo make install`
- Upload artifacts from workspace directory (no permission issues)
- 1-day retention (sufficient for workflow job chain)

#### 2. Dependent Jobs (build-agent-zero-core, build-agent-zero-full, test-demonstrations)
```yaml
- name: Download OpenCog dependencies artifact
  uses: actions/download-artifact@v4
  with:
    name: opencog-dependencies-${{ runner.os }}
    path: ${{ github.workspace }}/opencog-install
  continue-on-error: true

- name: Install OpenCog dependencies from artifact or rebuild if missing
  run: |
    # Install from artifact if available
    if [ -d "${{ github.workspace }}/opencog-install" ] && [ "$(ls -A ${{ github.workspace }}/opencog-install)" ]; then
      echo "📦 Installing OpenCog dependencies from artifact..."
      sudo cp -r ${{ github.workspace }}/opencog-install/* /usr/local/
      sudo ldconfig
    fi
    
    # Check if OpenCog libraries are available
    sudo ldconfig
    export PKG_CONFIG_PATH=/usr/local/lib/pkgconfig:/usr/local/share/pkgconfig:/usr/lib/pkgconfig:$PKG_CONFIG_PATH
    
    MISSING=0
    if ! pkg-config --exists cogutil 2>/dev/null; then MISSING=1; fi
    if ! pkg-config --exists atomspace 2>/dev/null; then MISSING=1; fi
    if ! pkg-config --exists cogserver 2>/dev/null; then MISSING=1; fi
    
    if [ $MISSING -eq 1 ]; then
      echo "🔧 OpenCog dependencies not found in artifact - rebuilding from source"
      # ... rebuild logic ...
    fi
```

**Key Changes**:
- Download artifacts to workspace directory
- `continue-on-error: true` prevents failure if artifact upload/download has issues
- Explicitly copy artifacts to system location with `sudo cp -r`
- Improved verification with pkg-config checks
- Robust fallback to rebuild if artifacts missing or incomplete

## Benefits of This Approach

### 1. No Permission Issues
- All artifact operations work in user-accessible directories
- Only `sudo cp` requires elevation (which is properly available in GitHub Actions)
- No tar permission conflicts

### 2. Better Error Handling
- `continue-on-error: true` on artifact download prevents hard failures
- Explicit artifact installation with status checking
- Fallback rebuild mechanism ensures workflow continues even if artifacts unavailable

### 3. Improved Reliability
- Artifacts are explicitly validated before use
- pkg-config verification ensures libraries are properly installed
- Clear logging distinguishes between artifact installation and rebuild

### 4. Consistent Behavior
- Same pattern used across all dependent jobs
- Clear separation of concerns (build once, distribute via artifacts)
- Predictable dependency propagation

## Implementation Details

### Files Modified
- `.github/workflows/agent-zero-ci.yml`: Main workflow file with all changes

### Lines Changed
- 62 insertions, 45 deletions
- All cache operations replaced with artifact operations
- Improved dependency verification in all jobs

### Validation
- ✅ YAML syntax validated with PyYAML
- ✅ No breaking changes to existing job structure
- ✅ Fallback mechanisms preserve existing rebuild logic

## Testing Recommendations

1. **Verify Artifact Upload**: Check that `opencog-dependencies-${{ runner.os }}` artifact is created in build-opencog-dependencies job
2. **Verify Artifact Download**: Confirm subsequent jobs download and extract artifacts successfully
3. **Verify Installation**: Check logs for "📦 Installing OpenCog dependencies from artifact..." message
4. **Verify pkg-config**: Ensure cogutil, atomspace, and cogserver are found via pkg-config
5. **Test Fallback**: Artificially fail artifact download to verify rebuild logic works

## Related Documentation

- GitHub Actions Artifacts: https://docs.github.com/en/actions/using-workflows/storing-workflow-data-as-artifacts
- OpenCog Dependency Build: See `docs/OPENCOG_DEPENDENCY_BUILD.md`
- Agent-Zero CI: See `agents/AGENT_ZERO_CI_README.md`

## Adherence to Problem Statement

This implementation addresses all issues identified in the cognitive diagnostic flowchart:

✅ **PATHWAY A: Fix cache restore & permissions**
- Restricted all operations to project/workspace directories
- Removed system directory caching completely
- Proper use of sudo only where needed (cp, make install)

✅ **PATHWAY B: Ensure OpenCog dependencies are available**
- Explicit build and installation of atomspace, cogserver, cogutil
- Artifact-based distribution ensures availability across jobs
- Fallback rebuild if artifacts missing

✅ **PATHWAY C: CMake/Workflow robustness**
- Pre-checks with pkg-config verify dependencies before CMake
- PKG_CONFIG_PATH properly configured
- Clear error messages guide debugging

## Conclusion

This fix resolves the root causes of CI failures by:
1. Eliminating permission errors through workspace-based artifacts
2. Ensuring reliable dependency propagation between jobs
3. Providing robust fallback mechanisms
4. Improving error handling and debugging visibility

The workflow now follows GitHub Actions best practices and OpenCog dependency management patterns.
