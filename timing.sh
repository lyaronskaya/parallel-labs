#!/usr/bin/bash

for i in $@; do
    echo "Concurrency $i:"
    time bash run.sh main $i 256 256 10 >/dev/null
done

