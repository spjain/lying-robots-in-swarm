#!/usr/bin/env bash

# Stop execution after any error
set -e

# Create data dir if not already there
# mkdir -p data

for PARAM1 in $(seq 1 5 100)
do
  PARAM2=$[$PARAM1+4]
  sbatch experiments.sh $PARAM1 $PARAM2
  sleep 1
done
