set term png
set output "quad.png"
set grid
set style line 1 lt 1 lw 2 pt 1 ps 0.5 lc rgb "red"
set style line 2 lt 1 lw 2 pt 1 ps 0.5 lc rgb "blue"
set style line 3 lt 1 lw 2 pt 1 ps 0.5 lc rgb "green"
set title "Rezultate Quad"

set yrange [0:4]
set xlabel "Dimensiune matrice(N)"
set ylabel "Durata executie(sec)"
plot  "< paste ./quad-step ./quad-blas" using 1:2 title "atlas blas" with linespoints ls 1, \
"< paste ./quad-step ./quad-opt" using 1:2 title "optimized" with linespoints ls 2, \
"<paste ./quad-step ./quad-raw" using 1:2 title "simple" with linespoints ls 3
