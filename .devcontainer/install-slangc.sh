#!/bin/sh
set -eu

VERSION="2026.8.1"
ARCHIVE="slang-${VERSION}-linux-aarch64.tar.gz"
URL="https://github.com/shader-slang/slang/releases/download/v${VERSION}/${ARCHIVE}"

INSTALL_DIR="/opt/slang-${VERSION}"

curl -fsSL "${URL}" -o "/tmp/${ARCHIVE}"

mkdir -p "${INSTALL_DIR}"
tar -xzf "/tmp/${ARCHIVE}" -C "${INSTALL_DIR}" --strip-components=1

ln -sfn "${INSTALL_DIR}/slangc" "/usr/local/bin/slangc"

rm "/tmp/${ARCHIVE}"
