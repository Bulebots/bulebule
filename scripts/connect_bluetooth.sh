#!/bin/bash

address=$(hcitool scan | grep -i theseus | cut -f 2)
sudo rfcomm bind 0 $address
sudo cat /dev/rfcomm0
