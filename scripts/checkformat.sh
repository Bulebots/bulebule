#!/bin/bash
root="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
$root/checkformat_clang.sh
$root/checkformat_checkpatch.sh
