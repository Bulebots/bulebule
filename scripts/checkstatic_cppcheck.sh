#!/bin/bash

cd ..

cppcheck --enable=all src/ -isrc/simulation -isrc/printf -I src/mmlib -I src --check-config --suppress=missingIncludeSystem --quiet --error-exitcode=1 

cppcheck --enable=all src/ -isrc/simulation -isrc/printf -I src/mmlib -I src --template=gcc --suppress=unusedFunction  --suppress=missingInclude --quiet --error-exitcode=1




