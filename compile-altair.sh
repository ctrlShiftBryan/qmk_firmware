#!/bin/bash

# Altair QMK Firmware Compilation Script
# Usage: ./compile-altair.sh

set -e  # Exit on error

echo "🔨 Compiling Altair Firmware (bryan keymap)..."
echo ""

# Change to QMK firmware directory
cd "$(dirname "$0")"

# Compile the firmware
qmk compile -kb ai03/altair -km bryan

# Check if compilation was successful
if [ $? -eq 0 ]; then
    echo ""
    echo "✅ Compilation successful!"
    echo ""
    echo "📁 Firmware location:"
    echo "   $(pwd)/ai03_altair_bryan.uf2"
    echo ""
    echo "📋 To flash:"
    echo "   1. Put keyboard in bootloader mode (bootmagic or reset key)"
    echo "   2. Drag ai03_altair_bryan.uf2 to the RPI-RP2 drive"
    echo ""

    # Show file info
    ls -lh ai03_altair_bryan.uf2 | awk '{print "   Size: " $5 "  Modified: " $6 " " $7 " " $8}'

    # Open Finder to the firmware file
    echo ""
    read -p "Open firmware in Finder? (y/n) " -n 1 -r
    echo
    if [[ $REPLY =~ ^[Yy]$ ]]; then
        open -R ai03_altair_bryan.uf2
    fi
else
    echo ""
    echo "❌ Compilation failed!"
    echo "   Check the errors above"
    exit 1
fi
