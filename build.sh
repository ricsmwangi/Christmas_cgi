#!/bin/bash
set -e

echo "🎄 Checking for build tools..."

# Try to install build tools if needed (for Render environment)
if command -v apt-get &> /dev/null; then
    echo "Installing build-essential..."
    apt-get update -qq
    apt-get install -y build-essential
elif command -v brew &> /dev/null; then
    echo "Using existing brew..."
    brew install gcc || true
fi

echo "🎄 Building C programs..."
make clean
make

echo "✅ Build complete!"
ls -lah *.cgi
