# GitPod Dockerfile for OpenCog development environment
# Based on gitpod/workspace-python-3.10 for optimal OpenCog compatibility

FROM gitpod/workspace-python-3.10:2025-07-23-06-50-33

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive
ENV GUIX_PROFILE="/home/gitpod/.guix-profile"
ENV PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"
ENV GUIX_LOCPATH="$GUIX_PROFILE/lib/locale"

# Switch to gitpod user
USER gitpod

# Install system dependencies for OpenCog
RUN sudo apt-get update && sudo apt-get install -y \
    build-essential \
    cmake \
    cmake-data \
    git \
    libboost-all-dev \
    libcppunit-dev \
    guile-3.0-dev \
    libbz2-dev \
    libexpat1-dev \
    libgsl-dev \
    libhyphen-dev \
    libpthread-stubs0-dev \
    libtool \
    libzmq3-dev \
    pkg-config \
    python3-dev \
    python3-pip \
    python3-numpy \
    python3-scipy \
    cython3 \
    python3-nose \
    binutils \
    unzip \
    wget \
    curl \
    lsb-release \
    locales \
    locales-all \
    && sudo apt-get clean \
    && sudo rm -rf /var/lib/apt/lists/*

# Install Guix package manager for reproducible builds
RUN cd /tmp && \
    wget https://git.savannah.gnu.org/cgit/guix.git/plain/etc/guix-install.sh && \
    chmod +x guix-install.sh && \
    sudo ./guix-install.sh --enable-substitutes && \
    sudo ln -sf /var/guix/profiles/per-user/root/current-guix/bin/guix /usr/local/bin/guix && \
    rm guix-install.sh

# Set up Guix environment for gitpod user
RUN sudo mkdir -p /var/guix/profiles/per-user/gitpod && \
    sudo chown gitpod:gitpod /var/guix/profiles/per-user/gitpod && \
    sudo usermod -aG guixbuild gitpod

# Install Python packages commonly used with OpenCog
RUN pip3 install --user \
    numpy \
    scipy \
    matplotlib \
    jupyter \
    notebook \
    cython \
    nose \
    pytest \
    networkx \
    scikit-learn \
    pandas

# Create directories for OpenCog development
RUN mkdir -p /home/gitpod/opencog-workspace && \
    mkdir -p /home/gitpod/opencog-build && \
    mkdir -p /home/gitpod/.local/bin

# Add helpful scripts for improved workflow
RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/opencog_status && \
    echo 'echo "🧠 OpenCog Environment Status"' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'echo "=========================="' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'echo "Workspace: $OPENCOG_WORKSPACE"' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'echo "Build Dir: $OPENCOG_BUILD_DIR"' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'echo ""' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'for comp in cogutil atomspace cogserver; do' >> /home/gitpod/.local/bin/opencog_status && \
    echo '  if [ -d "/workspace/opencog-org/$comp" ]; then' >> /home/gitpod/.local/bin/opencog_status && \
    echo '    echo "  ✅ $comp"' >> /home/gitpod/.local/bin/opencog_status && \
    echo '  else' >> /home/gitpod/.local/bin/opencog_status && \
    echo '    echo "  ❌ $comp (not found)"' >> /home/gitpod/.local/bin/opencog_status && \
    echo '  fi' >> /home/gitpod/.local/bin/opencog_status && \
    echo 'done' >> /home/gitpod/.local/bin/opencog_status && \
    chmod +x /home/gitpod/.local/bin/opencog_status

# Enhanced build scripts for improved development workflow
RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/build-component && \
    echo 'if [ $# -eq 0 ]; then' >> /home/gitpod/.local/bin/build-component && \
    echo '  echo "Usage: build-component <component-name>"' >> /home/gitpod/.local/bin/build-component && \
    echo '  exit 1' >> /home/gitpod/.local/bin/build-component && \
    echo 'fi' >> /home/gitpod/.local/bin/build-component && \
    echo 'cd /workspace/opencog-org/$1' >> /home/gitpod/.local/bin/build-component && \
    echo 'mkdir -p build && cd build' >> /home/gitpod/.local/bin/build-component && \
    echo 'cmake .. -DCMAKE_BUILD_TYPE=Release && make -j2' >> /home/gitpod/.local/bin/build-component && \
    chmod +x /home/gitpod/.local/bin/build-component

# Service management scripts
RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/demos && \
    echo 'echo "🎮 OpenCog Demos Available:"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo ""' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "1. CogServer Demo:"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "   start-cogserver"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo ""' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "2. AtomSpace Python Demo:"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "   python3 -c '\''from opencog.atomspace import *; print(\"AtomSpace available\")'\'"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo ""' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "3. Guile Scheme Demo:"' >> /home/gitpod/.local/bin/demos && \
    echo 'echo "   start-atomspace-repl"' >> /home/gitpod/.local/bin/demos && \
    chmod +x /home/gitpod/.local/bin/demos

RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/guix-shell && \
    echo 'guix shell gcc-toolchain cmake pkg-config boost cppunit guile python python-cython' >> /home/gitpod/.local/bin/guix-shell && \
    chmod +x /home/gitpod/.local/bin/guix-shell

# Enhanced environment and welcome setup
RUN echo 'export PATH="$HOME/.local/bin:$PATH"' >> /home/gitpod/.bashrc && \
    echo 'export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"' >> /home/gitpod/.bashrc && \
    echo 'export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"' >> /home/gitpod/.bashrc && \
    echo 'export OPENCOG_WORKSPACE="/workspace/opencog-org"' >> /home/gitpod/.bashrc && \
    echo 'export OPENCOG_BUILD_DIR="$HOME/opencog-build"' >> /home/gitpod/.bashrc

# Useful aliases for OpenCog development
RUN echo 'alias ll="ls -la"' >> /home/gitpod/.bashrc && \
    echo 'alias og="cd /workspace/opencog-org"' >> /home/gitpod/.bashrc && \
    echo 'alias ogb="cd $HOME/opencog-build"' >> /home/gitpod/.bashrc && \
    echo 'alias build-all="build-opencog"' >> /home/gitpod/.bashrc && \
    echo 'alias status="opencog_status"' >> /home/gitpod/.bashrc

# Set up Git configuration for development
RUN git config --global core.editor "code --wait" && \
    git config --global init.defaultBranch main

# Enhanced welcome message
RUN echo 'echo "🧠 OpenCog GitPod Environment Loaded!"' >> /home/gitpod/.bashrc && \
    echo 'echo "Use: opencog_status to check environment or demos to see examples"' >> /home/gitpod/.bashrc

# Expose common OpenCog ports
EXPOSE 17001 18001 5000 8080

# Set working directory
WORKDIR /workspace/opencog-org