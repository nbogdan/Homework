#	Tema 3 ASC
#
#	Student:	NICULA Bogdan
#	Grupa:		334CB

==============================================================================================
I. Continutul arhivei
==============================================================================================
Arhiva are urmatoarea structura:

./ : 	/spu, /ppu, /serial
		README.txt (acest fisier)
		Makefile
		put_command.sh	: script care pune o comanda de testare in coada cell
		testing.sh	: script rulat pe coada cell care testeaza executabilul tema3

./spu:	my_spu.c : Codul care descrie modul de functionare pentru fiecare SPU

./ppu:	my_ppu.c : Codul care descrie modul de functionare pentru PPU
		btc.c, btc.h, pgm.c: Fisiere din scheletul temei(implementarea seriala) folosite
					pentru citirea/scrierea din fisiere

./serial:  Contine fisierele sursa pentru implementarea seriala. Este folosit pentru testarea rezultatelor obtinute.

./previous_results:	Contine rezultatele ultimelei rulari

==============================================================================================
II. Testarea temei
==============================================================================================
Pentru a compila tema(conectati fiind pe coada de compilare), rulati doar make.

Pentru testarea temei rulati script-ul put_command.sh cand sunteti logati pe fep. Acesta va pune in coada
cell un task constand in executarea script-ului testing.sh

Prin intermediul acestuia tema va fi testata pentru 1, 2, 4, 8 SPU-uri, normal si double buffering, pe imaginile de test.

OBSERVATIE: Imaginile de test(format .pgm) nu sunt incluse in arhiva. Acestea trebuiesc copiate in radacina arhivei,
 adica in acelasi loc cu fisierul de README sau cu executabilul temei.
 Ele pot fi luate de pe moodle(doar testele small), si de pe fep(doar testele mari), din directorul '/export/asc/btc_input'.

In urma testarii vor rezulta 3 fisiere:

output: Contine timpii de rulare pentru fiecare test
output_compare: Contine rezultatul compararii rezultatelor implementarii seriale cu cele ale implementarii pentru cell.
error:	Contine warning-uri si posibile erori.

==============================================================================================
III. Modul de functionare(generalitati)
==============================================================================================
Toate cerintele din enunt, inclusiv double buffering au fost implementate.

In continuare voi descrie succint modul de functionare:

~~~~~~~~~~~~~~~~~~~
La nivelul PPU-ului 
~~~~~~~~~~~~~~~~~~~
Se creeaza num_spu thread-uri care lanseaza tot atatea SPU-uri.
Acestea vor primi ca parametru adresa catre o structura de forma:

typedef struct {
	short int *sourcePGM;	// adresa matrice(liniarizata) pgm sursa
	struct block* destBTC;	// adresa vector de structuri block
	short int *destPGM;	// adresa matrice(liniarizata) pgm destinatie
	int width, height;	/* dimensiuni pgm initial */
	int num_spus; /* nr spu-uri */
	int spu_id;	/* id spu*/
	char padding[4]; /* octeti folositi pentru ca dimensiunea structurii sa fie multiplu de 16
						padding[0] indica tipul operatiei: normal/double buffering */
} info_token;

Se asteapta ca spu-urile sa isi incheie executia, se masoara si se afiseaza timpul trecut
de la crearea primului thread pana la oprirea ultimului si se afiseaza(acesta este timpul
pentru comprimare + decomprimare) si rezultatele(imaginea comprimata si decomprimata)
sunt scrise in fisiere.


~~~~~~~~~~~~~~~~~~~
La nivelul SPU-ului 
~~~~~~~~~~~~~~~~~~~
Fiecare SPU va face un prim request DMA pentru a obtine propria sa structura info_token.
Odata primita aceasta va trece prin urmatorii pasi:
	1. Calculeaza numarul de blocuri pe care trebuie sa le prelucreze
	2. Preia un bloc prin DMA 
	3. Realizeaza compresia(PGM->BTC) blocului si o trimite prin DMA(mfc_put in adresa destinatie
	specificata de info_token)
	4. Pentru BTC-ul obtinut anterior se realizeaza si decompresia si se trimite prin mfc_put
	rezultatul catre PPU
	5. Se reia pasul 2 pentru urmatorul bloc.

