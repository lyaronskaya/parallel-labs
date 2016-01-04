#!/bin/bash

prog=$1
concurrency=$2
h=$3
w=$4
runs=$5

$prog <<<"
START $h $w $concurrency
RUN $runs
STOP
STATUS
QUIT
"

