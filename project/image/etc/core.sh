#!/bin/sh
/bin/gzip -1 > /appconfigs/coredump.process_$1.gz
sync
