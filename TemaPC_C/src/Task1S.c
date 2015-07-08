#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

#include "lib.h"


int Task1(int speed, int delay, char* filename)
{
	pkg myPackage;
	msg* mesajDuplicat; /* mesaj duplicat folosit pentru salvarea elementelor in coada */
	msg t, aux;

	long bdp, window, winSave;
	int fdIn, count;
	char infoBuffer[PKGSIZE]; /* buffer citire din fisier */
	int i, packNum = 0, currentPackage = 0, timeout;
	TLista bufferMesaje; /* coada in care se retin mesajele neconfirmate */
	
	/* initializare variabila timeout */
	if(2 * delay > 1000)
		timeout = 2 * delay;
	else timeout = 1000;
	
	bdp = speed * delay * 1000;
	
	/* window = number of frames in the 'network', unacked */
	window = bdp / (FRAMESIZE * 8); 
	printf("[SENDER]Fereastra = %li cadre\n", window);

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
	memcpy(myPackage.payload, filename, strlen(filename));
	memcpy(t.payload, &myPackage, sizeof(pkg));
	
	t.len = strlen(myPackage.payload) + 1;
	

	printf("[SENDER]Sending init message.\n");
	if(send_message(&t)  < 0){
		fflush(stdin);
		perror("Error sending init message.\n");
		fflush(stdin);
		return -1;
	}
	
	/* salvez dimensiunea ferestrei; window urmeaza a fi decrementat */
	winSave = window;
	/* astept confirmarea initializarii; in caz de pierdere a mesajului, retrimit */
	while (recv_message_timeout(&aux, timeout) < 0) {
		fflush(stdin);
		printf("[SENDER] receive error\nWill attempt to resend.\n");
		fflush(stdin);
		send_message(&t);
	} 
	printf("[SENDER] Got reply with payload: %s\n", t.payload);
	
	
	/*trimitere efectiva mesaje*/
	printf("-----Commence filereading.-----\n");
	count = read(fdIn, infoBuffer, PKGSIZE);
	while(count > 0) {
		/* pregatire pachet date */
		memset(t.payload, 0, sizeof(t.payload));
		memset(myPackage.payload, 0, sizeof(myPackage.payload));
		
		/* actualizare tip pachet + numar de ordine */
		myPackage.messageType = TYPE_INFO;
		myPackage.packNum = packNum++;
		memcpy(myPackage.payload, infoBuffer, count);
		memcpy(t.payload, &myPackage, sizeof(myPackage));
		printf("[SENDER]Message numer %i has been sent.\n", myPackage.packNum);
		t.len = count + 1;
		
		/* creeare duplicat mesaj pentru a il salva in buffer */
		mesajDuplicat = mesageDup(&t);
		send_message(&t);
		InsSfL(&bufferMesaje, mesajDuplicat);
		/* mesajul este stocat in buffer pana ce va fi confirmat de receiver */

		/* daca fereastra este momentan plina */
		if(window == 0)
		{
			/* daca ACK-ul este primit la timp */
			if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* daca am primit ACK-ul mesajului dorit */
				if((*(pkg*)aux.payload).packNum == currentPackage){
					currentPackage++;
					}
				/* daca am primit ACK-ul unui alt mesaj */
				else 
				{
					/* retrimit toate mesajele pana ce primesc confirmarea pentru 
					currentPackage */
					printf("[SENDER]Didn't receive ACK for package: %i\n", currentPackage);
					do
					{
						printf("[SENDER]Attempting resend.\n");
						for(i = 0; i < winSave; i++)
						{
							send_message(getindex(&bufferMesaje,i));
						}
					}while((*(pkg*)aux.payload).packNum != currentPackage);
					currentPackage++;
					/* actualizare indice pentru pachetul curent a carui confirmare
					se asteapta */
				}
			}
			/* daca ACK-ul nu este primit la timp, retrimitem toate pachetele salvate */
			else {
				printf("[SENDER]Timeout reached while waiting for package: %i.\n", currentPackage);
				do {
					printf("[SENDER]Attempting resend.\n");
					for(i = 0; i < winSave; i++)
					{
						send_message(getindex(&bufferMesaje,i));
					}	
				}while(recv_message_timeout(&aux, timeout) < 0 || 
					(*(pkg*)aux.payload).packNum != currentPackage);
				currentPackage++;
			}
					
			/* daca currentPackage a fost confirmat, il eliminam din coada si continuam */
			fflush(stdin);
			printf("[SENDER]Received confirmation for package: %i.\n", currentPackage - 1);
			fflush(stdin);
			ElimPL(&bufferMesaje);
			
		}
		else window--;
		
		/* citim date pentru urmatorul pachet */
		count = read(fdIn, infoBuffer, PKGSIZE);
	}
	
	/* mesaj confirmare sfarsit date */
	printf("[SENDER]Filereading has ended.\n");
	myPackage.messageType = TYPE_END;
	myPackage.packNum = packNum++;
	memcpy(t.payload, &myPackage, sizeof(myPackage));
	mesajDuplicat = mesageDup(&t);
	send_message(&t);
	InsSfL(&bufferMesaje, mesajDuplicat);
	send_message(&t);
	
	/* cat timp bufferul de mesaje nu este gol */
	/* NOTA: codul(modul de abordare) este aproape identic cu cel de mai sus */
	printf("[SENDER]Will attempt resend for all the messages left in the buffer.\n");
	while(ListLength(&bufferMesaje))
	{
		/* daca ACK-ul este primit la timp */
		if(recv_message_timeout(&aux, timeout) > 0)
			{
				/* daca s-a confirmat pachetul corect(urmatorul in ordine) */
				if((*(pkg*)aux.payload).packNum == currentPackage){
					currentPackage++;
					}
				/* daca s-a confirmat un alt pachet*/
				else 
				{
					/* retrimitem tot buffer-ul pana primim confirmarea dorita */
					printf("[SENDER]Didn't receive ACK for package: %i\n", currentPackage);
					do
					{
						printf("[SENDER]Attempting resend.\n");
						for(i = 0; i < ListLength(&bufferMesaje); i++)
						{
							send_message(getindex(&bufferMesaje,i));
						}
					}while((*(pkg*)aux.payload).packNum != currentPackage);
					currentPackage++;
				}
			}
		/* daca nu s-a primit ACK la timp */
		else {
			/* retrimitem mesajele din buffer pana primim confirmare */
			printf("[SENDER]Timeout reached while waiting for package: %i.\n", currentPackage);
			do {
				printf("[SENDER]Attempting resend.\n");
				for(i = 0; i < ListLength(&bufferMesaje); i++)
				{
					send_message(getindex(&bufferMesaje,i));
				}	
			}while(recv_message_timeout(&aux, timeout) < 0 || 
			(*(pkg*)aux.payload).packNum != currentPackage);
			
			currentPackage++;
			}
		
				
		fflush(stdin);
		printf("[SENDER]Received confirmation for package: %i.\n", currentPackage - 1);
		fflush(stdin);		
		ElimPL(&bufferMesaje);/* eliminare mesaj confirmat din buffer */	
	}
	printf("[SENDER]Job done.\n");
	
	close(fdIn);
	return 0;
}
