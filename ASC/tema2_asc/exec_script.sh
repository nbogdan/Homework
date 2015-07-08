#!/bin/bash
#
#
# Tema 2 ASC
#
#	NICULA Bogdan Daniel
#	Grupa 334CB
#
#	Modul: principal
#
# Acesta este scriptul care va fi executat ca job pe fiecare din cele 3 cozi.
# El compileaza sursele si ruleaza cele 3 executabile(blas, naiv si optimizat), 
# redirectand rezultatele in niste fisiere speciale.

RESULTS=./results


[[ -z $COMPILER ]] && COMPILER="gcc"


if [[ $COMPILER="gcc" ]]; then

	# daca sunt pe coada potrivita
	if [[ $1 == "OPTERON" ]]; then
		# scriu in fisierul *-step dimensiunea matricei pentru care se analizeaza implementarile
		echo $2 >> $RESULTS/opt-step
		echo "OPTERON" 
		echo "-------------------------"
		echo "blas"
		# creez cele 3 executabile
		make compile-O
		# rulez cele 3 executabile redirectand outputul in fiserele necesare
   		./blas-O $2 up >> $RESULTS/opt-blas
		echo "naive"
		./naive-O $2 up >> $RESULTS/opt-naive
		echo "opt"
		./optimized-O $2 up >> $RESULTS/opt-opt
	fi
	
	if [[ $1 == "NEHALEM" ]]; then
		echo $2 >> $RESULTS/neh-step
		echo "NEHALEM"
		echo "-------------------------"
		echo "blas"
		make compile-N
   		./blas-N $2 up >> $RESULTS/neh-blas
		echo "naive"
		./naive-N $2 up >> $RESULTS/neh-naive
		echo "opt"
		./optimized-N $2 up >> $RESULTS/neh-opt
   		
	fi
	if [[ $1 == "QUAD" ]]; then
		echo $2 >> $RESULTS/quad-step
		echo "QUAD"
		echo "-------------------------"
		echo "blas"
		make compile-Q
   		./blas-Q $2 up >> $RESULTS/quad-blas
		echo "naive"
		./naive-Q $2 up >> $RESULTS/quad-naive
		echo "opt"
		./optimized-Q $2 up >> $RESULTS/quad-opt
	fi
	
fi
