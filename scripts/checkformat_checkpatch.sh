#!/bin/bash
linux="https://git.kernel.org/pub/scm/linux/kernel/git/torvalds/linux.git"
wget --no-clobber --quiet $linux/plain/scripts/spelling.txt
wget --no-clobber --quiet $linux/plain/scripts/checkpatch.pl
wget --no-clobber --quiet $linux/plain/scripts/const_structs.checkpatch
chmod +x checkpatch.pl
./checkpatch.pl --terse --no-tree --max-line-length=80 --ignore ARRAY_SIZE,LEADING_SPACE,SPLIT_STRING,USE_NEGATIVE_ERRNO,VOLATILE --file src/*.[ch]
