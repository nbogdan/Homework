#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"


/*
Functii auxiliare folosite in rezolvarea task-urilor.

NOTA: Functiile care gestioneaza lucrul cu structura de date(lista)
au fost preluate, cu mici modificari, din cursul de SD, anul 1.
*/

/* Functie care creeaza un duplicat al unui mesaj.
Intoarce adresa catre noua structura creata. */
msg* mesageDup(msg* e)
{
	msg* aux = (msg*)malloc(sizeof(msg));
	if(aux)
	{
		aux->len = e->len;
		memcpy(aux->payload, e->payload, MSGSIZE);
	}
	
	return aux;
}

/* Functie care calculeaza lungimea listei. */
int ListLength(ALista aL)
{
	int contor = 0;
	while (*aL != NULL) 
	{
		aL = &(*aL)->urm;
		contor++;
	}
	return contor;
}

/* Functie care aloca o celula in cadrul listei, continand o structura de tip mesaj. */
/* Rezultatul va fi adresa noii celule sau NULL. */
TLista AlocCel(msg* e)          
{ TLista aux = (TLista)malloc(sizeof(TCel));
  
  if (aux)               
  { aux->info = *e;      
    aux->urm = NULL;
  }
  
  return aux;            
}

/* Functie care insereaza un mesaj la inceputul unei liste. */
/* Intoarce 1 pentru succes, 0 pentru esec. */
int InsInc(ALista aL, msg* e)    
{ 
  TLista aux = (TLista)malloc(sizeof(TCel));
  
  if (!aux) return 0;            
  aux->info = *e;                
  aux->urm = *aL;
  *aL = aux;                     
  
  return 1;                      
}

/* Functie care insereaza un mesaj la sfarsitul unei liste. */
/* Intoarce 1 pentru succes, 0 pentru esec. */
ALista InsSfL(ALista aL, msg* e) 
{ 
  TLista aux;
  
  aux = AlocCel(e);              
  if (!aux) return aL;           
  while (*aL != NULL) aL = &(*aL)->urm;
  *aL = aux;                        
     
  return aL;                        
}

/* Functie care insereaza un mesaj astfel incat lista sa fie ordonata dupa packNum. */
/* Intoarce 1 pentru succes, 0 pentru esec. */
int InsOrd(ALista aL, msg* e)
{
	while (*aL != NULL && (*(pkg*)((*aL)->info).payload).packNum < 
				(*(pkg*)(*e).payload).packNum) aL = &(*aL)->urm; 
	return InsInc(aL, e);
}

/* Functie care intoarce adresa catre al i-lea mesaj din lista. */
msg* getindex(ALista aL, int i)
{
	int index = 0;
	while (*aL != NULL && index < i){
		aL = &(*aL)->urm;
		index++;
	}
	return &((*aL)->info);
}	

/* Functie care intoarce adresa catre mesajul cu valoarea packNum in campul packNum. */
/* Intoarce NULL in caz de esec. */
msg* getPackNum(ALista aL, int packNum)
{
	while((*aL)->urm != NULL)
	{
		if((*(pkg*)((*aL)->info).payload).packNum == packNum)
			return &((*aL)->info);
		aL = &(*aL)->urm;
	}
	return NULL;
}

/* Functie care elimina primul element din lista. */
/* Intoarce 1 pentru operatie reusita, 0 pentru esec. */
int ElimPL(ALista aL)  
{ TLista aux = *aL;    
  if (!aux) return 0;  
  *aL = aux->urm;      
  free (aux);          
  return 1;
}

/* Functie care elimina pachetul cu numar de ordine packNum. */
/* Intoarce 1 pentru succes, 0 sau -1 pentru esec(lista vida, element negasit). */
int ElimPackNum(ALista aL, int packNum)
{
	while((*aL)->urm != NULL)
	{
		if((*(pkg*)((*aL)->info).payload).packNum == packNum)
			return ElimPL(aL);
		aL = &(*aL)->urm;
	}
	return -1;
}

/* Functie care elimina ultimul element din lista. */
int ElimLast(ALista aL)
{
	while ((*aL)->urm != NULL) aL = &(*aL)->urm;
	return ElimPL(aL);
}

/* Functie care afiseaza valorile elementelor din lista. */
void AfisareL(ALista aL)
{ TLista aux = *aL;
  for (; aux != NULL; aux = aux->urm)
    printf("Numar pachet: %i\n", (*(pkg*)(aux->info).payload).packNum);
}

/* Functie care tipareste in fisierul de indice fd toate pachetele consecutive, 
pornind de la currentPack, si le elimina din lista. */
/* Ex: 1,2,3,4,6,7,8,9 elementele din lista, currentPack = 1
	Se tiparesc si elimina elementele 1,2,3,4 
	currentPack  = 5, lista = 6,7,8,9 */
/* Intoarce numar mesaje eliminate. */
int EmptyBuffer(ALista aL, int fd, int currentPack)
{
	int contorEliminari = 0;
	while(*aL != NULL && (*(pkg*)((*aL)->info).payload).packNum == currentPack)
	{
		write(fd, (*(pkg*)((*aL)->info).payload).payload, ((*aL)->info).len - 1);
		ElimPL(aL);
		currentPack++;
		contorEliminari++;
	}
	return contorEliminari;
}

/* Functie care verifica daca un mesaj cu numar de ordine packNum 
este continut in lista. */
int Contains(ALista aL, int packNum)
{
	while(*aL != NULL)
	{
		if((*(pkg*)((*aL)->info).payload).packNum == packNum)
			return 1;
		if((*(pkg*)((*aL)->info).payload).packNum > packNum)
			return 0;
		aL = &(*aL)->urm;
	}
	return -1;
}

/* Functie care gaseste in vectorul NAKArray, de dimensiune size, elementul x. */
/* Intoarce indicele acestuia. */
int find(int* vector, int size, int x)
{
	int i;
	for(i = 0; i < size; i++)
		if(vector[i] == x)
			return i;
	return -1;
}

/* Functie care realizeaza XOR la nivel de byte. */
/* Este aproape inutila. */
char byteXOR(char octet, char checkSum)
{
	checkSum = octet ^ checkSum;
	return checkSum;
}

/* Functie care genereaza un checkSum pe 8 biti pentru un mesaj. */
char msgCheckSum(msg e)
{
	char checkSum = 0;
	int j = 0;
	for(j = 0; j < MSGSIZE; j++)
		checkSum = byteXOR(checkSum, e.payload[j]);
	return checkSum;
}
