#include "includes/SocketUDP.hpp"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define PROMPT "$>"
#define EXIT_CMD "quit"

// struttura parametri thread
typedef struct {
	SocketUDP* myself;
	char* msg;
} Params;


void* my_thread(void*);


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	int port = atoi(argv[1]);
	char* msg = argv[2];

	// creazione Address server
	Address addr_myself((char*)IP_DHCP, port);
	// parametri thread con la creazione SocketUDP del server
	Params p = {new SocketUDP(addr_myself), msg};
	// creazione thread
	pthread_t thread_id;
	int rc = pthread_create(&thread_id, NULL, my_thread, (void*) &p);
	if (rc != 0) errore((char*)"pthread_create()", -2);

	// gestione input da tastiera
	printf("%s ", PROMPT);
	char* command = inputStr();
	while(strcmp(command, EXIT_CMD)) {
		free(command);
		command = inputStr();
	}
	free(command);

	return 0;
}


void* my_thread(void* param) {

	Params* p = (Params*) param;

	// assegnazione SocketUDP del server
	SocketUDP* myself = p->myself;
	char* msg = p->msg;

	// ciclo per la ricezione e invio dei messaggi
	while(1) {

		// creazione Address del destinatario
		Address dest;

		// ricezione messaggio
		char* request = myself->ricevi(&dest);
		if (request == NULL) errore((char*)"ricevi()", -3);
		printf("received from %s '%s'\n", dest.toString(), request);
		free(request);

		// invio risposta
		int rc = myself->invia(dest, msg);
		if (rc != strlen(p->msg)+1) errore((char*)"invia()", -4);
		printf("%s sent to %s '%s'\n", PROMPT, dest.toString(), msg);
		printf("%s ", PROMPT);
		fflush(stdout);
	}

	delete(myself);

	pthread_exit(NULL);
}