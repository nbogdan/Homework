#				Tema 4 ASC
#
#	Student: 	NICULA Bogdan
#	Grupa: 		334CB
#
#
#	Script pentru generarea plotului shared vs no_shared
#


set term png
set output "gpu_comparison.png"
set grid
set style line 1 lt 1 lw 2 pt 1 ps 0.5 lc rgb "red"
set style line 2 lt 1 lw 2 pt 1 ps 0.5 lc rgb "blue"
set title "Comparatie gpu no-shared vs gpu shared"

set xlabel "Dimensiune problema(nr pixeli calculati)"
set ylabel "Durata executie(msec)"
plot  "< paste ./problem_size_step ./gpu_no_shared" using 1:2 title "gpu no-shared" with linespoints ls 1, \
"<paste ./problem_size_step ./gpu_shared" using 1:2 title "gpu shared" with linespoints ls 2






