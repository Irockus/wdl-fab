#!/bin/bash
find . -type d  \( -name '*.dir' -o -name '*.build' -o -name 'CMake*' -o -name 'x64*' -o -name 'Debug' -o -name 'Release' \) -exec rm -rf "{} " 2>&1 >/dev/null \;
