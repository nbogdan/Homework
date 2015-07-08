#!/bin/bash
#
#
# Tema 2 ASC
#
#	NICULA Bogdan Daniel
#	Grupa 334CB
#
#	Modul: debugging
#
#
# Job folosit pentru "debugging". Va fi executat DOAR pe coada opteron.
# Ruleaza executabilele in modul de debugging(acestea afiseaza la STDOUT vectorul rezultat),
# apoi salveaza rezultatul in niste fisiere speciale.


RESULTS=./correctitude_test/results


[[ -z $COMPILER ]] && COMPILER="gcc"

cd ..
if [[ $COMPILER="gcc" ]]; then
	if [[ $1 == "OPTERON" ]]; then
		echo $2 >> $RESULTS/opt-step
		echo "OPTERON" 
		echo "-------------------------"
		# creez cele 3 executabile, specificand dimensiunea matricei($2)
		make compile-O
		echo "blas"
		# testez implementarea atlas blas
   		./blas-O $2 up debug >> $RESULTS/opt-blas
		echo "naive"
		# testez implementarea "naiva"
		./naive-O $2 up debug >> $RESULTS/opt-naive
		echo "opt"
		# testez implementarea optimizata
		./optimized-O $2 up debug >> $RESULTS/opt-opt
	fi
	
fi
cd "correctitude_test"
