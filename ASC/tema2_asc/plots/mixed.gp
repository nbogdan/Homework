set term png
set output "mixed.png"
set grid
set style line 1 lt 1 lw 2 pt 1 ps 0.5 lc rgb "red"
set style line 2 lt 1 lw 2 pt 1 ps 0.5 lc rgb "blue"
set style line 3 lt 1 lw 2 pt 1 ps 0.5 lc rgb "green"
set title "Rezultate Nehalem vs Opteron vs Quad"

set yrange [0:4]
set xlabel "Dimensiune matrice(N)"
set ylabel "Durata executie(sec)"
plot  "< paste ./neh-step ./neh-raw" using 1:2 title "neh naive" with linespoints ls 1, \
"< paste ./neh-step ./opt-raw" using 1:2 title "opteron naive" with linespoints ls 2, \
"<paste ./neh-step ./quad-raw" using 1:2 title "quad naive" with linespoints ls 3
