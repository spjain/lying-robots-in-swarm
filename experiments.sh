i="1"
algo="Algo2_average_belief_limited_comm"
j="1"
k="1"
mkdir data/"${algo}"
while [ $k -le 4 ]
do
	mkdir data/"${algo}"/num_lying_"${k}"
	sed -e "s|LIARS|${k}|" buzz/"${algo}".bzz1 > buzz/"${algo}".bzz
	cd build
	make
	cd ..
	j="1"
	while [ $j -le 1 ]
	do
		mkdir data/"${algo}"/num_lying_"${k}"/"${j}"
		i="1"
		while [ $i -le 20 ]
		do
  			sed -e "s|RUN|${i}|" -e "s|COMMRANGE|${j}|" -e "s|LIARS|${k}|" -e "s/ALGO/${algo}/g" experiments/foraging_temp.argos > experiments/foraging_run.argos
  			argos3 -c experiments/foraging_run.argos
  			i=$[$i+1]
		done
		j=$[$j+1]
	done
	k=$[$k+1]
done
