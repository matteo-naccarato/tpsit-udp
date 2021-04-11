#include "includes/SocketUDP.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 4) {
		printf("USAGE: %s IP PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	char* ip = argv[1];
	int port = atoi(argv[2]);
	char* msg = argv[3];

	// creazione SocketUDP del client
	SocketUDP* myself = new SocketUDP();

	// creazione Address del server
	Address server(ip, port);

	// invio messaggio al server
	int rc = myself->invia(server, msg);
	if (rc != strlen(msg)+1) errore((char*)"invia()", -2);
	printf("sent to %s '%s'\n", server.toString(), msg);

	// ricezione messaggio dal server
	char* resp = strdup(myself->ricevi(&server));
	if (resp == NULL) errore((char*)"ricevi()", -3);
	printf("received from %s '%s'\n", server.toString(), resp);
	free(resp);

	return 0;
}