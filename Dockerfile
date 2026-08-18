# Dockerfile for visualization.matrix build environment
# Multi-stage build for smaller final image
# Optimized for CI/CD and production use

# ============================================
# Stage 1: Build stage
# ============================================
FROM ubuntu:22.04 AS builder

# Set environment variables
ENV DEBIAN_FRONTEND=noninteractive \
    CMAKE_BUILD_PARALLEL_LEVEL=$(nproc)

# Install minimal build dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    cmake \
    g++ \
    make \
    git \
    libgl1-mesa-dev \
    libglu1-mesa-dev \
    ninja-build \
    && rm -rf /var/lib/apt/lists/*

# Clone Kodi source (required for building addons)
WORKDIR /workspace
RUN git clone --depth=1 --branch Matrix https://github.com/xbmc/xbmc.git /kodi

# Copy source code
COPY . .

# Build the addon with optimized flags
RUN mkdir -p build && \
    cd build && \
    cmake \
      -G Ninja \
      -DCMAKE_BUILD_TYPE=Release \
      -DADDONS_TO_BUILD=visualization.matrix \
      -DADDON_SRC_PREFIX=/workspace \
      -DADDONS_DEFINITION_DIR=/workspace/build/definition \
      -DCMAKE_INSTALL_PREFIX=/workspace/kodi/addons \
      -DPACKAGE_ZIP=1 \
      -DCMAKE_CXX_FLAGS="-O3 -DNDEBUG" \
      -DCMAKE_CXX_FLAGS_RELEASE="-O3 -DNDEBUG" \
      /kodi/cmake/addons && \
    ninja -j$(nproc)

# ============================================
# Stage 2: Runtime stage
# ============================================
FROM ubuntu:22.04 AS runtime

# Install minimal runtime dependencies
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
    libgl1-mesa-glx \
    libglu1-mesa \
    ca-certificates \
    && rm -rf /var/lib/apt/lists/*

# Copy built addon from builder stage
COPY --from=builder /workspace/kodi/addons /addons

# Set working directory
WORKDIR /addons

# Create non-root user for security
RUN useradd -m -u 1000 kodi && \
    chown -R kodi:kodi /addons

# Switch to non-root user
USER kodi

# Default command (can be overridden)
CMD ["echo", "visualization.matrix addon built successfully. Files are in /addons"]

# ============================================
# Multi-arch build support
# ============================================
# To build for different architectures, use:
# docker build --platform linux/amd64 -t visualization.matrix:amd64 .
# docker build --platform linux/arm64 -t visualization.matrix:arm64 .
# docker build --platform linux/arm/v7 -t visualization.matrix:armv7 .

# ============================================
# Build arguments for customization
# ============================================
ARG KODI_BRANCH=Matrix
ARG BUILD_TYPE=Release
