#include "includes/Address.hpp"
#include "includes/utilities.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define PROMPT "$>"
#define EXIT_CMD "quit"

void* my_thread(void*);

// struttura parametri thread
typedef struct {
	char* msg;
	int sock_id;
	Address dest;
} Params;


int main(int argc, char* argv[]) {

	// controllo parametri
	if (argc != 3) {
		printf("USAGE: %s PORT MESSAGE\n", argv[0]);
		return -1;
	}

	// assegnazione variabili di supporto
	int port = atoi(argv[1]);
	char* msg = argv[2];

	// creazione socket
	int socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	if (socket_id == -1) errore((char*)"socket()", -2);

	// creazione Address del server
	Address myself((char*)IP_DHCP, port);
	// bind server
	int rc = bind(socket_id,
					(struct sockaddr*) myself.getAddress(),
					(socklen_t) sizeof(struct sockaddr_in));
	if (rc != 0) errore((char*)"bind()", -3);

	// creazione thread
	pthread_t thread_id;
	Params p = {msg, socket_id, myself};
	rc = pthread_create(&thread_id, NULL, my_thread, (void*) &p);
	if (rc != 0) errore((char*)"pthread_create()", -4);

	// gestione input da tastiera
	printf("%s ", PROMPT);
	char* command = inputStr();
	while(strcmp(command, EXIT_CMD)) {
		free(command);
		command = inputStr();
	}
	free(command);

	// chiusura socket
	close(socket_id);

	return 0;
}


void* my_thread(void* param) {

	// ciclo per la ricezione e invio dei messaggi
	while(1) {

		Params* p = (Params*) param;

		// creazione Address del destinatario
		Address* destinatario = new Address(p->dest);

		struct sockaddr_in* dest_addr = destinatario->getAddress();
		int addr_len = sizeof(struct sockaddr);
		char buffer[MAX_STR + 1];
		// ricezione con la recvfrom()
		int rc = recvfrom(p->sock_id,
							buffer,
							MAX_STR,
							0,
							(struct sockaddr*) dest_addr,
							(socklen_t*)&addr_len);
		if (rc <= 0) errore((char*)"recvfrom()", -5);
		buffer[rc] = '\0';
		printf("received from %s '%s'\n", destinatario->toString(), buffer);

		// invio risposta con la sendto()
		rc = sendto(p->sock_id,
					p->msg,
					strlen(p->msg)+1,
					0,
					(struct sockaddr*) dest_addr,
					(socklen_t) sizeof(struct sockaddr_in));
		if (rc != strlen(p->msg)+1) errore((char*)"sendto()", -6);
		printf("%s sent to %s '%s'\n", PROMPT, destinatario->toString(), p->msg);
		printf("%s ", PROMPT);
		fflush(stdout);

		delete destinatario;
	}

	return NULL;
}