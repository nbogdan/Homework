#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>


#include "lib.h"

/* Functia care se ocupa cu trimiterea pachetelor pentru task-ul 0. */

int Task0(int speed, int delay, char* filename)
{
	pkg myPackage;/* pachet informatii */
	msg t, aux;/* mesaj primire informatii, mesaj auxiliar pentru confirmari */
	long bdp, window, winSave;
	int fdIn;
	int count;/* variabila in care stocam numar octeti transmisi */
	char infoBuffer[PKGSIZE];/* buffer informatie pentru un pachet */
	int i;
	int packNum = 0;/* variabila pentru numararea pachetelor trimise */
	
	bdp = speed * delay * 1000;
	
	window = bdp / (FRAMESIZE * 8); 
	printf("[SENDER]Fereastra = %li cadre\n", window);
	
	printf("[SENDER]Sender starts.\n");
	printf("[SENDER]Filename=%s, task_index=%d, speed=%d, delay=%d\n", filename, 0, speed, delay);
	
	/*deschidere fisier*/
	fdIn = open(filename, O_RDONLY);
	if(fdIn < 0)
		perror("File openning error.\n");
	
	/* pregatire pachet initializare retea */
	memset(t.payload, 0, sizeof(t.payload));
	memset(myPackage.payload, 0, sizeof(myPackage.payload));
	
	myPackage.messageType = TYPE_INIT;	
	myPackage.packNum = window; //pachetul de initializare trimite dimensiunea ferestrei(just in case)
	memcpy(myPackage.payload, filename, strlen(filename));
	memcpy(t.payload, &myPackage, sizeof(pkg));
	
	t.len = strlen(myPackage.payload) + 1;
	send_message(&t);

	/* salvare dimensiune fereastra */
	winSave = window;
	while (recv_message(&aux) < 0) {
		printf("[SENDER] receive error\nWill attempt to resend.");
		send_message(&t);
	} 
	printf("[SENDER] Got reply with payload: %s\n", t.payload);
	/* Avansam doar dupa ce primim confirmarea initializarii corecte a receiver-ului */
	
	
	/* trimitere efectiva mesaje */
	printf("Commence filereading.\n");
	count = read(fdIn, infoBuffer, PKGSIZE);
	while(count > 0) {
		/* trimitem mesaj */
		memset(t.payload, 0, sizeof(t.payload));
		memset(myPackage.payload, 0, sizeof(myPackage.payload));
		
		myPackage.messageType = TYPE_INFO;
		myPackage.packNum = packNum++;
		memcpy(myPackage.payload, infoBuffer, count);
		memcpy(t.payload, &myPackage, sizeof(myPackage));
		printf("[SENDER]Message type %i\n", myPackage.messageType);
		t.len = count + 1;

		
		send_message(&t);

		/* Daca fereastra este plina se asteapta confirmare */
		if(window == 0)
		{
			if (recv_message(&aux) < 0) {
				perror("[SENDER] Receive error\n");
				return -1;
			}
		
			printf("[SENDER]Received conf: %s\n", aux.payload);
		}
		else window--;
		count = read(fdIn, infoBuffer, PKGSIZE);
	}
	
	/* trimitem pachet care confirma incheierea transmisiei de date */
	myPackage.messageType = TYPE_END;
	memcpy(t.payload, &myPackage, sizeof(myPackage));
	send_message(&t);
	
	for(i = 0; i <= winSave; i++)
	{
		if (recv_message(&aux) < 0) {
			perror("[SENDER] Receive error\n");
			return -1;
		}		
		printf("[SENDER]Received conf: %s\n", aux.payload );
	}
	printf("[SENDER] Job done.\n");
	
	close(fdIn);

	return 1;
}
