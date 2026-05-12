FROM mcr.microsoft.com/devcontainers/cpp:2-ubuntu24.04

RUN apt update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt install -y --no-install-recommends \
        clangd-18 \
        clang-format-18 \
        clang-tidy-18 \
    && apt install -y \
        libvulkan-dev \
        vulkan-validationlayers \
    && rm -rf /var/lib/apt/lists/*

COPY install-slangc.sh /tmp/install-slangc.sh
RUN chmod +x /tmp/install-slangc.sh \
    && /tmp/install-slangc.sh \
    && rm /tmp/install-slangc.sh
