#!/bin/bash

rm -rf ./build
mkdir -p ./build && cd build
cmake .. -DCMAKE_BUILD_TYPE=release
make -j$(nproc)
