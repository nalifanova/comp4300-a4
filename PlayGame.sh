#!/bin/bash

export PATH=$PATH:`dirname $(which cmake)`
# create directories and move it there
mkdir -p GAME17 && cd GAME17

# Run cmake and wait for the result
if cmake ..; then
    if make; then # Happy case with build
        ./bin/Game17
    else
        echo "Something went wrong with the build process."
    fi
else
    echo "Error: CMake failed."
fi
