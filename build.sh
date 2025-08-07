#!/bin/bash

# License Expiration Checker - macOS Build Script

set -e

echo "Building License Expiration Checker for macOS..."

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    mkdir build
fi

cd build

# Clean previous build
if [ "$1" = "clean" ]; then
    echo "Cleaning previous build..."
    rm -rf *
fi

# Configure with CMake
echo "Configuring build..."
cmake ..

# Build the project
echo "Building..."
make -j$(sysctl -n hw.ncpu)

echo "Build complete!"
echo "Executable: $(pwd)/license_checker"

# Test if build was successful
if [ -f "license_checker" ]; then
    echo ""
    echo "Testing build..."
    ./license_checker --help
    echo ""
    echo "Build successful! You can now run:"
    echo "  $(pwd)/license_checker <path>"
else
    echo "Build failed - executable not found"
    exit 1
fi
