#!/bin/bash

# Test script for GitPod OpenCog environment
# This script validates the GitPod setup works correctly

echo "🧪 Testing GitPod OpenCog Environment"
echo "====================================="

# Test 1: Check if files exist
echo "📁 Checking configuration files..."
files=(".gitpod.yml" ".gitpod.Dockerfile" "setup-opencog-gitpod.sh")
for file in "${files[@]}"; do
    if [ -f "$file" ]; then
        echo "✅ $file exists"
    else
        echo "❌ $file missing"
        exit 1
    fi
done

# Test 2: Validate YAML syntax
echo "📝 Validating .gitpod.yml syntax..."
if python3 -c "import yaml; yaml.safe_load(open('.gitpod.yml', 'r'))" 2>/dev/null; then
    echo "✅ .gitpod.yml is valid YAML"
else
    echo "❌ .gitpod.yml has invalid syntax"
    exit 1
fi

# Test 3: Check script permissions
echo "🔐 Checking script permissions..."
if [ -x "setup-opencog-gitpod.sh" ]; then
    echo "✅ setup-opencog-gitpod.sh is executable"
else
    echo "❌ setup-opencog-gitpod.sh is not executable"
    exit 1
fi

# Test 4: Check if key directories exist
echo "📂 Checking OpenCog directories..."
key_dirs=("atomspace" "cogserver" "cogutil" "opencog" "docker")
for dir in "${key_dirs[@]}"; do
    if [ -d "$dir" ]; then
        echo "✅ $dir directory exists"
    else
        echo "⚠️ $dir directory not found (may be expected)"
    fi
done

# Test 5: Validate README has GitPod button
echo "📖 Checking README for GitPod button..."
if grep -q "gitpod.io" README.md; then
    echo "✅ GitPod button found in README.md"
else
    echo "❌ GitPod button missing from README.md"
    exit 1
fi

# Test 6: Check if setup script contains expected commands
echo "🔧 Validating setup script content..."
if grep -q "build-opencog" setup-opencog-gitpod.sh; then
    echo "✅ build-opencog command found in setup script"
else
    echo "❌ build-opencog command missing from setup script"
    exit 1
fi

echo ""
echo "🎉 All tests passed! GitPod environment is ready."
echo ""
echo "🚀 To use the GitPod environment:"
echo "  1. Click the GitPod button in README.md"
echo "  2. Wait for the environment to initialize"
echo "  3. Run ./setup-opencog-gitpod.sh"
echo "  4. Use build-opencog to build the complete environment"