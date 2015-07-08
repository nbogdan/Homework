#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include "lib.h"


/* Functia care "rezolva" practic task-ul numarul 0. */

int Task0()
{
	pkg myPkg;/* pachet informatii */
	msg t, aux;/* mesaj primire informatii, mesaj auxiliar pentru confirmari */
	int fdOut;/* indice scriere fisier */
	int res;
	char* filename;
	
	printf("[RECEIVER]Receiver starts.\n");
	printf("[RECEIVER]Task index=%d\n", 0);
		

	if (recv_message(&t) < 0) {
		perror("[RECEIVER]Error receiving message\n");
		return -1;
	}
	/* Initializare si deschidere fisier out. */
	// primire pachet initializare
	myPkg = *((pkg*)t.payload);
	printf("[RECEIVER] Got msg with payload: %s\n", myPkg.payload);
	if(myPkg.messageType != TYPE_INIT){
		perror("Didn't receive init file!\n");
	}
	sprintf(aux.payload, "ACK(%s)", myPkg.payload);
	aux.len = strlen(aux.payload + 1);
	send_message(&aux);// confirmare pachet initializare
	
	filename = strdup("recv_");
	strcat(filename, myPkg.payload);
	
	fdOut = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	
	/* scriere in fisier pe masura ce se primesc pachetele */
	printf("Commence filewriting.\n");
	while(1) {
		/* curatare msg */
		memset(&myPkg, 0, sizeof(pkg));
		memset(&t, 0, sizeof(msg));
		
		/* asteapta mesaj */
		res = recv_message(&t);
		myPkg = *((pkg*)t.payload);
		
		fflush(stdin);
		printf("[RECEIVER]Received package: %d\n", myPkg.packNum);
		if (res < 0) {
			perror("[RECEIVER] Receive error. Exiting.\n");
			return -1;
		}
		if(myPkg.messageType != TYPE_INFO){
			printf("Successfully transfered all information.\n");
			break;
		}
		
		/* scriere pachet in fisier */
		write(fdOut, myPkg.payload, t.len - 1);
		
		/* send dummy ACK */
		sprintf(aux.payload, "ACK Pachet numarul: (%d)", myPkg.packNum);
		aux.len = strlen(aux.payload + 1);
		
		res = send_message(&aux);
		if (res < 0) {
			perror("[RECEIVER]Send ACK error. Exiting.\n");
			return -1;
		}
	}
	
	/* Trimitere ACK final */	
	sprintf(aux.payload, "ACK Pachet final");
	aux.len = strlen(aux.payload + 1);
	res = send_message(&aux);
	

	printf("[RECEIVER]All done.\n");
	close(fdOut);

	return 1;
}

