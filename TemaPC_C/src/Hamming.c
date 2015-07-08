#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

/* Functie care intoarce valoarea lui x la puterea y. */
int power (int x, int y)
{
	int i;
	int result = 1;
	for(i = 0; i < y; i++)
		result *= x;
	
	return result;
}


/* Functie care creeaza vectorul de biti de control. */
void controlBiti(char* vectBitiControl, msg e)
{
	int i, j;
	
	for(i = 0; i < CORRECTORSIZE; i++)
		for(j = 1; j <= MSGSIZE; j++)
			if(j & power(2, i))
					vectBitiControl[i] = vectBitiControl[i] ^ e.payload[j - 1];	
}

/* Functie care calculeaza codul Hamming, descris in README, si salveaza in campul din mesaj. */
void calculateHamming(msg* t)
{
	int j;
	char auxiliar[CORRECTORSIZE];
	memset(auxiliar, 0, CORRECTORSIZE);
	
	controlBiti(auxiliar, *t);
	
	for(j = 0; j < CORRECTORSIZE; j++)
		(*t).payload[power(2, j) - 1] = auxiliar[j];
}

/* Functie care realizeaza corectia. Se porneste de la presupunerea ca un singur octet este afectat.
Nu oricare 8 biti consecutivi. */
void correctHamming(msg* t)
{
	char checker[CORRECTORSIZE];
	int i;
	int index, aux;
	memset(checker, 0, CORRECTORSIZE);
	controlBiti(checker, *t);
	
	index = -1;
	for(i = 0; i < CORRECTORSIZE; i++)
	{
		if(checker[i] != 0)
		{
			index += power(2, i);
			aux = checker[i];
		}
	}
	
	for(i = 0; i < CORRECTORSIZE; i++)

	(*t).payload[index] = (*t).payload[index] ^ aux;
}

/* Aseaza cei 11 biti de control situati in mod normal la inceputul campului payload in pozitiile
caracteristice. */
void swapBytes(msg* t)
{
	int i;
	char aux;

	
	for(i = CORRECTORSIZE - 1; i >= 0; i--)
	{

		aux = (*t).payload[i];	
		(*t).payload[i] = (*t).payload[power(2, i) - 1];
		(*t).payload[power(2, i) - 1] = aux;

		}

}

/* Realizeaza procesul invers functiei swap. */
void unSwapBytes(msg * t)
{
	int i;
	char aux;

	
	for(i = 0; i <CORRECTORSIZE; i++)
	{

		aux = (*t).payload[i];	
		(*t).payload[i] = (*t).payload[power(2, i) - 1];
		(*t).payload[power(2, i) - 1] = aux;
		
		}
}
