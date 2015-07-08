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
# Acesta este makefile-ul

CC=gcc
LOCAL=/opt/tools/libraries/atlas/
MACHINEQ=3.10.1-quad-gcc-4.4.6/
MACHINEO=3.10.1-opteron-gcc-4.4.6/
MACHINEN=3.10.1-nehalem-gcc-4.4.6/

# cale catre include / lib
OPT_LIB=$(LOCAL)$(MACHINEO)lib
OPT_INC=$(LOCAL)$(MACHINEO)include
QUAD_LIB=$(LOCAL)$(MACHINEQ)lib
QUAD_INC=$(LOCAL)$(MACHINEQ)include
NEH_LIB=$(LOCAL)$(MACHINEN)lib
NEH_INC=$(LOCAL)$(MACHINEN)include

# flag compilare pentru cazul optimizat
CFLAGS_gcc=-O5

# reguli de compilare ale celor 3 executabile (naive, blas si optimized) pe fiecare din cele 3 cozi
compile-Q: utils.o
	$(CC) blas_call.c utils.o -o blas-Q -I $(QUAD_INC) -L $(QUAD_LIB) -lcblas -latlas
	$(CC) naive.c utils.o -o naive-Q
	$(CC) $(CFLAGS_gcc) optimized.c utils.o -o optimized-Q
compile-N: utils.o
	$(CC) blas_call.c utils.o -o blas-N -I $(NEH_INC) -L $(NEH_LIB) -lcblas -latlas
	$(CC) naive.c utils.o -o naive-N
	$(CC) $(CFLAGS_gcc) optimized.c utils.o -o optimized-N
compile-O: utils.o
	$(CC) blas_call.c utils.o -o blas-O -I $(OPT_INC) -L $(OPT_LIB) -lcblas -latlas
	$(CC) naive.c utils.o -o naive-O
	$(CC) $(CFLAGS_gcc) optimized.c utils.o -o optimized-O


# creare executabile naive / optimized (folosit pentru testare locala)
naive-compile: utils.o
	$(CC) naive.c utils.o -o naive
opt-compile: utils.o
	$(CC) $(CFLAGS_gcc) optimized.c utils.o -o optimized

util:
	$(CC) -c utils.c


run:
	./naive



clean:
	rm -Rf naive
	rm blas-*
	rm naive-*
	rm optimized-*
	rm utils.o
