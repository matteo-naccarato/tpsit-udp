#include "includes/SocketUDP.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	/* PROCESSO FIGLIO */
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}

	printf("\n[%d] Sono il figlio\n", getpid());

	// assegnazione variabili di supporto
	int port = atoi(argv[1]);
	char* msg = argv[2];
	
	// creazione Address server
	Address addr_myself((char*)IP_DHCP, port);

	SocketUDP* myself = new SocketUDP(addr_myself);

	// ciclo per la ricezione e invio dei messaggi
	while(1) {	

		// creazione Address del destinatario
		Address dest;

		// ricezione messaggio
		char* request = strdup(myself->ricevi(&dest));

		if (request == NULL) errore((char*)"ricevi()", -3);
		printf("\n[%d] received from %s '%s'\n", getpid(), dest.toString(), request);
		free(request);

		// invio risposta
		int rc = myself->invia(dest, msg);
		if (rc != strlen(msg)+1) errore((char*)"invia()", -4);
		printf("[%d] sent to %s '%s'\n", getpid(), dest.toString(), msg);
	}
	
	delete(myself);

	return 0;
}