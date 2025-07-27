# GitPod Dockerfile for OpenCog development environment
# Based on gitpod/workspace-python-3.10 as requested in issue #7

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
    libguile-3.0-dev \
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
    python3-cython \
    python3-nose \
    python3-cogserver \
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

# Add helpful scripts
RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/build-atomspace && \
    echo 'cd /workspace/opencog-org/atomspace' >> /home/gitpod/.local/bin/build-atomspace && \
    echo 'mkdir -p build && cd build' >> /home/gitpod/.local/bin/build-atomspace && \
    echo 'cmake .. && make -j$(nproc)' >> /home/gitpod/.local/bin/build-atomspace && \
    chmod +x /home/gitpod/.local/bin/build-atomspace

RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/build-cogserver && \
    echo 'cd /workspace/opencog-org/cogserver' >> /home/gitpod/.local/bin/build-cogserver && \
    echo 'mkdir -p build && cd build' >> /home/gitpod/.local/bin/build-cogserver && \
    echo 'cmake .. && make -j$(nproc)' >> /home/gitpod/.local/bin/build-cogserver && \
    chmod +x /home/gitpod/.local/bin/build-cogserver

RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/run-demos && \
    echo 'echo "🎮 OpenCog Demos:"' >> /home/gitpod/.local/bin/run-demos && \
    echo 'echo "  - CogServer: cd cogserver/build && ./opencog/cogserver/server/cogserver"' >> /home/gitpod/.local/bin/run-demos && \
    echo 'echo "  - AtomSpace REPL: cd atomspace/build && ./opencog/guile/opencog-guile"' >> /home/gitpod/.local/bin/run-demos && \
    echo 'echo "  - Python bindings: python3 -c \\"from opencog.atomspace import *; print(\\'AtomSpace loaded\\')\\""' >> /home/gitpod/.local/bin/run-demos && \
    chmod +x /home/gitpod/.local/bin/run-demos

RUN echo '#!/bin/bash' > /home/gitpod/.local/bin/guix-shell && \
    echo 'guix shell gcc-toolchain cmake pkg-config boost cppunit guile python python-cython' >> /home/gitpod/.local/bin/guix-shell && \
    chmod +x /home/gitpod/.local/bin/guix-shell

# Add local bin to PATH
RUN echo 'export PATH="$HOME/.local/bin:$PATH"' >> /home/gitpod/.bashrc

# Set up Guix profile
RUN echo 'export PATH="/var/guix/profiles/per-user/gitpod/current-guix/bin:$PATH"' >> /home/gitpod/.bashrc && \
    echo 'export GUIX_LOCPATH="$HOME/.guix-profile/lib/locale"' >> /home/gitpod/.bashrc

# Add helpful aliases for OpenCog development
RUN echo 'alias ll="ls -la"' >> /home/gitpod/.bashrc && \
    echo 'alias build-all="build-atomspace && build-cogserver"' >> /home/gitpod/.bashrc && \
    echo 'alias og="cd /workspace/opencog-org"' >> /home/gitpod/.bashrc && \
    echo 'alias ogb="cd /home/gitpod/opencog-build"' >> /home/gitpod/.bashrc

# Set up Git configuration for development
RUN git config --global core.editor "code --wait" && \
    git config --global init.defaultBranch main

# Create welcome message
RUN echo 'echo "🧠 Welcome to OpenCog GitPod Environment!"' >> /home/gitpod/.bashrc && \
    echo 'echo "Run: ~/setup-opencog.sh to complete setup"' >> /home/gitpod/.bashrc

# Expose common OpenCog ports
EXPOSE 17001 18001 5000 8080

# Set working directory
WORKDIR /workspace/opencog-org