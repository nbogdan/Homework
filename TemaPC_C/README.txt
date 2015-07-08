#####   Tema 1 PC   ####
///Student: Nicula Bogdan Daniel
///Grupa: 325CB
///Data: Aprilie 2013


NOTA: Scheletul initial al temei se afla in arhiva Tema1_PC. Pentru detalii cu privire la 
cerintele temie consultati: Tema1_PC.pdf

I. Task-ul 0

Aceasta problema este asemanatoare cu Laboratorul 4 ca si abordare.
a)Se primesc ca date de intrare viteza si timpul de propagare caracteristice legaturii de date. 
Astfel primul pas reprezinta calcularea BDP-ului si a dimensiunii ferestrei(W).
b)Se defineste tipul pkg, care se muleaza peste campul payload la msg-ului:

typedef struct {
  int packNum;
  int messageType;
  char checkSum;
  char payload[PKGSIZE];
} pkg;
//PKGSIZE =	1391

Aceasta structura se va folosi la task-urile 0, 1, 2, 3.

Definim tipurile: 	1- pachet INIT: contine numele fisierului
			2- pachet INFO: contine informatie din fisier
			3- pachet END: marcheaza finalul transmisiei.

c)Primul pachet se trateaza separat.(Nu se mai trimit alte mesaje pana ce avem confirmarea primirii lui).
d)Dupa primirea acelei confirmari se trimit primele W pachete fara a se astepta confirmare.
Apoi, pentru fiecare confirmare primita, se mai poate trimite un pachet.
e)Dupa trimiterea ultimului pachet(de tip INFO), se asteapta primirea  tuturor confirmarilor ramase, 
iar apoi se transmite pachetul END.


II. Task-ul 1

Punctele (a), (b), (c) sunt identice cu task-ul 0

->in sender
d)Se foloseste o structura de tip lista( se foloseste ca o coada: inseram la sfarsitul listei, 
scoatem de la inceputul ei) pe post de buffer.
e)Se trimit primele W pachete(pe masura ce se trimit se si salveaza in buffer) fara a se astepta confirmare.
f)Cand primim confirmarea pentru un pachet, acesta este eliminat din buffer, 
iar urmatorul pachet este trimis, si in prealabil salvat in buffer.
f)In cazul in care depasim timpul timeout retrimitem intreg bufferul de mesaje salvate
(acest buffer e mereu <= dimensiune_fereastra, deci nu exista pericolul de a suprasolicita reteaua).
g)Procesul se incheie atunci cand fisierul a fost citit in intregime, si toate packetele stocate in buffer
au fost trimise si confirmate.

->in receiver
g)Mereu stim ce mesaj urmeaza sa primim, in cazul in care se primeste un alt mesaj il ignoram.

Observatii:
-programul ruleaza destul de incet(3-4 minute pentru 2-3MB de date) deoarece mereu pierderea unui pachet este 
"confirmata" de atingerea timeout-ului, iar timeout-ul este definit: max(1000, 2*DELAY)
-daca timeout-ul ar fi fost definit min(1000, 2*DELAY), acest proces ar fi fost semnificativ imbunatatit
-protocolul se putea imbunatatii si prin trimiterea de catre receiver a unor mesaje NACK atunci cand primeste 
un pachet eronat. Aceasta ar fi redus timpul consumat pentru un pachet pierdut la (DELAY, 2 * DELAY), 
dar in opinia mea nu respecta intocmai protocolul descris in slide-urile de curs.

III. Task-ul 2

Pasii (a), (b), (c), (d) sunt identici cu Task-ul 1.

->in sender
e)Trimitem primele W pachete fara a astepta vreo confirmare si le salvam in buffer.
f)Asteptam ACK-uri sau NACK-uri de la receiver. 
		-in cazul in care primim NACK retrimitem pachetul in cauza,
		-daca primim ACK, stergem pachetul respectiv din buffer si trimitem urmatorul 
	pachet, salvandu-l in buffer.
		-daca se atinge timeout-ul, retrimitem toate mesajele aflate in buffer
	(a carui dimensiune nu depaseste dimensiunea ferestrei)
g)Odata ce s-a incheiat citirea din fisier trimitem un ultim pachet care marcheaza sfarsitul informatiei utile,
apoi mai avem ca scop golirea buffer-ului din sender
De aceea se aplicam algoritmul descris la (f) pana ce buffer-ul se goleste.

