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
# Testeaza cele trei implementari(naiva, optimizata si atlas blas) pentru matrici de dimensiune 
# 5000, 10000, 15000, 20000, salvand output-ul in 3 fisiere.(Testarea se face doar pe coada opteron)
# Apoi verifica diferentele intre cele 3 fisiere de output
# 

#testare
cd ./correctitude_test
bash test_correct.sh
#comparare rezultate
cd results
val1=$(diff opt-opt opt-naive)
val2=$(diff opt-blas opt-opt)

errors=$((${#val1}+${#val2}))

echo $errors " errors found." 

cd ../..
