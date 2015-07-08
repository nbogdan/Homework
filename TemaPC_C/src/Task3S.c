#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"

/* Functia care descrie comportamentul sender-ului petru task-ul 3 */

int Task3(int speed, int delay, char* filename)
{
	pkg myPackage;
	msg* haos;
	msg t, aux;

	long bdp, window;
	int fdIn, count;
	char infoBuffer[PKGSIZE]; /* buffer citire fisier */
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
	printf("[SENDER]Filename=%s, task_index=%d, speed=%d, delay=%d\n", filename, 2, speed, delay);
	
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
	
	/* calcul suma de control */
	myPackage.checkSum = 0;
	memcpy(myPackage.payload, filename, strlen(filename));
	memcpy(t.payload, &myPackage, sizeof(pkg));
	(*(pkg*)t.payload).checkSum = msgCheckSum(t);

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
	printf("[SENDER] Got reply with payload: %s\n", t.payload);
	
	
	/* trimitere efectiva mesaje */
	window--;/* se vor trimite mesajele de la 0 la window - 1*/
	printf("-----Commence filereading.-----\n");
	count = read(fdIn, infoBuffer, PKGSIZE);
	while(count > 0) {
		/* pregatire pachet date */
		memset(t.payload, 0, sizeof(t.payload));
		memset(myPackage.payload, 0, sizeof(myPackage.payload));
		/* actualizare tip pachet + numar de ordine */
		myPackage.messageType = TYPE_INFO;
		myPackage.packNum = packNum++;
		if(count < PKGSIZE)
			myPackage.messageType = TYPE_END;
		myPackage.checkSum = 0;
		memcpy(myPackage.payload, infoBuffer, count);
		memcpy(t.payload, &myPackage, sizeof(myPackage));
		(*(pkg*)t.payload).checkSum = msgCheckSum(t);
		printf("[SENDER]Message number %i has been sent.\n", myPackage.packNum);
		t.len = count + 1;
		
		/* creare duplicat mesaj pentru a il salva in buffer */
		haos = mesageDup(&t);
		send_message(&t);
		InsSfL(&bufferMesaje, haos);
		
		if(window == 0)
		{
			/* daca (N)ACK-ul este primit la timp */
			if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* cat timp primim NACK-uri, retrimitem pachetele in cauza */
				while((*(pkg*)aux.payload).messageType == TYPE_NACK)
				{
					/* daca s-a primit NACK pentru un pachet care inca nu a 
					fost trimis */
					if(k >= packNum)
						break;
					/* retinem numarul de ordine al pachetului pentru care 
					s-a primit NACK*/
					k = (*(pkg*)aux.payload).packNum;
					printf("[SENDER]Message %i has been resent.\n", k);
					
					/* retrimit pachetul */
					res = send_message(getPackNum(&bufferMesaje, k));
					if(res < 0){
						perror("RESEND error\n");
						return -1;
					}
					recv_message_timeout(&aux, timeout);
				}	
				
				/* elimin pachetul confirmat din buffer */
				printf("[SENDER]Eliminating package %i from queue.\n", (*(pkg*)aux.payload).packNum);
				if(ElimPackNum(&bufferMesaje,(*(pkg*)aux.payload).packNum) < 0)
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
					for(i = 0; i < ListLength(&bufferMesaje); i++)
					{
						send_message(getindex(&bufferMesaje,i));
					}	
				}while(recv_message_timeout(&aux, timeout) < 0);

				}
		}
		else window--;
		count = read(fdIn, infoBuffer, PKGSIZE);
	}
	
	/* s-a incheiat citirea din fisier; trimitem un pachet final */
	printf("[SENDER]Filereading has ended. Last file is: %i\n", packNum);
	myPackage.messageType = TYPE_END;
	myPackage.packNum = packNum++;
	myPackage.checkSum = 0;
	memcpy(t.payload, &myPackage, sizeof(myPackage));
	(*(pkg*)t.payload).checkSum = msgCheckSum(t);
	t.len = 0;
	haos = mesageDup(&t);
	send_message(&t);
	InsSfL(&bufferMesaje, haos);
	send_message(&t);
	
	/* asteptam sa primim confirmarea tuturor pachetelor trimise */
	/* NOTA: mod de operare similar cu ce avem mai sus */
	printf("[SENDER]Will attempt resend for all the messages left in the buffer.\n");
	while(ListLength(&bufferMesaje) >= 1)
	{
			/* daca (N)ACK-ul este primit la timp */
			if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* cat timp primim NACK-uri, retrimitem pachetele aferente */
				while((*(pkg*)aux.payload).messageType != TYPE_ACK)
				{
					printf("[SENDER]Message %i has been resent.\n", k);
					k = (*(pkg*)aux.payload).packNum;
					
					if(k >= packNum)
						break;
					
					res = send_message(getPackNum(&bufferMesaje, k));
					
					if(res < 0){

						AfisareL(&bufferMesaje);
						printf("[SENDER]RESEND error.\n");
						return -1;
					}
					recv_message_timeout(&aux, timeout);
				}	
				/* cand am primit un ACK, eliminam pachetul respectiv din buffer*/
				printf("[SENDER]Eliminating package %i from queue.\n", (*(pkg*)aux.payload).packNum);
				if(ElimPackNum(&bufferMesaje,(*(pkg*)aux.payload).packNum) < 0)
				{
				/* eroare care nu afecteaza puternic activitatea programului, 
				dar trebuie semnalata */
					printf("[SENDER]Error eliminating package.\n");	
				}
			}
				
			else {
				printf("[SENDER]Timeout reached.\n");
					
				printf("[SENDER]Attempting resend.\n");
				for(i = 0; i < ListLength(&bufferMesaje); i++)
				{
					res = send_message(getindex(&bufferMesaje,i));
						
					if(res < 0){

						AfisareL(&bufferMesaje);
						printf("[SENDER]RESEND error.\n");
						return -1;
					}
				}	
				//}while(recv_message_timeout(&aux, timeout) < 0 || j < 2);
					
				}
			/* daca a mai ramas doar mesajul de final, ne oprim */
			if(ListLength(&bufferMesaje) == 1)
				if( (*(pkg*)(bufferMesaje->info).payload).messageType == TYPE_END)
					break;
	}
	printf("[SENDER]Job done.\n");
	
	close(fdIn);
	return 0;
}

