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
# Script de submitere a job-urilor de testare pe cozile OPTERON/NEHALEM/QUAD
# Primeste ca parametru($1) dimensiunea matricei pe care se va testa, si 
# o paseaza mai departe scriptului.
#



echo "*********************************"
echo "Testing for matrix size: " $1
echo "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
qsub -cwd -q ibm-nehalem.q -R y -e ./error -o ./output -b n exec_script.sh "NEHALEM" $1
qsub -cwd -q ibm-opteron.q -R y -e ./error -o ./output -b n exec_script.sh "OPTERON" $1
qsub -cwd -q ibm-quad.q -R y -e ./error -o ./outpu  -b n exec_script.sh "QUAD" $1
