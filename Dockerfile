# Dockerfile for visualization.matrix build environment
# Multi-stage build for smaller final image

# ============================================
# Stage 1: Build stage
# ============================================
FROM ubuntu:22.04 AS builder

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    make \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    ninja-build \
    clang \
    lld \
    clang-tidy-14 \
    cppcheck \
    && rm -rf /var/lib/apt/lists/*

# Clone Kodi source (required for building addons)
RUN git clone --depth=1 --branch Matrix https://github.com/xbmc/xbmc.git /kodi

# Copy source code
WORKDIR /workspace
COPY . .

# Build the addon
RUN mkdir -p build && \
    cd build && \
    cmake \
      -DCMAKE_BUILD_TYPE=Release \
      -DADDONS_TO_BUILD=visualization.matrix \
      -DADDON_SRC_PREFIX=/workspace \
      -DADDONS_DEFINITION_DIR=/workspace/build/definition \
      -DCMAKE_INSTALL_PREFIX=/workspace/kodi/addons \
      -DPACKAGE_ZIP=1 \
      /kodi/cmake/addons && \
    make -j$(nproc)

# ============================================
# Stage 2: Runtime stage
# ============================================
FROM ubuntu:22.04 AS runtime

# Install runtime dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libgl1-mesa-glx \
    libglu1-mesa \
    && rm -rf /var/lib/apt/lists/*

# Copy built addon from builder stage
COPY --from=builder /workspace/kodi/addons /addons

# Set working directory
WORKDIR /addons

# Default command (can be overridden)
CMD ["echo", "visualization.matrix addon built successfully. Files are in /addons"]

# ============================================
# Multi-arch build support
# ============================================
# To build for different architectures, use:
# docker build --platform linux/amd64 -t visualization.matrix:amd64 .
# docker build --platform linux/arm64 -t visualization.matrix:arm64 .
# docker build --platform linux/arm/v7 -t visualization.matrix:armv7 .
