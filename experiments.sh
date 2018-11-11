#!/usr/bin/env bash
#SBATCH -J liarsRun
#SBATCH -n 12
#SBATCH -N 1
#SBATCH -p short
#SBATCH --mem 8G

# Stop execution after any error
set -e
#good_fun="filter"
# bad_fun="constant"
cells_on_side="2"
pattern="6"
wrong_frequency="2"
wrong_pattern="3"
#noise_prob="0.1"
comm_period="1"
num_robots="10"
# num_liars="1"
comm_range="1"
# density="0.5"
trialLow=$1
trialHigh=$2
for noise_prob in 0.0 0.1 0.25 0.49
do
	for density in 0.5 1 2 3
	do
		for bad_fun in "constant" "sameConstant" "uniform" "periodic"
		do
			for good_fun in "basic" "cosine" "iou"
			do
				for num_liars in 10 20 30 40 50
				do
					mkdir -p data/good_"${good_fun}"/bad_"${bad_fun}"/side_"${cells_on_side}"/pattern_"${pattern}"/noise_"${noise_prob}"/comm_"${comm_period}"/num_robots_"${num_robots}"/num_lying_"${num_liars}"/range_"${comm_range}"/density_"${density}"/
					mkdir -p data_argos/good_"${good_fun}"/bad_"${bad_fun}"/side_"${cells_on_side}"/pattern_"${pattern}"/noise_"${noise_prob}"/comm_"${comm_period}"/num_robots_"${num_robots}"/num_lying_"${num_liars}"/range_"${comm_range}"/density_"${density}"/
					for trial in $(seq ${trialLow} 1 ${trialHigh})
					do
						sed -e "s|WRONG_PATTERN|${wrong_pattern}|" -e "s|WRONG_FREQUENCY|${wrong_frequency}|" -e "s|RUN|${trial}|" -e "s|GOOD_FUN|${good_fun}|" -e "s|BAD_FUN|${bad_fun}|" -e "s|SIDE|${cells_on_side}|" -e "s|PATTERN|${pattern}|" -e "s|NOISE|${noise_prob}|" -e "s|COMM_PERIOD|${comm_period}|" -e "s|NUM_ROBOTS|${num_robots}|" -e "s|NUM_LIARS|${num_liars}|" -e "s|COMM_RANGE|${comm_range}|" -e "s|DENSITY|${density}|" experiments/pattern_temp.argos > data_argos/good_"${good_fun}"/bad_"${bad_fun}"/side_"${cells_on_side}"/pattern_"${pattern}"/noise_"${noise_prob}"/comm_"${comm_period}"/num_robots_"${num_robots}"/num_lying_"${num_liars}"/range_"${comm_range}"/density_"${density}"/"${trial}".argos
  						argos3 -c data_argos/good_"${good_fun}"/bad_"${bad_fun}"/side_"${cells_on_side}"/pattern_"${pattern}"/noise_"${noise_prob}"/comm_"${comm_period}"/num_robots_"${num_robots}"/num_lying_"${num_liars}"/range_"${comm_range}"/density_"${density}"/"${trial}".argos &
					done
  				done
				sleep 60
  			done
  		done
	done
done
