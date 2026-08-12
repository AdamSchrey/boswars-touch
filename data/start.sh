#!/bin/bash

# Set working directory to the data directory
cd "$(dirname "$0")"

# Export LD_LIBRARY_PATH to find the included libraries
export LD_LIBRARY_PATH="${LD_LIBRARY_PATH:+$LD_LIBRARY_PATH:}$(dirname "$0")/../lib/${ARCH_TRIPLET}"

# Run the game
./boswars

exit 0
