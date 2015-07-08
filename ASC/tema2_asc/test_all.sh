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
# Testeaza cele 3 implementari pe matrici cu dimensiuni intre(500, 30000) cu pasul 500
#
#

rm ./results/*

for INDEX in {500..30000..500}
do
	# pune in cozi un job pentru matrici cu dimensiune de $INDEX
	bash my_run.sh $INDEX
	
	# cat timp rezultatul apelului "qstat" nu este un sir vid, se asteapta
	line=$(qstat)
	
	# la fiecare 5 secunde se verifica valoarea intoarsa de apelul comenzii qstat
	while [ ${#line} -gt 0 ]; do
		sleep 5
		line=$(qstat)
	done
	echo "************"
done
