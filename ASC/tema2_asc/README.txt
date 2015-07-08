# Tema 2 ASC
#
#	NICULA Bogdan Daniel
#	Grupa 334CB
#
#


===================================================================================================
0.) INFORMATII GENERALE
===================================================================================================

-------------------------------------------------
a. Observatii
-------------------------------------------------

-> Urmatorii termeni vor fi folositi avand intelesul:
-coada/masina: arhitectura pe care se testeaza(Opteron, Quad, Nehalem)


-> Pentru explicatiile necesare subpunctului 5, sariti la capitolul 3.(linia 218)

-> Dimensiunea matricei A este mereu de NxN, iar a vectorului x de N.

-> Matricea si vectorul sunt generati random la executie folosind seed-ul 0.
Dimensiunea lor este specificata de primul parametru in linie de comanda primit de executabile


-------------------------------------------------
b. Continutul arhivei
-------------------------------------------------
.
├── blas_call.c 			Modulul in care este apelata implementarea ATLAS BLAS
├── check_corectitude.sh	Script pentru verificarea corectitudinii implementarilor
├── correctitude_test		Modul in care se gasesc scripturile folosite in cadrul lui "chack_corectitude"	
│   ├── debug_script.sh	
│   ├── my_run_debug.sh
│   |── results
│   └── test_correct.sh
|-- draw_plot.sh			Script de generare a plot-urilor
├── exec_script.sh			Script-ul executat de un job pus intr-o coada(nehalem/opteron/quad)
├── Makefile				Makefile-ul folosit
├── my_run.sh				Script care pune pe fiecare coada cate un job, primind dimensiunea matricei si a vectorului
|							ca parametru($1)
├── naive.c 				Implementarea naiva a inmultirii unei matrici cu un vector
├── optimized.c 			Implementarea optimizata.
|── plots					Folder continand script-urile de plot
├── results 				Rezultate rulare( vor fi folosite pentru plotare )
├── test_all.sh 			Script-ul care apeleaza "my_run.sh" pentru dimensiuni diferite de matrici(intre 500 si 30000)
|── tests					Folder continand ploturile testele care au stat la baza acelor plot-uri
├── utils.c 				Functie generare matrice
└── utils.h

-------------------------------------------------
c. Target-uri makefile
-------------------------------------------------
compile-Q	|
compile-N	|=Creeaza cele 3 executabile(naive, optimized, blas) folosite pe cele 3 cozi.
compile-O 	|		(-Q pt Quad, -N pt Nehalem, -O pt Opteron)

run 		Ruleaza script-ul test_all.sh, testand cele 3 implementari pe fiecare din cele 3 cozi 
			pentru matrici de dimensiune N= 500 pana la 30000 (pasul fiind de 1000)

draw-plot 	Porneste de la presupunerea ca inainte s-a rulat "make run". Genereaza 4 plot-uri, 
			3 prezentand comparativ cele 3 metode de rezolvare la nivelul fiecarei cozi, 
			si unul comparand timpii pentru implementarea naiva pe cele 3 cozi

debug 		Ruleaza cele 3 executabile in mod "Debug"(la stdout se afiseaza vectorul rezultat) 
			pentru matrici de dimensiuni 5000, 10000, 15000, 20000. Acestea mai apoi se compara cu un diff 
			afisandu-se numarul de diferente gasite intre fisiere.


===================================================================================================
2.) IMPLEMENTAREA INMULTIRII
===================================================================================================

Am implementat inmultirea intre o matrice de dimensiune n*n si un vector(coloana) de dimensiune n pentru cazurile:
-matrice superior/inferior triunghiulara
-folosire matrice transpusa, sau matrice originala

-------------------------------------------------
a. Implementarea naiva (modulul naive.c; functia "simpleMultiply()")
-------------------------------------------------
Functia primeste ca parametrii matricea, vectorul si 2 flag-uri indicand daca este superior/inferior triunghiulara,
respectiv daca se foloseste transpusa sau matricea primita.

Implementarea este cea mai simpla cu putinta. Singura optimizare consta in faptul ca sunt parcurse doar elementele
despre care se stie ca pot fi nenule("triunghiul superior" in cazul unei matrici superior triunghiulare)

-------------------------------------------------
b. Implementarea optimizata (modulul optimized.c; functia "optimized_multiply()")
-------------------------------------------------
Functia primeste aceeasi parametrii ca functia pentru implementarea naiva.

Optimizarile constau in:
	-folosirea parcurgerii cu pointeri pentru a reduce numarul de calcule(calculul indicilor)
	-acumularea sumelor intr-o variabila "register"
	-compilare folosind flag-ul O5

-------------------------------------------------
c. Generarea matricei si a vectorului
-------------------------------------------------
Acestea sunt generate random folosind functia populateStructure() din modulul utils.c

