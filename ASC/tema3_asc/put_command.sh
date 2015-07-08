#	Tema 3 ASC
#
#	Student:	NICULA Bogdan
#	Grupa:		334CB

qsub -cwd -q ibm-cell-qs22.q -pe openmpi*12 12 -e ./error -o ./output -b n testing.sh