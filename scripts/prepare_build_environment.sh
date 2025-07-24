#!/usr/bin/env bash
# prepare_build_environment.sh - Prepare environment for OpenCog builds
# This script ensures all necessary dependencies and configurations are in place

set -e

echo "🔧 Preparing build environment..."

# Ensure CMake config files exist for dependencies
if [ -f "scripts/create_cmake_configs.py" ]; then
    echo "📦 Creating CMake configuration files..."
    python3 scripts/create_cmake_configs.py
fi

# Create missing lib directories for common modules
for module in atomspace atomspace-rocks atomspace-restful ure moses cogserver; do
    if [ -d "$module" ] && [ ! -d "$module/lib" ]; then
        echo "📁 Creating lib directory for $module"
        mkdir -p "$module/lib"
        echo "# Empty lib directory for build compatibility" > "$module/lib/CMakeLists.txt"
    fi
done

# Ensure ldconfig is run to update library cache
echo "🔄 Updating library cache..."
sudo ldconfig

echo "✅ Build environment prepared successfully"