#!/bin/bash

prog=$1
workers_count=$2
w=$3
h=$4
iterations_count=$5

$prog <<<"
START $w $h $workers_count
RUN $iterations_count
STOP
STATUS
QUIT
"

