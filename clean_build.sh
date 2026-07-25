#!/bin/bash
# FractalAura clean rebuild script
# Useful for starting fresh or troubleshooting build issues

set -e

echo "=== FractalAura Clean Rebuild ==="
echo ""

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

# Parse arguments
BUILD_TYPE="Release"
GENERATOR="Xcode"

while [[ $# -gt 0 ]]; do
    case $1 in
        --debug)
            BUILD_TYPE="Debug"
            shift
            ;;
        --ninja)
            GENERATOR="Ninja"
            shift
            ;;
        --help)
            echo "Usage: ./clean_build.sh [options]"
            echo "Options:"
            echo "  --debug       Build in Debug mode (default: Release)"
            echo "  --ninja       Use Ninja generator (default: Xcode)"
            exit 0
            ;;
        *)
            echo "Unknown option: $1"
            exit 1
            ;;
    esac
done

echo "Build Type: $BUILD_TYPE"
echo "Generator: $GENERATOR"
echo ""

# Remove old build
if [ -d "build" ]; then
    echo "Removing old build directory..."
    rm -rf build
    echo -e "${GREEN}✓ Old build removed${NC}"
fi

echo ""
echo "Ensuring JUCE submodule is initialized..."
if [ ! -d "JUCE/.git" ]; then
    git submodule update --init --recursive
    echo -e "${GREEN}✓ JUCE submodule initialized${NC}"
else
    echo -e "${GREEN}✓ JUCE submodule present${NC}"
fi

echo ""
echo "Creating build directory..."
mkdir -p build
cd build

echo ""
echo "Configuring with CMake..."
if [ "$GENERATOR" == "Ninja" ]; then
    cmake .. -G Ninja -DCMAKE_BUILD_TYPE=$BUILD_TYPE
else
    cmake .. -G Xcode -DCMAKE_BUILD_TYPE=$BUILD_TYPE
fi
echo -e "${GREEN}✓ Configuration complete${NC}"

echo ""
echo "Building plugin..."
cmake --build . --config $BUILD_TYPE
echo -e "${GREEN}✓ Build successful${NC}"

echo ""
echo -e "${GREEN}=== Build Complete ===${NC}"
echo ""
echo "Output locations:"
echo "  VST3: $(pwd)/FractalAura_artefacts/$BUILD_TYPE/VST3/FractalAura.vst3"
echo "  AU:   $(pwd)/FractalAura_artefacts/$BUILD_TYPE/AU/FractalAura.component"
echo ""
echo "To install VST3:"
echo "  cp -r FractalAura_artefacts/$BUILD_TYPE/VST3/FractalAura.vst3 /Library/Audio/Plug-Ins/VST3/"
echo ""
