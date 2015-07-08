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
# Script pentru testarea corectitudinii implementarilor.
#
# Trimite 4 cereri catre coada opteron pentru matrici de dimensiune 5000, 10000, 15000, 20000.

rm ./results/*

for INDEX in {5000..20000..5000}
do
	# trimit cerere catre opteron/quad/nehalem
	bash my_run_debug.sh $INDEX
	
	# cat timp qstat nu imi intoarce un sir vid astept
	line=$(qstat)
	
	# verific valoarea lui qstat la fiecare 5 secunde
	while [ ${#line} -gt 0 ]; do
		sleep 5
		line=$(qstat)
	done
	echo "************"
	
done
