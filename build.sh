#!/bin/bash
# FractalAura macOS build script

set -e

echo "=== FractalAura VST3 Builder ==="
echo ""

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Check for required tools
echo "Checking dependencies..."
if ! command -v cmake &> /dev/null; then
    echo -e "${RED}Error: CMake not found. Install with: brew install cmake${NC}"
    exit 1
fi

if ! command -v git &> /dev/null; then
    echo -e "${RED}Error: Git not found${NC}"
    exit 1
fi

echo -e "${GREEN}✓ CMake and Git found${NC}"

# Initialize submodules if needed
echo ""
echo "Initializing JUCE submodule..."
if [ ! -d "JUCE/.git" ]; then
    git submodule update --init --recursive
    echo -e "${GREEN}✓ JUCE submodule initialized${NC}"
else
    echo -e "${GREEN}✓ JUCE submodule already present${NC}"
fi

# Create build directory
echo ""
echo "Setting up build directory..."
if [ ! -d "build" ]; then
    mkdir build
    echo -e "${GREEN}✓ Created build directory${NC}"
else
    echo -e "${YELLOW}ℹ build directory already exists${NC}"
fi

cd build

# Configure with CMake
echo ""
echo "Configuring with CMake..."
cmake .. -G Xcode -DCMAKE_BUILD_TYPE=Release
echo -e "${GREEN}✓ CMake configuration complete${NC}"

# Build
echo ""
echo "Building plugin..."
cmake --build . --config Release
echo -e "${GREEN}✓ Build complete${NC}"

# Report output locations
echo ""
echo -e "${GREEN}=== Build Successful ===${NC}"
echo ""
echo "VST3 Plugin:"
echo "  $(pwd)/FractalAura_artefacts/Release/VST3/FractalAura.vst3"
echo ""
echo "AU Plugin:"
echo "  $(pwd)/FractalAura_artefacts/Release/AU/FractalAura.component"
echo ""
echo "To install VST3:"
echo "  cp -r FractalAura_artefacts/Release/VST3/FractalAura.vst3 /Library/Audio/Plug-Ins/VST3/"
echo ""
echo "To install AU:"
echo "  cp -r FractalAura_artefacts/Release/AU/FractalAura.component /Library/Audio/Plug-Ins/Components/"
echo ""
