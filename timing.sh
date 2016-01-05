#!/usr/bin/bash

for i in $@; do
    echo "Workers count $i:"
    time bash run.sh main $i 256 256 10 >/dev/null
done
