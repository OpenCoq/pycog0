#!/bin/bash

# OpenCog GitPod Service Scripts Fix
# This script creates missing service scripts that should have been created during deployment

set -e

echo "🔧 Creating missing OpenCog service scripts..."

# Ensure the bin directory exists
mkdir -p "$HOME/.local/bin"

# Set workspace path
WORKSPACE="${OPENCOG_WORKSPACE:-/workspace/opencog-org}"

# Create start-cogserver script
cat > "$HOME/.local/bin/start-cogserver" << 'EOF'
#!/bin/bash
echo "🖥️ Starting CogServer on port 17001..."
WORKSPACE="${OPENCOG_WORKSPACE:-/workspace/opencog-org}"
cd "$WORKSPACE/cogserver/build"
if [ -f "./opencog/cogserver/server/cogserver" ]; then
    ./opencog/cogserver/server/cogserver
else
    echo "❌ CogServer binary not found. Run build-cogserver first."
    echo "   Expected location: $WORKSPACE/cogserver/build/opencog/cogserver/server/cogserver"
fi
EOF

# Create start-atomspace-repl script
cat > "$HOME/.local/bin/start-atomspace-repl" << 'EOF'
#!/bin/bash
echo "🔬 Starting AtomSpace Guile REPL..."
WORKSPACE="${OPENCOG_WORKSPACE:-/workspace/opencog-org}"
cd "$WORKSPACE/atomspace/build"
if [ -f "./opencog/guile/opencog-guile" ]; then
    ./opencog/guile/opencog-guile
else
    echo "❌ AtomSpace Guile REPL not found. Run build-atomspace first."
    echo "   Expected location: $WORKSPACE/atomspace/build/opencog/guile/opencog-guile"
fi
EOF

# Make scripts executable
chmod +x "$HOME/.local/bin/start-cogserver"
chmod +x "$HOME/.local/bin/start-atomspace-repl"

# Add ~/.local/bin to PATH if not already there
if [[ ":$PATH:" != *":$HOME/.local/bin:"* ]]; then
    echo 'export PATH="$HOME/.local/bin:$PATH"' >> ~/.bashrc
    export PATH="$HOME/.local/bin:$PATH"
    echo "✅ Added ~/.local/bin to PATH"
fi

echo "✅ Service scripts created successfully!"
echo ""
echo "Available commands:"
echo "  start-cogserver      - Launch CogServer on port 17001"
echo "  start-atomspace-repl - Launch AtomSpace Guile REPL"
echo ""
echo "Note: You may need to run 'source ~/.bashrc' to reload your environment."