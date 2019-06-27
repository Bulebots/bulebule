#!/bin/bash
opencm3=$(dirname $0)/../src/opencm3/

# Initialize submodules
git submodule update --init --recursive

# Download libopencm3 basic make rules
wget https://raw.githubusercontent.com/libopencm3/libopencm3-examples/master/examples/rules.mk \
	-O ${opencm3}libopencm3.rules.mk

# Compile libopencm3
make -C ${opencm3}libopencm3
