FROM mcr.microsoft.com/devcontainers/cpp:2-ubuntu24.04

RUN apt update \
    && export DEBIAN_FRONTEND=noninteractive \
    && apt install -y --no-install-recommends \
        clangd-18 \
        clang-format-18 \
        clang-tidy-18 \
    && rm -rf /var/lib/apt/lists/*
