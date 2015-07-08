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
# Script de submitere a job-urilor de testare pe coada OPTERON .
# Folosit pentru debugging.


echo "*********************************"
echo "Testing for matrix size: " $1
echo "&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&"
qsub -cwd -q ibm-opteron.q -R y -e ./error -o ./output -b n debug_script.sh "OPTERON" $1