Valorile generate sunt intre 0 si 1(maximum 6 zecimale) si se genereaza cu un seed
constant(0), prin urmare vom avea mereu aceeasi matrice pt o dimensiune N data.


-------------------------------------------------
d. Executabile
-------------------------------------------------
Pentru fiecare din cele 3 cazuri: naiv, optimizat si folosire dtrmv atlas blas se genereaza un executabil separat.

acestea pot fi apelate astfel:

./executabil $N up => intoarce timpul necesar pentru a inmultii o matrice superior triunghiulara
					cu un vector
./executabil $N down => idem pentru matrice inferior triunghiulara

./executabil $N up debug => intoarce la stdout vectorul obtinut in urma inmultirii.(apel folosit pt verificarea
							corectitudinii implementarilor).



Dimensiunea matricei analizate este specificata la rulare, prin primul parametru.
Matricea si vectorul sunt generate random, cu seed constant(vor fi aceleasi pt acelasi N dat).

Implicit consideram ca nu vom lucra cu transpusa, de aceea nu avem parametru special pentru aceasta.

===================================================================================================
2.) Functionarea scripturilor
===================================================================================================
-------------------------------------------------
a. In modulul principal:
-------------------------------------------------
Scopul: Efectuarea de teste pe matrici de diferite dimensiuni si salvarea rezultatelor in 
folderul "./results"

Voi enumera scripturile in ordinea apelarii:

->test_all.sh(apelat de target-ul run din Makefile)

Pentru un N cu dimensiuni intre 500 si 30000(pas de 1000), calculeaza timpul necesar inmultirii
unei matrici de dimensiune NxN cu un vector de dimensiune N.

Pentru fiecare N(500, 1500, .. 29500) apeleaza scriptul "my_run.sh" trimitand N ca parametru.
Apoi, la fiecare 5 secunde verifica starea cozilor apeland qstat. Cand toate task-urile
au fost rezolvate N se incrementeaza si procesul se reia.


->my_run.sh(apelat de test_all.sh)

Trimite cate un job pentru fiecare din cele 3 cozi. Pe langa numele script-ului care va fi 
executat ca job se trimite si dimensiunea N ca parametru, si numele cozii la care a fost trimis
("NEHALEM", "OPTERON", "QUAD").

Job-urile vor fi reprezentate de o executie a script-ului exec_script.sh


->exec_script.sh(apelat de my_run.sh)

Verifica(se uita la primul parametru) coada in care este rulat. In functie de aceasta
compileaza sursele, generand cele 3 executabile(naive, optimized, blas), si apoi le ruleaza,
specificandu-le ca parametrii dimensiunea matricei(N) si tipul ei(superior/inferior triunghiulara).

-------------------------------------------------
b. In modulul de debugging:
-------------------------------------------------
Debugging-ul este realizat prin folosirea target-ului debug in Makefile, sau prin rularea
scriptului check_corectitude.sh

->check_corectitude.sh
Apeleaza test_correct.sh din modulul correctitude_test si apoi verifica daca fisierele generate
opt, naive si blas sunt identice. Daca da afiseaza "0 errors found"

->test_correct.sh
Acesta pune in coada OPTERON 4 job-uri pentru matrici de dimensiune 5000, 10000, 15000, 20000
si asteapta incheierea fiecarui job inainte de a-l lansa pe urmatorul.

->my_run_debug.sh
Este folosit pentru punerea job-urilor pe coada OPTERON

->debug_script.sh
Reprezinta modulul executat in fiecare job


-------------------------------------------------
c. In modulul plotare:
-------------------------------------------------
In urma executiei modulului principal rezulta 12 fisiere:
-9 de forma
	$(nume_coada)-$(tip_implementare)

	Contin pe fiecare linie un timp

3 de forma
	$(nume_coada)-step

	Contine pe fiecare linie o dimensiune de matrice

Intr cele 2 tipuri exista urmatoarea corespondenta:

linia i a fisierulelor "opt-$(tip_implementare)" corespunde linei
i a fisierului "opt-step". Astfel stim pentru fiecare timp, dimensiunea matricei pt care a fost obtinut.

Se genereaza 4 ploturi prin intermediul scriptului draw_plot.sh care apeleaza 4 scripturi *.gp.

In cadrul unui script *gp:
	-abscisa va fi data de un fisier "*-step"
	-ordonata va fi luata dintr-un fisier "$(nume_coada)-$(tip_implementare)"


In urma executiei "draw_plot.sh" se vor crea cele 4 plot-uri in folder-ul ./results


===================================================================================================
III. Analiza rezultatelor
===================================================================================================

In urmatoarea analiza ma voi folosi de ploturile existente in aceasta arhiva(folderul tests):

nehalem.png:	|= 	Graficul reprezinta timpii de executie raportati la dimensiunea problemei
opteron.png:	|	pentru cele 3 implementari(optimizata, naiva si atlas blas dtrmv)
quad.png:		|
mixed.png:		Graficul care prezinta timpii de executie raportati la dimensiunea problemei
				pentru fiecare din cele 3 masini pe implementarea naiva.

