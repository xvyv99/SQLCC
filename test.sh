#!/bin/sh
cmake -S . -B build && cmake --build build && cd build && ctest --rerun-failed --output-on-failure 
