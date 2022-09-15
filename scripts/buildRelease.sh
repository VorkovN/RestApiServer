#!/bin/bash

cd ..
rm -rf build bin
mkdir build
cd build
cmake -E make_directory build
cmake -DCMAKE_BUILD_TYPE=Release ..
cmake --build . --target install
cd ..
rm -rf build