~~~~~~~~~~~~~~~~~~~
Observatii
~~~~~~~~~~~~~~~~~~~
-> Fiecare bloc necesita 8 cereri mfc_get, cate una pentru fiecare linie. Astfel vom avea:
8 * numar_blocuri cereri DMA efectuate.
Solutia nu este optima, insa este usor de inteles, si scaleaza, rezultatul obtinut pentru 8 SPU-uri 
fiind de aprox. 8 ori mai rapid decat cel serial

-> Pentru a evita branch-urile la nivel de SPU(ele fiind foarte costisitoare) le-am inlocuit
prin expresii matematice(folosind mai ales functia fabs())

exemplu:

if(a > 0) a = 0;	devine		a = (a + fabs(a)) / 2;


==============================================================================================
IV. Instructiuni vectoriale
==============================================================================================
-> Calculul sumei:

Peste blocul de 64 de short-int-uri am mapat un array de vector de short int. Astfel operatiile
de adunare s-au efectuat la nivel de vector(cate 8 operatii deodata), procesul fiind mai rapid.

-> Calculul deviatiei standard:

Cele 64 de short int-uri au fost covertite intr-un nou array de float-uri, peste care s-a mapat
un vector de float-uri. La nivelul acestuia se efectueaza operatiile necesare pentur calculul
deviatiei standard(scaderi, inmultiri).


-> Alte imbunatatiri posibile:

Array-ul de short int-uri putea fi convertit la unul de char-uri, pe care mai apoi sa se efectueze
spu_cmp, respectiv spu_cntb pentru determinarea bitplane-ului si a lui q.
Aceasta operatie nu a mai fost implementata deoarece overhead-ul de creare a 3 vectori de char-uri
era prea mare(in opinia mea) pentru a o justifica.


==============================================================================================
V. Double buffering
==============================================================================================
Functionare double buffering:
	Cer block A
	Cer block B
	Astept block A
	prelucrez block A
	Cer block A_nou
	Astept block B
	prelucrez bloc B
	Cer block B_nou
	...

Datorita alegerii de a cere cate un bloc, si nu un chunk de block-uri, speed-up - ul oferit de aceasta 
metoda nu este substantial(sub 1.1).
Este evident ca in cazul in care de fiecare data cer 16000 de octeti(125 blocuri) o astfel de metoda
poate injumatatii timpii de executie, insa in acest caz nu il modifica drastic.


==============================================================================================
VI. Rezultate teste
==============================================================================================

Ultimele teste rulate se afla in directorul previous_tests.
Din acestea am extras rezultatele pentru  imaginea in1.pgm, testata pe
1, 2, 4, 8 SPU-uri, normal si double buffering.


-----------------------------------
in1.pgm
---------Serial implementation-----------
Encoding / Decoding time: 1.145198
Total time: 1.425869
---------Personal implementation-----------
Mode normal
Num SPUs 1
Time: 1.644497

Mode double buffering
Num SPUs 1
Time: 1.622522
---------Personal implementation-----------
Mode normal
Num SPUs 2
Time: 0.822221

Mode double buffering
Num SPUs 2un 
Time: 0.813033
---------Personal implementation-----------
Mode normal
Num SPUs 4
Time: 0.413112

Mode double buffering
Num SPUs 4
Time: 0.408538
---------Personal implementation-----------
Mode normal
Num SPUs 8
Time: 0.214503

Mode double buffering
Num SPUs 8
Time: 0.211554


Se observa ca pentru 1 SPU rezultatul este mai slab decat cel serial, acest fapt datorandu-se overhead-ului de 
comunicare. Totusi de la 2 SPU-uri in sus timpii obtinuti sunt mult mai buni, obtinandu-se un speedup
de 5 pentru 8 SPU-uri.(raportat la versiunea seriala)

De asemenea se observa ca double buffering-ul ofera un rezultat mai bun, dar datorita elementelor prezentate mai sus
rezultatul nu reprezinta o imbunatatire semnificativa.(1-2%)


Observatie: Pe testele pentru double buffering se obtin mici diferente
la nivel de pixeli(1 la 10000), acestea neafectand aspectul imaginii.
