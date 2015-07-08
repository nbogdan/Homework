
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"



int Task1()
{
	pkg myPkg;
	msg t, aux;
	char* filename;

	int fdOut;
	int res, packNum = 0;
	
	printf("[RECEIVER]Receiver starts.\n");
	printf("[RECEIVER]Task index=%d\n", 2);

	if (recv_message(&t) < 0) {
		perror("[RECEIVER]Error receiving message");
		return -1;
	}
	/* asteptare fisier initializare */
	myPkg = *((pkg*)t.payload);
	printf("[RECEIVER] Got msg with payload: %s\n", myPkg.payload);
	
	
	if(myPkg.messageType != TYPE_INIT){
		perror("[RECEIVER]Didn't receive init file!\n");
	}
	sprintf(aux.payload, "ACK(%s)", myPkg.payload);
	aux.len = strlen(aux.payload + 1);
	send_message(&aux);
	/* initializare si deschidere fisier out */
	filename = strdup("recv_");
	strcat(filename, myPkg.payload);
	fdOut = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	
	/* scriere in fisier pe masura ce se primesc pachetele */
	printf("Commence filewriting.\n");
	while(1) {
		/* curatare msg */
		memset(&myPkg, 0, sizeof(pkg));
		memset(&t, 0, sizeof(msg));
		
		/* astept mesajul packNum */
		do
		{
			res = recv_message(&t);
			myPkg = *((pkg*)t.payload);
		}while(myPkg.packNum != packNum);
		
		/* incrementez numarul de ordine al mesajului asteptat */
		packNum++;
		fflush(stdin);
		printf("[RECEIVER]Received package: %i\n", myPkg.packNum);
		if (res < 0) {
			perror("[RECEIVER]Receive error. Exiting.\n");
			return -1;
		}
		if(myPkg.messageType != TYPE_INFO){
			printf("[RECEIVER]Successfully transfered all information.\n");
			break;
		}
		
		/* tiparire pachet primit */
		write(fdOut, myPkg.payload, t.len - 1);
		
		/* tirimitere ACK */
		sprintf(aux.payload, "ACK Pachet numarul: (%d)", myPkg.packNum);
		aux.len = strlen(aux.payload + 1);
		(*(pkg*)aux.payload).packNum = myPkg.packNum;
		
		res = send_message(&aux);
		if (res < 0) {
			perror("[RECEIVER]Send ACK error. Exiting.\n");
			return -1;
		}
	}
	
	/* trimitere pachet final de confirmare a incheierii transferului */
	sprintf((*(pkg*)aux.payload).payload, "ACK Pachet final");
	aux.len = strlen(aux.payload + 1);
	(*(pkg*)aux.payload).packNum = myPkg.packNum;	
	res = send_message(&aux);
	

	printf("[RECEIVER]All done.\n");
	close(fdOut);
	return 0;

}

