#!/bin/bash
#				Tema 4 ASC
#
#	Student: 	NICULA Bogdan
#	Grupa: 		334CB
#
#
#
#	Script care prelucreaza output-ul obtinut si genereaza
#	2 plot-uri(shared vs no-shared si cpu vs gpu) pentru datele obtinute
#
#



#prelucrez rezultatele
cat output | grep "kernel:" | awk '{ print $4}' > ./plot/gpu_no_shared
cat output | grep "partajata:" | awk 'NR % 2 == 0' |  awk '{ print $7}' > ./plot/gpu_shared
cat output | grep "seriala:" | awk '{ print $4}' > ./plot/cpu


rm ./plot/problem_size_step
for index in {1..20}
do
	file='./tests/test_'$index'.txt'

	m=$((`cat $file | awk '{ print $1}'`))
	n=$((`cat $file | awk '{ print $2}'`))

	result=$(($m*$n))

	#echo $file	
	echo $result >> ./plot/problem_size_step

done

#generez ploturi (ATENTIE, utilitarul gnuplot trebuie instalat inainte.)
cd plot
gnuplot ./plot.gp
gnuplot ./plot2.gp #verificati sa aveti instalat gnuplot
cd ..