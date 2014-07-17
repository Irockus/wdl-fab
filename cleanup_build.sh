#!/bin/bash
find . -type d  \( -name '*.dir' -o -name '*.build' -o -name 'CMake*' -o -name 'x64*' -o -name 'Debug' -o -name 'Release' -o -iname 'PCH' \) -exec rm -rf "{}" \;
find . -type f \( -iname 'cmake_install.*' -o -iname 'CMakeCache*' -o -name '*~' \) -exec rm "{}" \;
rm -rf ./bin ./lib build/osx build/win