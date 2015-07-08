>>>>Proiectarea Algoritmilor - Tema 2<<<<<

///Student: Nicula Bogdan
///Grupa: 325CB
///Data:  aprilie 2013

NOTA: Cerintele acestei teme, precum si server-ul folosit pentru testarea ei se gasesc in 
arhiva ResurseTema2PA.zip
--------------------------------------------------------------------------------------------
I. Algoritmul de rezolvare al problemei

Datorita nivelului scazut de dificultate al botilor am ajuns la concluzia ca problema
poate fi rezolvata printr-un algoritm de tip "Greedy" care alege solutia optima la pasul
curent, fara a ne interesa de mutarile adversarului la pasii urmatori.

Deoarece scopul este "sa bagam mingea in poarta" (matematic sa ajungem in punctul (4,11), 
vezi reprezentarea de mai jos), solutia optima va fi cea care ne duce in punctul cel mai
apropiat de punctul (4,11).

Rezolvare efectiva:

-citim mutare adversar si actualizam tabla de joc

-In acest moment de timp ne aflam in punctul x. 
-Vom calcula toate punctele la care se poate ajunge din x, conform regulilor jocului. 
-Din lista de puncte, il alegem punctul cel mai apropiat de centrul portii (4,11) calculand
distanta euclidiana.
-Pentru punctul gasit construim ruta de la punctul curent la cel final, actualizam tabla
de joc si afisam la System.out ruta folosita

-citim urmatoarea mutare a adversarului si actualizam iar tabla de joc

Acest proces este reluat cat timp primim mesaje de mutare (M) de la server.


--------------------------------------------------------------------------------------------
II.Implementare:

1)Clase auxiliare

a)Clasa Point
	-este reprezentarea unui punct de pe tabla de joc
	-am suprascris metodele equals si toString pentru verificari mai usoare pe parcurs

b)Clasa Arc
	-este reprezentarea unui arc(latura de patrat) de pe tabla de joc
	-din nou am suprascris metodele equals si toString
	-pe langa am mai definit o functie care verifica apartenenta unui punct la un arc


2)Clasa Principala(Clasa Bot)

Aceasta clasa reprezinta botul efectiv. In cadrul ei retinem si reprezentarea tablei de joc.

---->Tabla de joc.

Consideram urmatorul sistem de coordonate: 

		      __	
	 (10,0)x_____|	|______x(10,8)
						Am marcat colturile tablei si punctul de
		      ...			pornire. Un alt reper este centrul portii
						la care vrem sa marcam (4,11)	
		       x(4,5)	

		      ...
	  (0,0)x_____    ______x(0,8)
		     |__|		


Nu am optat pentru o reprezentare clasica, matriciala, in schimb am reprezentat tabla ca pe
un graf neorientat avand drept varfuri punctele prin care s-a trecut si arce laturile care
au fost strabatute pana in acel punct.
Punctul de pornire (4,5) este considerat ca facand garantat parte din graf

Exemplu:

Dupa cele 2 mutari:
A: M 1 0
B: M 1 2

Avem:

puncte = [(4,5), (4,6), (5,6)]
laturi = [(4,5)-(4,6), (4,6)-(5,6)]

iar punctul curent este (5,6)


---->Metode care gestioneaza mutarile adversarului:

>returnDestination
Intoarce nodul(punctul) destinatie pentru un nod curent si o mutare(0..7) date

>move
Efectueaza o mutare, actualizand tabla de joc

>adversaryMove
Pentru fiecare mutare a adversarului actualizeaza tabla de joc(Adauga noduri si arce).
Intoarce pozitia curenta dupa ce adversarul si-a incheiat randul.


---->metode care gestioneaza mutarea botului:

>possibleMoves
Pentru un nod(punct) dat intoarce o lista cu toate nodurile(adiacente) in care se poate muta.

Exemplu:		
o--
-ao   o -> noduri care au deja arc (o,a) sau (a,o)
o--   - -> noduri care nu sunt legate la a

Se intoarce o lista continand toate punctele de tip "-".
	

>getAllPoints
Pentru un nod(punct) dat intoarce o lista cu toate punctele la care se poate ajunge, respectand
regulile jocului.
Mod de functionare:
	-ia punctele adiacente(possibleMoves)
		-pentru fiecare:
			-daca este punct final(nu este deja salvat in lista de puncte) il 
		intoarcem ca rezultat
			-daca nu este punct final aplicam functia(getAllPoints) pe el

La final, prin concatenare se obtine lista tuturor punctelor "destinatie" posibile

>getPath
Reconstruieste recursiv calea dintre punctul a si punctul b si o intoarce sub forma unei liste.
In acelasi timp actualizeaza si tabla de joc.
Aceasta functie o aplicam pe un punct destinatie b doar atunci cand suntem siguri
ca acesta este solutia optima la pasul curent.
Mod de functionare:
	-pornind de la punctul a ia toate punctele adiacente
	 	-pentru fiecare
			-daca e punct final si e egal cu b intorc lista
			-altfel intorc o lista vida
Recurenta:

getPath(a1,b)[i] = a1 + getPath(a2,b)[i+1] 

Gradual se construieste lista : [a .. a1, a2, .. b]

>convertPath
Converteste o lista de forma celei de mai sus intr-o lista de directii(valori intre 0 si 7).


---->metoda principala: solve
Mod de functionare:
-initializez tabla de joc
-citesc mutare adversar(daca primesc S, incep eu)
-cat timp primesc mutari ale adversarului
	-actualizez tabla
	-pentru punctul curent calculez toate punctele-destinatie posibile
	-aleg punctul optim(cel mai apropiat de centrul portii)
	-generez calea pana la el, o convertesc in lista de directii si o afisez la System.out
-program incheiat

--------------------------------------------------------------------------------------------
III.Observatii:

-In cazul in care botul "deschide" jocul mutarea sa a fost hardcodata(M 1 0; mutare spre N),
 deoarece este, in opinia mea, cea mai eficienta mutare la acel pas. Astfel eliminam niste 
calcule suplimentare lipsite de utilitate.

-Toate cele 3 clase folosite au fost definite in cadrul aceluiasi modul. Am luat aceasta decizie
deoarece 2 clase erau de dimensiuni mici(20-40 linii) iar a treia(clasa Bot) de dimensiune
mare(in jur de 500 linii). Decat sa creez 2 module infime si unul mare am considerat ca 
pot spori macar vizibilitatea codului prin aceasta alegere.

-Intrucat nu am variabile generate Random, nu am folosit seed-ul pasat ca argument.