De asemenea in folderul tests avem fisierele pe baza carora s-au generat aceste plot-uri

Din acestea se pot trage urmatoarele concluzii cu privire la:

-------------------------------------------------
1. Atlas blas dtrmv vs. implementarea mea naiva
-------------------------------------------------

Diferentele sunt imense. Se poate observa ca pentru o matrice de dimensiune 15000 implementarea naiva
consuma 3 secunde pe opteron si quad, respectiv 1.8 pe nehalem, in timp ce implementarea atlas blas
nu depaseste pragul 0.2 secunde.
De asemenea se poate observa, dupa forma curbelor, ca in cazul implementarii naive avem aproape o
crestere exponentiala a timpului raportata la dimensiune, in timp ce pentru implementarea atlas blas
cresterea este polinomiala(de gradul I).

Aceasta diferenta se datoreaza mai multor factori, cum ar fi:

-implementarea naiva nu optimizeaza in niciun fel accesul la memorie
		- matricea este parcursa cu indici, pierzandu-se timp cu calcularea acestora
		- accesul la constante din interiorul buclerol nu este optimizat(prin folosirea de variabile "register")

-este posibil ca atlas blas dtrmv-ul sa aiba si optimizari specifice pentru cache-ul masinilor pe care se ruleaza

-------------------------------------------------
2. Atlas blas dtrmv vs. implementarea mea optimizata
-------------------------------------------------

Spre deosebire de punctul anterior aici diferentele nu mai sunt atat de mari. Se poate observa, spre 
exemplu pe opteron, ca implementarea optimizata este putin mai inceata, insa este destul de apropiata 
ca performante de implementarea atlas blas.

- pentru matrici de dimensiuni intre 0 si 7000 timpii sunt aproape identici
- odata cu cresterea dimensiunii diferenta de timp creste si ea, dar se poate observa ca implementarea
optimizata reuseste sa scoata un timp cam cu 30-40% mai mare decat implementarea atlas blas.
(pe opteron, la dimensiune 30000 observam 0.31s pentru atlas blas, si 0.52s pentru nehalem)


Imbunatatirea timpilor fata de versiunea naiva se datoreaza:
		-imbunatatirii accesului la constante
		-parcurgerii structurii cu pointer(evit operatii suplimentare pe indici)
		-compilarii cu flag-ul O5


-------------------------------------------------
3. Cele 3 masini pe care se testeaza
-------------------------------------------------

In primul rand sa trecem in revista specificatiile celor 3 cozi.

->NEHALEM
Frecventa: 		2.53 GHz
Cache L2: 		12 MB
Numar core-uri:	16
Memoria:		32GB

->OPTERON
Frecventa: 		2.6 GHz
Cache L2: 		3 Mb
Numar core-uri:	12
Memoria:		16GB

->QUAD
Frecventa: 		2 GHz
Cache L2: 		12 Mb
Numar core-uri:	8
Memoria:		16GB



Din rezultatele obtinute, in special din plot-ul mixed.png, se observa ca 
NEHALEM are performante net superioare celorlalte 2. 
Aceasta se datoreaza in principal cache-ului L2, de 8 ori mai mare decat la QUAD si de 32 de
ori mai mare decat la OPTERON. De asemenea si memoria poate juca un rol important,
pe teste mai mari decat cele realizate(in momentul in care pe cele cu 16GB ar fi
necesar un swap).
Numarul core-urilor nu consider ca poate fi un factor neaparat decisiv intrucat
programul(testele) ruleaza pe un singur core.



Intre OPTERON si QUAD diferenta este mult mai mica. 

In primul rand se observa ca pentru implementarea naiva timpul obtinut pe OPTERON
este mai bun(cu 50-100 ms) decat cel obtinut pe QUAD. De asemenea si pentru implementarea 
optimizata se observa un timp marginal mai bun pe OPTERON.
Cand analizam timpii pentru implementarea atlas blas observam o crestere de peste 50% a timpului
pe QUAD.

Explicatia: Cresterile de timp pot fi date de frecventa mai mica a procesorului 
pentru arhitectura QUAD, iar in cazul atlas blas depind de modul in care au fost facute optimizarile.

Personal ma asteptam ca rezultatele, cel putin in cazul atlas blas, sa fie mai bune pe QUAD
datorita dimensiunii cache-ului.(Eram destul de sigur ca atlas blas face optimizari si la nivel
de cache).


Analiza intre versiunea naiva si cea optimizata:

-> OPTERON (N=30.000)
optimizat: 	1.11 sec
naiv:		11.05 sec

-> QUAD (N=30.000)
optimizat: 	1.21 sec
naiv:		11.52 sec

-> NEHALEM (N=30.000)
optimizat: 	0.54 sec
naiv:		7.17 sec