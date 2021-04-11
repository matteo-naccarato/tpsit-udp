#include "includes/SocketUDP.hpp"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define PROMPT "$>"
#define EXIT_CMD "quit"

// struttura parametri thread
typedef struct {
	SocketUDP* myself;
	Address broadcast;
} Params;


void* th_ricevi(void*);
void* th_invia(void*);


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 3) {
		printf("USAGE: %s BROADCAST PORT\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	char* br = argv[1];
	int port = atoi(argv[2]);

	Address addr_myself((char*)IP_DHCP, port);
	Address broadcast((char*)br, port);

	SocketUDP* sock_myself = new SocketUDP(addr_myself);
	int rc =  sock_myself->broadcast(true);
	if (rc) errore((char*)"broadcast()", -2);
	Params p = { sock_myself, broadcast };

	// creazione thread ricezione
	pthread_t thread_ricevi_id;
	rc = pthread_create(&thread_ricevi_id, NULL, th_ricevi, (void*) sock_myself);
	if (rc != 0) errore((char*)"pthread_create()", -3);

	// creazione thread invio
	pthread_t thread_invia_id;
	rc = pthread_create(&thread_invia_id, NULL, th_invia, (void*) &p);
	if (rc != 0) errore((char*)"pthread_create()", -4); 

	pthread_join(thread_invia_id, NULL);

	//delete sock_myself;

	return 0;
}

void* th_invia(void* param) {

	Params* p = (Params*) param;

	SocketUDP* myself = p->myself;
	Address broadcast = p->broadcast;

	char* cmd = inputStr();
	while(strcmp(cmd, EXIT_CMD)) {

		int rc = myself->invia(broadcast, cmd);
		printf("[you] said '%s'\n", cmd);
		if (rc != strlen(cmd)+1) errore((char*)"invia()", -5);
		free(cmd);
		cmd = inputStr();
	}
	free(cmd);

	return NULL;
}


void* th_ricevi(void* param) {

	// assegnazione SocketUDP
	SocketUDP* myself = (SocketUDP*) param;
	
	// ciclo per la ricezione e invio dei messaggi
	while(1) {
		// creazione Address del mittente
		Address mitt;

		// ricezione messaggio
		char* request = strdup(myself->ricevi(&mitt));
		if (request == NULL) errore((char*)"ricevi()", -6);
		printf("%s said: '%s'\n", mitt.toString(), request);
		free(request);
	}

	delete(myself);

	pthread_exit(NULL);
}