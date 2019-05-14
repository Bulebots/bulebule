#!/bin/bash

cd ..

cppcheck --enable=all src/ -isrc/simulation -isrc/printf -Isrc/mmlib -Isrc --check-config --suppress=missingIncludeSystem --quiet --error-exitcode=1

cppcheck --enable=all src/ -isrc/simulation -isrc/printf -Isrc/mmlib -Isrc --template=gcc --suppress=unusedFunction  --suppress=missingInclude --quiet --error-exitcode=1
