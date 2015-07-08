#!/bin/bash
#
#
# Tema 2 ASC
#
#	NICULA Bogdan Daniel
#	Grupa 334CB
#
#	Modul: plotting
#
# Acest modul se ocupa de desenarea plot-urilor


cp ./plots/* ./results
cd results

gnuplot plot_quad.gp
gnuplot plot_nehalem.gp
gnuplot plot_opteron.gp
gnuplot mixed.gp

rm *.gp
cd ..