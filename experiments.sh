#!/usr/bin/env bash
#SBATCH -J liarsRun
#SBATCH -n 8
#SBATCH -N 1
#SBATCH -p short
#SBATCH --mem 24G
#SBATCH -C E5-2695

# Stop execution after any error
set -e
good_fun="filter"
bad_fun="constant"
cells_on_side="2"
pattern="6"
#noise_prob="0.1"
comm_period="1"
num_robots="10"
# num_liars="1"
# comm_range="1"
# density="0.5"
trialLow=$1
trialHigh=$2
for noise_prob in 0.0 0.1 0.25 0.49
do
	for num_liars in $(seq 1 1 5)
	do
		for comm_range in $(seq 1 1 3)
		do
			for density in $(seq 0.5 0.5 2)
			do
				mkdir -p data/good_"${good_fun}"/bad_"${bad_fun}"/side_"${cells_on_side}"/pattern_"${pattern}"/noise_"${noise_prob}"/comm_"${comm_period}"/num_robots_"${num_robots}"/num_lying_"${num_liars}"/range_"${comm_range}"/density_"${density}"/
				for trial in $(seq ${trialLow} 1 ${trialHigh})
				do
					sed -e "s|RUN|${trial}|" -e "s|GOOD_FUN|${good_fun}|" -e "s|BAD_FUN|${bad_fun}|" -e "s|SIDE|${cells_on_side}|" -e "s|PATTERN|${pattern}|" -e "s|NOISE|${noise_prob}|" -e "s|COMM_PERIOD|${comm_period}|" -e "s|NUM_ROBOTS|${num_robots}|" -e "s|NUM_LIARS|${num_liars}|" -e "s|COMM_RANGE|${comm_range}|" -e "s|DENSITY|${density}|" experiments/pattern_temp.argos > experiments/pattern_run_"${trialLow}".argos
  					argos3 -c experiments/pattern_run_"${trialLow}".argos
  				done
  			done
  		done
	done
done
