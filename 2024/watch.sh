#!/usr/bin/env bash

# Requires `entr` to be installed

SCRIPT_ROOT=$(pwd)
DAY=$1

if [ $# -eq 0 ] || [ $# -gt 2 ]; then
  echo "Expected 1 or 2 arguments"
  echo "$0 <DAY>"
  echo "$0 test <DAY>"
  exit 1
fi

if [ $# -eq 2 ] && [ $2 -eq 'test' ]; then
  DAY=$1
fi

# Run once before watching to make sure the code file has been created.
$SCRIPT_ROOT/run.sh $DAY $2
ls $SCRIPT_ROOT/run.sh $SCRIPT_ROOT/src/${DAY}.cpp |
    entr $SCRIPT_ROOT/run.sh $DAY $2
