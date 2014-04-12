#!/bin/bash
find . \( -name '*.cpp' \
    -o -name '*.h' \
    -o -name '*.c' \
    -o -name '*.mm' \)  \
    -exec astyle --style=allman -s2 -o -O -S "{}" \;