->in receiver
h)Folosim un vector de int-uri de dimensiunea ferestrei pentru a contoriza pachetele pentru care urmeaza sa 
primim pachete(NAKArray). Acesta este initializat cu valorile: 0, 1, ..., window_size-1. Daca pachetul i este 
primit la pozitia din vector unde avem scrisa valoarea i suprascriem numarul de ordine al urmatoarei ferestre
asteptate:

0, 1, 2, ... i-1, window_size, i+1, ..., window_size-1

Astfel stim mereu ce pachete urmeaza sa primim.
i)Retinem in variabila last numarul de ordine(packNum) al ultimului pachet primit, iar in current al celui curent.
In timp ce trimitem ACK-ul pentru pachetul current, vom trimite si NACK-uri pentru pachetele aflate intre
last si current.
j)Pachetul primit se salveaza intr-un buffer, similar celui din sender, iar apoi se verifica daca se pot tipari
elemente in fisierul de iesire.
explicatie: 
	- in currentPack se retine numarul de ordine al pachetului care ar trebui tiparit.
	- daca acesta se afla in buffer, este tiparit, si impreuna cu el, toate mesajele consecutive
	- apoi acestea se sterg din buffer.
exemplu:
			Avem mesajele 3,4,5,7,9,10,11 si currentPack = 3
			Se vor scrie mesajele 3, 4, 5, iar currentPack devine 6
k)La primirea pachetului care marcheaza sfarsitul de date, actualizam variabila superior cu 
PackNum-ul acestuia (Aceasta fiind limita superioara a valorilor din NAKArray).
Atunci cand toate valorile din NAKArray sunt "fictive"(> superior) procesul se opreste.

		
IV. Task-ul 3
Modul de transmitere al pachetelor este cel de la problema 2.
Ne folosim de campul: char controlSum, care va permite detectia erorilor.
Se calculeaza suma de control:
Facem XOR la nivel de octet intre controlSum si toti octetii din payload-ul mesajului (mai exact 
calculam separat paritatea pentru toti bitii nr 1 ai tuturor octetilor, separat pentru toti bitii nr 2… )

->in sender
Se initializeaza campul controlSum cu 0.
Se calculeaza suma de control pentru paritate para, si se salveaza in campul controlSum

->in receiver
La primirea pachetelor in receiver se recalculeaza suma de control pentru tot payload-ul. 
Daca mesajul nu este corupt, suma de control obtinuta va fi 0.
In cazul unui pachet corupt ne comportam ca si cum nu l-am fi primit, 
iar algoritmul de la Task-ul 2 se va ocupa de problema.



V. Task-ul 4(BONUS)
Se foloseste transmiterea de la Task-ul 2.

Corectia simpla cu cod Hamming nu poate fi folosita aici, deoarece aceea rezolva doar erorile de 1 bit.

Totusi rezolvarea se va folosi de codul Hamming.

exemplu:(Doar vizual, a nu se verifica corectitudinea)

->1			11010101
->2			10110101
->3			01011101
->4			01101010
->5			11101100
->6			01010100
->7			01111011
->8			11110111
...			........

Octetii 1, 2, 4, 8 vor fi considerati octeti de control. Bitul i al fiecaruia dintre acestia va
fi considerat bit de control pentru bitii i ai octetilor de date.
Practic vom folosi "algoritmul/codul" Hamming clasic, doar ca pe coloane.

Implementare:
Se foloseste o noua structura:

typedef struct {
  char corrector[CORRECTORSIZE];	
  int packNum;
  int messageType;
  char payload[PKGSIZE2];
}  pkgbonus;

Deoarece payload-ul are 1400 octeti, vor fi necesari 11 octeti de control, salvati in vectorul corrector.

->in sender(dupa ce pachetele sunt gata de trimitere)
-vectorul corrector este initializat cu 0.
-octetii de control sunt plasati in pozitiile corecte(1, 2, 4, 8, 16 ... /*numerotare de la 1*/ )
-se calculeaza octetii de control(functia calculateHamming)
-octetii sunt reasezati in pozitiile initiale(functia unSwap) si mesajul este transmis

->in receiver
-se aplica swap pe payload-ul pachetului primit.
-se realizeaza corectia:
		-calculez octetii de control
		-insumez valorile pozitiilor octetiilor nenuli
		-la adresa obtinuta fac XOR intre valoarea de acolo, si valoarea unui octet de control nenul
-apoi se aplica unSwap

Observatie:
Aceasta rezolvare porneste de la presupunerea ca se corupe un octet:
ex: [OOOO.OOOO][XXXX.XXXX][OOOO.OOOO]
si NU oricare 8 biti consecutivi:
ex: [OOOO.OOOO][OOOX.XXXX][XXXO.OOOO]
