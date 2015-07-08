#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"
#include "Auxiliary2.c"

/* Functia care descrie comportamentul sender-ului petru task-ul 4 */

int Task4(int speed, int delay, char* filename)
{
	pkgbonus myPackage;
	msg* auxPackage;
	msg t, aux;

	long bdp, window;
	int fdIn, count;
	char infoBuffer[PKGSIZE2]; /* buffer citire fisier */
	int i, packNum = 0, timeout;
	TLista bufferMesaje; /* buffer pachete */
	int k, res;
	
	/* setare varibaila timeout */
	if(2 * delay > 1000) 
		timeout = 2 * delay;
	else timeout = 1000;
	
	bdp = speed * delay * 1000;
 
 	window = bdp / (FRAMESIZE * 8); 
	printf("[SENDER]Window = %ld frames\n", window);

	
	printf("[SENDER]Sender starts.\n");
	printf("[SENDER]Filename=%s, task_index=%d, speed=%d, delay=%d\n", filename, 4, speed, delay);
	
	/* deschidere fisier */
	fdIn = open(filename, O_RDONLY);
	if(fdIn < 0){
		perror("File openning error.\n");
		return -1;
	}

	
	/* pachet initializare retea */
	memset(t.payload, 0, sizeof(t.payload));
	memset(myPackage.payload, 0, sizeof(myPackage.payload));
	
	myPackage.messageType = TYPE_INIT;	
	myPackage.packNum = window;
	
	memcpy(myPackage.payload, filename, strlen(filename));
	memcpy(t.payload, &myPackage, sizeof(pkgbonus));
	/* initializez correctorBytes */
	memset(t.payload, 0, CORRECTORSIZE);
	/* plasez correctorBytes in pozitiile potrivite(puteri ale lui 2) */
	swapBytes(&t);
	/* le actualizez valorile */
	calculateHamming(&t);
	/* revin la pozitiile initiale */
	unSwapBytes(&t);

	t.len = strlen(myPackage.payload) + 1;
	
	printf("[SENDER]Sending init message.\n");
	if(send_message(&t)  < 0){
		fflush(stdin);
		perror("Error sending init message.\n");
		fflush(stdin);
		return -1;
	}

	/* retrimit pachetul pana ce sunt sigur ca si receiverul este initializat */
	while (recv_message_timeout(&aux, timeout) < 0) {
		fflush(stdin);
		printf("[SENDER] receive error\nWill attempt to resend.\n");
		fflush(stdin);
		send_message(&t);
	} 
	
	
	/* trimitere efectiva mesaje */
	window--;/* se vor trimite mesajele de la 0 la window - 1*/
	printf("-----Commence filereading.-----\n");
	count = read(fdIn, infoBuffer, PKGSIZE2);
	while(count > 0) {
		/* pregatire pachet date */
		memset(t.payload, 0, sizeof(t.payload));
		memset(myPackage.payload, 0, sizeof(myPackage.payload));
		/* actualizare tip pachet + numar de ordine */
		myPackage.messageType = TYPE_INFO;
		myPackage.packNum = packNum++;
		if(count < PKGSIZE2)
			myPackage.messageType = TYPE_END;

		memcpy(t.payload, &myPackage, sizeof(pkgbonus));
		
		/* acelasi procedeu descris anterior */
		memset(t.payload, 0, CORRECTORSIZE);
		swapBytes(&t);
		calculateHamming(&t);
		unSwapBytes(&t);
	
		printf("[SENDER]Message number %i has been sent.\n", (*(pkgbonus*)t.payload).packNum);
		t.len = count + 1;
		
		/* creare duplicat mesaj pentru a il salva in buffer */
		auxPackage = mesageDup2(&t);
		send_message(&t);
		InsSfL2(&bufferMesaje, auxPackage);
		
		if(window == 0)
		{
		AfisareL2(&bufferMesaje);
			/* daca (N)ACK-ul este primit la timp */
			if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* cat timp primim NACK-uri, retrimitem pachetele in cauza */
				while((*(pkgbonus*)aux.payload).messageType == TYPE_NACK)
				{
					/* daca s-a primit NACK pentru un pachet care inca nu a 
					fost trimis */
					if(k >= packNum)
						break;
					/* retinem numarul de ordine al pachetului pentru care 
					s-a primit NACK*/
					k = (*(pkgbonus*)aux.payload).packNum;
					printf("[SENDER]Message %i has been resent.\n", k);
					
					/* retrimit pachetul */
					res = send_message(getPackNum2(&bufferMesaje, k));
					if(res < 0){
						perror("RESEND error\n");
						return -1;
					}
					recv_message_timeout(&aux, timeout);
				}	
				
				/* elimin pachetul confirmat din buffer */
				printf("[SENDER]Eliminating package %i from queue.\n", (*(pkgbonus*)aux.payload).packNum);
				if(ElimPackNum2(&bufferMesaje,(*(pkgbonus*)aux.payload).packNum) < 0)
				{
					printf("[SENDER]Error eliminating package.\n");	
					return -1;
				}
			}
			
			/* Daca timeout-ul a fost atins, retrimitem tot ce avem in buffer*/
			else {
					printf("[SENDER]Timeout reached.\n");
				
				do {
					printf("[SENDER]Attempting resend.\n");
					for(i = 0; i < ListLength2(&bufferMesaje); i++)
					{
						send_message(getindex(&bufferMesaje,i));
					}	
				}while(recv_message_timeout(&aux, timeout) < 0);

				}
		}
		else window--;
		count = read(fdIn, infoBuffer, PKGSIZE2);
	}
	
	/* s-a incheiat citirea din fisier; trimitem un pachet final */
	printf("[SENDER]Filereading has ended. Last file is: %i\n", packNum);
	myPackage.messageType = TYPE_END;
	myPackage.packNum = packNum++;
	
	memcpy(t.payload, &myPackage, sizeof(pkgbonus));

	/* calculez si salvez bitii corectori */
	memset(t.payload, 0, CORRECTORSIZE);	
	swapBytes(&t);
	calculateHamming(&t);
	unSwapBytes(&t);

	t.len = 0;
	auxPackage = mesageDup(&t);
	send_message(&t);
	InsSfL2(&bufferMesaje, auxPackage);
	send_message(&t);
	AfisareL2(&bufferMesaje);
	/* asteptam sa primim confirmarea tuturor pachetelor trimise */
	/* NOTA: mod de operare similar cu ce avem mai sus */
	printf("[SENDER]Will attempt resend for all the messages left in the buffer.\n");
	while(ListLength2(&bufferMesaje) >= 1)
	{
			/* daca (N)ACK-ul este primit la timp */
			if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* cat timp primim NACK-uri, retrimitem pachetele aferente */
				while((*(pkgbonus*)aux.payload).messageType != TYPE_ACK)
				{
					printf("[SENDER]Message %i has been resent.\n", k);
					k = (*(pkgbonus*)aux.payload).packNum;
					
					if(k >= packNum)
						break;
					
					res = send_message(getPackNum2(&bufferMesaje, k));
					
					if(res < 0){

						AfisareL2(&bufferMesaje);
						printf("[SENDER]RESEND error.\n");
						return -1;
					}
					recv_message_timeout(&aux, timeout);
				}	
				/* cand am primit un ACK, eliminam pachetul respectiv din buffer*/
				printf("[SENDER]Eliminating package %i from queue.\n", (*(pkgbonus*)aux.payload).packNum);
				if(ElimPackNum2(&bufferMesaje,(*(pkgbonus*)aux.payload).packNum) < 0)
				{
				/* eroare care nu afecteaza puternic activitatea programului, 
				dar trebuie semnalata */
					printf("[SENDER]Error eliminating package.\n");	
				}
			}
				
			else {
					printf("[SENDER]Timeout reached.\n");
				}
			/* daca a mai ramas doar mesajul de final, ne oprim */
			if(ListLength2(&bufferMesaje) == 1)
				if( (*(pkgbonus*)(bufferMesaje->info).payload).messageType == TYPE_END)
					break;
	}
	printf("[SENDER]Job done.\n");
	
	close(fdIn);
	return 0;
}

