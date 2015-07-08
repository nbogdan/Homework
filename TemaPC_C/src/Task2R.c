#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"

/* Functia care descrie comportamentul receiver-ului petru task-ul 2 */


int Task2()
{
	pkg myPkg;
	msg* mesajDuplicat;
	msg t, aux;	
	char* filename;

	int window, k;
	int* NAKArray; /* vector in care retinem packNum-ul pachetelor care nu pot fi confirmate*/
	int superior = MAX_VALUE; /* limita superioara a numarului de pachete; 
					folosit in constructia NAKArray */
	
	int current = 0, last = 0, currentAux = 0, i = 0;
	
	int fdOut;
	int res;
	TLista bufferMesaje;
	int currentPack = 0;
	
	printf("[RECEIVER]Receiver starts.\n");
	printf("[RECEIVER]Task index=%i\n", 2);
		
	/* asteptare mesaj initializare */
	if (recv_message(&t) < 0) {
		perror("[RECEIVER]Error receiving message");
		return -1;
	}

	/* initializare */
	myPkg = *((pkg*)t.payload);
	printf("[RECEIVER] Got msg with payload: %s\n", myPkg.payload);
	
	window = myPkg.packNum;
	
	NAKArray = (int*)calloc(window + 1, sizeof(int));
	if(myPkg.messageType != TYPE_INIT){
		perror("[RECEIVER]Didn't receive init file!\n");
	}
	sprintf(aux.payload, "ACK(%s)", myPkg.payload);
	aux.len = strlen(aux.payload + 1);
	send_message(&aux);
	
	/* deschidere fisier out */
	filename = strdup("recv_");
	strcat(filename, myPkg.payload);
	fdOut = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	
	/*scriere in fisier pe masura ce se primesc pachetele.*/
	printf("Commence filewriting.\n");
	
	/* initializam NAKArray, salvand numele primelor window pachete care sunt asteptate */
	/* imediat ce un pachet este primit, el este sters din NAKArray, si urmatorul pachet 
	asteptat este trecut in locul lui */
	for(k = 0; k < window; k++)
		NAKArray[k] = k;
	
	/* initializare indice pachet curent */
	currentPack = 0;
	while(1) {
		/* curatare msg */
		memset(&myPkg, 0, sizeof(pkg));
		memset(&t, 0, sizeof(msg));
		
		res = recv_message(&t);
		myPkg = *((pkg*)t.payload);
		
		if (res < 0) {
				perror("[RECEIVER]Send ACK error. Exiting.\n");
				return -1;
			}
		printf("[RECEIVER]Received message number: %i\n", myPkg.packNum);
		
		/* daca pachetul final a fost primit, marcam o limita superioara pentru elementele
		din NAKArray */		
		if(myPkg.messageType == TYPE_END)
			superior = myPkg.packNum;	
		
		int auxiliar = 0;/* variabila folosita doar in cazul acestui ciclu */
		/* daca toate pachetele asteptate sunt "fictive" (numar de ordine mai mare decat
		al pachetului final, ne oprim) */
		for(i = 0; i < window; i++)
		{
			if(superior >= NAKArray[i])
				auxiliar = 1;
		}
		if(!auxiliar)
			break;
		
		/* daca pachetul primit se afla in lista de pachete asteptate */
		if(find(NAKArray, window, myPkg.packNum) >= 0)
		{
			/* salvam indicele ultimului pachet primit */
			last = current;
			/* retinem indicele pachetului curent*/
			current = find(NAKArray, window, myPkg.packNum);
			/* daca exista pachete neconfirmate intre ultimul pachet primit si 
			cel curent, trimitem NAK-uri pentru acestea*/
			if(current - last > 1)
				do{
					last++;
					printf("[RECEIVER]NACK Request %i %i for %i.\n",
								current, last, NAKArray[last]);
					sprintf(aux.payload, "NACK Pachet numarul: (%d)", NAKArray[last]);
					aux.len = strlen(aux.payload + 1);
					(*(pkg*)aux.payload).packNum = NAKArray[last];
					(*(pkg*)aux.payload).messageType = TYPE_NACK;
					
					res = send_message(&aux);
					if (res < 0) {
						perror("[RECEIVER]Send NACK error. Exiting.\n");
						return -1;
					}
				}while(current - last > 1);
			/* sunt analizate mai multe cazuri de pozitionare pentru last si current*/
			else if(last - current > 0)
			{
			
				currentAux = last + 1;
				while(currentAux < window)
				{	
					printf("[RECEIVER]NACK Request for %i\n", NAKArray[currentAux]);
					sprintf(aux.payload, "NACK Pachet numarul: (%d)", NAKArray[currentAux]);
					aux.len = strlen(aux.payload + 1);
					(*(pkg*)aux.payload).packNum = NAKArray[currentAux];
					(*(pkg*)aux.payload).messageType = TYPE_NACK;
					
					res = send_message(&aux);
					if (res < 0) {
						perror("[RECEIVER]Send NACK error. Exiting.\n");
						return -1;
					}
					currentAux++;
				}
				i = 0;
				while(i < current){
					printf("[RECEIVER]NACK Request for %i\n", NAKArray[i]);
					sprintf(aux.payload, "NACK Pachet numarul: (%d)", NAKArray[i]);
					aux.len = strlen(aux.payload + 1);
					(*(pkg*)aux.payload).packNum = NAKArray[i];
					(*(pkg*)aux.payload).messageType = TYPE_NACK;
					
					res = send_message(&aux);
					if (res < 0) {
						perror("[RECEIVER]Send NACK error. Exiting.\n");
						return -1;
					}
					i++;
				}
				
			}
			
			
			printf("[RECEIVER]Received correct package. Sending ACK.\n");

			/* pachetul primit este stocat in buffer, fiind inserat la pozitia corecta*/
			mesajDuplicat = mesageDup(&t);
			InsOrd(&bufferMesaje, mesajDuplicat);
			
			/* afisam mesajele consecutive, pornind de la currentPack, salvate in buffer */
			/* ex: Avem mesajele 3,4,5,7,9,10,11 si currentPack = 3
				Se vor scrie mesajele 3, 4, 5, iar currentPack devine 6 */
			currentPack += EmptyBuffer(&bufferMesaje, fdOut, currentPack);
			sprintf(aux.payload, "ACK Pachet numarul: (%d)", myPkg.packNum);
			aux.len = strlen(aux.payload + 1);
			(*(pkg*)aux.payload).packNum = myPkg.packNum;
			(*(pkg*)aux.payload).messageType = TYPE_ACK;
			
			res = send_message(&aux);
			if (res < 0) {
				perror("[RECEIVER]Send ACK error. Exiting.\n");
				return -1;
			}
			
			/* elementul confirmat il stergem din NAKArray, si inseram urmatorul element
			asteptat */
			i = find(NAKArray, window, myPkg.packNum);
			NAKArray[i] = k++;
		}
		/* Daca pachetul primit nu este dintre cele asteptate, il ignoram si trimitem 
		un NACK, pentru unul din pachetele asteptate*/
		else{
			int myAux;
			for(i = current + 1; i < window; i++)
			if(NAKArray[i] < superior)
			{
				myAux = i;
				printf("[RECEIVER]NACK Request for %i\n", NAKArray[myAux]);
				sprintf(aux.payload, "NACK Pachet numarul: (%d)", NAKArray[myAux]);
				aux.len = strlen(aux.payload + 1);
				(*(pkg*)aux.payload).packNum = NAKArray[myAux];
				(*(pkg*)aux.payload).messageType = TYPE_NACK;
			
				res = send_message(&aux);
				if (res < 0) {
					perror("[RECEIVER]Send NACK error. Exiting.\n");
					return -1;
				}
				break;}
			
			printf("[RECEIVER]Bizzare package received.\n");

		}
	}
		
	/* confirmare sfarsit date */
	sprintf((*(pkg*)aux.payload).payload, "ACK Pachet final");
	aux.len = strlen(aux.payload + 1);
	(*(pkg*)aux.payload).packNum = myPkg.packNum;	
	res = send_message(&aux);
	
	free(NAKArray);
	printf("[RECEIVER]All done.\n");
	close(fdOut);
	return 0;
}
