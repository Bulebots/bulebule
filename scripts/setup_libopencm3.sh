#!/bin/bash
root=$(dirname $0)/../

# Initialize submodules
git submodule update --init --recursive

pushd $root/.opencm3/

# Download libopencm3 basic make rules
wget https://raw.githubusercontent.com/libopencm3/libopencm3-examples/master/examples/rules.mk -O libopencm3.rules.mk

# Compile libopencm3
cd libopencm3
make

popd
