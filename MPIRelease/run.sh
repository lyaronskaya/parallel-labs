#!/bin/bash

prog=$1
concurrency=$2
h=$3
w=$4
runs=$5

# echo $prog
# echo $concurrency
# echo $h
# echo $w
# echo $runs

$prog <<<"
start $concurrency $h $w
run $runs
block
status
quit
"

